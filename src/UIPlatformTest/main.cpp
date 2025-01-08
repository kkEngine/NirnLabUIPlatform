#include "PCH.h"
#include "TestCases/TestCases.hpp"

bool g_canUseAPI = false;

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
    SKSE::PluginVersionData v{};
    v.pluginVersion = 1;
    v.PluginName(PLUGIN_NAME);
    v.AuthorName("kkEngine"sv);
    v.CompatibleVersions({SKSE::RUNTIME_SSE_1_6_640, REL::Version(1, 6, 1170, 0)});
    v.UsesAddressLibrary(true);
    v.UsesStructsPost629(true);
    return v;
}();

void InitLog()
{
#ifdef _DEBUG
    const auto level = spdlog::level::trace;
    auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
    const auto level = spdlog::level::info;
    auto path = logger::log_directory();
    if (!path)
    {
        SKSE::stl::report_and_fail("Failed to find standard logging directory"sv);
    }

    *path /= fmt::format("{}.log"sv, PLUGIN_NAME);
    auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

    auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
    log->set_level(level);
    log->flush_on(level);

    spdlog::set_default_logger(std::move(log));
    spdlog::set_pattern("[%T.%e] [%^%l%$] : %v"s);
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
    if (a_skse->IsEditor())
    {
        return false;
    }

    // SKSE
    SKSE::Init(a_skse);
    SKSE::AllocTrampoline(1024);
    InitLog();
    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* a_msg) {
        switch (a_msg->type)
        {
        case SKSE::MessagingInterface::kPostPostLoad:
            // All plugins are loaded. Request lib version.
            SKSE::GetMessagingInterface()->Dispatch(NL::UI::APIMessageType::RequestVersion, nullptr, 0, NL::UI::LibVersion::PROJECT_NAME);
            break;
        case SKSE::MessagingInterface::kInputLoaded:
            if (g_canUseAPI)
            {
                NL::UI::Settings defaultSettings;
                // API version is ok. Request interface.
                SKSE::GetMessagingInterface()->Dispatch(NL::UI::APIMessageType::RequestAPI, &defaultSettings, sizeof(defaultSettings), NL::UI::LibVersion::PROJECT_NAME);
            }
            break;
        default:
            break;
        }
    });
    SKSE::GetMessagingInterface()->RegisterListener(NL::UI::LibVersion::PROJECT_NAME, [](SKSE::MessagingInterface::Message* a_msg) {
        spdlog::info("Received message({}) from \"{}\"", a_msg->type, a_msg->sender ? a_msg->sender : "nullptr");
        switch (a_msg->type)
        {
        case NL::UI::APIMessageType::ResponseVersion: {
            const auto versionInfo = reinterpret_cast<NL::UI::ResponseVersionMessage*>(a_msg->data);
            spdlog::info("NirnLabUIPlatform version: {}.{}", NL::UI::LibVersion::GetMajorVersion(versionInfo->libVersion), NL::UI::LibVersion::GetMinorVersion(versionInfo->libVersion));

            const auto majorAPIVersion = NL::UI::APIVersion::GetMajorVersion(versionInfo->apiVersion);
            // If the major version is different from ours, then using the API may cause problems
            if (majorAPIVersion != NL::UI::APIVersion::MAJOR)
            {
                g_canUseAPI = false;
                spdlog::error("Can't using this API version of NirnLabUIPlatform. We have {}.{} and installed is {}.{}",
                              NL::UI::APIVersion::MAJOR,
                              NL::UI::APIVersion::MINOR,
                              NL::UI::APIVersion::GetMajorVersion(versionInfo->apiVersion),
                              NL::UI::APIVersion::GetMinorVersion(versionInfo->apiVersion));
            }
            else
            {
                g_canUseAPI = true;
                spdlog::info("API version is ok. We have {}.{} and installed is {}.{}",
                             NL::UI::APIVersion::MAJOR,
                             NL::UI::APIVersion::MINOR,
                             NL::UI::APIVersion::GetMajorVersion(versionInfo->apiVersion),
                             NL::UI::APIVersion::GetMinorVersion(versionInfo->apiVersion));
            }
            break;
        }
        case NL::UI::APIMessageType::ResponseAPI: {
            auto api = reinterpret_cast<NL::UI::ResponseAPIMessage*>(a_msg->data)->API;
            if (api == nullptr)
            {
                spdlog::error("API is nullptr");
                break;
            }
            NL::UI::TestCase::StartTests(api);
            break;
        }
        default:
            break;
        }
    });

    const auto iniCollection = RE::INISettingCollection::GetSingleton();
    // [General]
    // Don't stop game when window is collapsed
    iniCollection->GetSetting("bAlwaysActive:General")->data.b = true;

    return true;
}
