#include "Controllers/PublicAPIController.h"

inline void ShowMessageBox(const char* a_msg)
{
    MessageBoxA(0, a_msg, "ERROR", MB_ICONERROR);
}

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

    *path /= fmt::format("{}.log"sv, LibVersion::PROJECT_NAME);
    auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

    auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
    log->set_level(level);
    log->flush_on(level);

    spdlog::set_default_logger(std::move(log));
    spdlog::set_pattern("[%T.%e] [%^%l%$] : %v"s);
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
    SKSE::PluginVersionData v{};
    v.pluginVersion = LibVersion::AS_INT;
    v.PluginName(LibVersion::PROJECT_NAME);
    v.AuthorName("kkEngine"sv);
    v.CompatibleVersions({REL::Version(1, 6, 1170, 0)});
    v.UsesAddressLibrary(true);
    v.UsesStructsPost629(true);
    return v;
}();

#pragma region Tests

#include "PCH.h"
#include "Utils/InputConverter.h"
#include "Providers/ICEFSettingsProvider.h"
#include "Providers/DefaultCEFSettingsProvider.h"
#include "Services/UIPlatformService.h"
#include "Services/CEFService.h"
#include "Menus/MultiLayerMenu.h"
#include "Menus/CEFMenu.h"
#include "Hooks/WinProcHook.h"
#include "JS/JSFunctionStorage.h"

void BuildTestMenu()
{
    const auto logger = spdlog::default_logger();
    const auto cefSettingsProvider = std::make_shared<NL::Providers::DefaultCEFSettingsProvider>();
    const auto cefService = std::make_shared<NL::Services::CEFService>(logger, cefSettingsProvider);

    if (!NL::Services::UIPlatformService::GetSingleton().Init(logger, cefService))
    {
        logger->error("Failed to init platform service");
        return;
    }

    auto msgQ = RE::UIMessageQueue::GetSingleton();
    if (msgQ)
    {
        msgQ->AddMessage(NL::Menus::MultiLayerMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, NULL);
    }

    // const auto cefMenu = std::make_shared<NL::Menus::CEFMenu>(spdlog::default_logger(), cefService, L"https://youtu.be/YPKhOyM1gZ8");
    // const auto cefMenu = std::make_shared<NL::Menus::CEFMenu>(logger, cefService, L"https://google.com");
    auto jsFuncStorage = std::make_shared<NL::JS::JSFunctionStorage>();
    jsFuncStorage->AddFunctionCallback(
        IPC_JS_WINDOW_OBJECT_NAME,
        "func",
        {[](const char** arr, int argCount) {
             spdlog::info("CALLING \"func\"");
             for (int i = 0; i < argCount; ++i)
             {
                 spdlog::info("arg[{}]: {}", i, arr[i]);
             }
         },
         true});
    jsFuncStorage->AddFunctionCallback(
        IPC_JS_WINDOW_OBJECT_NAME,
        "func2",
        {[](const char** arr, int argCount) {
             spdlog::info("func2");
         },
         false});

    const auto cefMenu = std::make_shared<NL::Menus::CEFMenu>(logger, cefService, jsFuncStorage);
    cefMenu->GetBrowser()->ToggleBrowserFocusByKeys(RE::BSKeyboardDevice::Keys::kF6, 0);
    cefMenu->GetBrowser()->ToggleBrowserVisibleByKeys(RE::BSKeyboardDevice::Keys::kF7, 0);
    NL::Services::UIPlatformService::GetSingleton().GetMultiLayerMenu()->AddSubMenu("CEF_DEFAULT", cefMenu);
    cefMenu->StartBrowser("file:///123.html");
}

void TestCase()
{
    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* a_msg) {
        switch (a_msg->type)
        {
        case SKSE::MessagingInterface::kPostLoad:
            break;
        case SKSE::MessagingInterface::kPostPostLoad:
            break;
        case SKSE::MessagingInterface::kInputLoaded:
            SKSE::GetTaskInterface()->AddTask([]() {
                BuildTestMenu();
            });
            break;
        case SKSE::MessagingInterface::kDataLoaded:
            break;
        case SKSE::MessagingInterface::kPreLoadGame:
            break;
        case SKSE::MessagingInterface::kPostLoadGame:
            break;
        default:
            break;
        }
    });
}

#pragma endregion

extern "C" DLLEXPORT bool SKSEAPI Entry(const SKSE::LoadInterface* a_skse)
{
    if (a_skse->IsEditor())
    {
        return false;
    }

    try
    {
        // SKSE
        SKSE::Init(a_skse);
        SKSE::AllocTrampoline(1024);
        InitLog();

        // Hooks
        NL::Hooks::WinProcHook::Install();

        // API controller
        NL::Controllers::PublicAPIController::GetSingleton().Init();
    }
    catch (const std::exception& e)
    {
        ShowMessageBox(e.what());
        return false;
    }

    return true;
}
