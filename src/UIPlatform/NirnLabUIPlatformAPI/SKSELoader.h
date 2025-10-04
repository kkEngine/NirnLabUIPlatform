#pragma once

#include "API.h"
#include <SKSE/SKSE.h>

namespace NL::UI::SKSELoader
{
    using APIReadyFunc_t = std::function<void(NL::UI::IUIPlatformAPI*)>;

    class LoaderData
    {
    public:
        static inline bool s_canUseAPI = false;
        static inline APIReadyFunc_t s_apiReadyCallback = nullptr;
    };

    inline void GetUIPlatformAPIWithVersionCheck(APIReadyFunc_t a_apiReadyCallback)
    {
        LoaderData::s_apiReadyCallback = a_apiReadyCallback;

        SKSE::GetMessagingInterface()->RegisterListener(NL::UI::LibVersion::PROJECT_NAME, [](SKSE::MessagingInterface::Message* a_msg) {
            switch (a_msg->type)
            {
            case NL::UI::APIMessageType::ResponseVersion: {
                const auto versionInfo = reinterpret_cast<NL::UI::ResponseVersionMessage*>(a_msg->data);
                spdlog::info("NirnLabUIPlatform version: {}.{}", NL::UI::LibVersion::GetMajorVersion(versionInfo->libVersion), NL::UI::LibVersion::GetMinorVersion(versionInfo->libVersion));

                const auto majorAPIVersion = NL::UI::APIVersion::GetMajorVersion(versionInfo->apiVersion);
                // If the major version is different from ours, then using the API may cause problems
                if (majorAPIVersion != NL::UI::APIVersion::MAJOR)
                {
                    LoaderData::s_canUseAPI = false;
                    spdlog::error("Can't using this API version of NirnLabUIPlatform. We have {}.{} and installed is {}.{}",
                                  NL::UI::APIVersion::MAJOR,
                                  NL::UI::APIVersion::MINOR,
                                  NL::UI::APIVersion::GetMajorVersion(versionInfo->apiVersion),
                                  NL::UI::APIVersion::GetMinorVersion(versionInfo->apiVersion));
                }
                else
                {
                    LoaderData::s_canUseAPI = true;
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

                LoaderData::s_apiReadyCallback(api);
                break;
            }
            default:
                break;
            }
        });
    }

    inline void ProcessSKSEMessage(const SKSE::MessagingInterface::Message* a_msg)
    {
        switch (a_msg->type)
        {
        case SKSE::MessagingInterface::kPostPostLoad:
            // All plugins are loaded. Request lib version.
            SKSE::GetMessagingInterface()->Dispatch(NL::UI::APIMessageType::RequestVersion, nullptr, 0, NL::UI::LibVersion::PROJECT_NAME);
            break;
        case SKSE::MessagingInterface::kInputLoaded:
            if (LoaderData::s_canUseAPI)
            {
                NL::UI::Settings defaultSettings;
                // API version is ok. Request interface.
                SKSE::GetMessagingInterface()->Dispatch(NL::UI::APIMessageType::RequestAPI, &defaultSettings, sizeof(defaultSettings), NL::UI::LibVersion::PROJECT_NAME);
            }
            break;
        default:
            break;
        }
    }
}
