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

    inline void ProcessSKSEMessage(const SKSE::MessagingInterface::Message* a_msg, NL::UI::Settings* settings = nullptr)
    {
        if (std::strcmp(a_msg->sender, "SKSE") == 0)
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
                    if (settings == nullptr)
                    {
                        settings = &defaultSettings;
                    }

                    // API version is ok. Request interface.
                    SKSE::GetMessagingInterface()->Dispatch(NL::UI::APIMessageType::RequestAPI, settings, sizeof(*settings), NL::UI::LibVersion::PROJECT_NAME);
                }
                break;
            default:
                break;
            }
        }
        else if (std::strcmp(a_msg->sender, NL::UI::LibVersion::PROJECT_NAME) == 0)
        {
            switch (a_msg->type)
            {
            case NL::UI::APIMessageType::ResponseVersion: {
                const auto versionInfo = reinterpret_cast<NL::UI::ResponseVersionMessage*>(a_msg->data);
                spdlog::info("NirnLabUIPlatform loader: installed version: {}.{}", NL::UI::LibVersion::GetMajorVersion(versionInfo->libVersion), NL::UI::LibVersion::GetMinorVersion(versionInfo->libVersion));

                const auto majorAPIVersion = NL::UI::APIVersion::GetMajorVersion(versionInfo->apiVersion);
                const auto minorAPIVersion = NL::UI::APIVersion::GetMinorVersion(versionInfo->apiVersion);
                // Different major version can cause serious compatibility issues. Older minor version may have missing methods
                if (majorAPIVersion != NL::UI::APIVersion::MAJOR || minorAPIVersion != NL::UI::APIVersion::MINOR)
                {
                    LoaderData::s_canUseAPI = false;
                    spdlog::error("NirnLabUIPlatform loader: can't use using this API version. We have {}.{}, but {}.{} is installed",
                                  NL::UI::APIVersion::MAJOR,
                                  NL::UI::APIVersion::MINOR,
                                  NL::UI::APIVersion::GetMajorVersion(versionInfo->apiVersion),
                                  NL::UI::APIVersion::GetMinorVersion(versionInfo->apiVersion));
                }
                else
                {
                    LoaderData::s_canUseAPI = true;
                    spdlog::info("NirnLabUIPlatform loader: API version is ok. Our version {}.{}, installed {}.{}",
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
        }
    }

    inline void GetUIPlatformAPIWithVersionCheck(APIReadyFunc_t a_apiReadyCallback)
    {
        LoaderData::s_apiReadyCallback = a_apiReadyCallback;

        SKSE::GetMessagingInterface()->RegisterListener(NL::UI::LibVersion::PROJECT_NAME, [](SKSE::MessagingInterface::Message* a_msg) {
            ProcessSKSEMessage(a_msg);
        });
    }
}
