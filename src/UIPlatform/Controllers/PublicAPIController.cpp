#include "PublicAPIController.h"

namespace NL::Controllers
{
    std::shared_ptr<NL::Menus::MultiLayerMenu> PublicAPIController::GetMultiLayerMenu()
    {
        auto& platformService = NL::Services::UIPlatformService::GetSingleton();
        if (!platformService.IsInited())
        {
            spdlog::error("{}: platform service must be inited", NameOf(PublicAPIController));
            return nullptr;
        }

        const auto mlMenu = platformService.GetMultiLayerMenu();
        if (mlMenu == nullptr)
        {
            spdlog::error("{}: native menu is nullptr", NameOf(PublicAPIController));
            return nullptr;
        }

        return mlMenu;
    }

    NL::UI::ResponseVersionMessage* PublicAPIController::GetVersionMessage()
    {
        return &m_rvMessage;
    }

    NL::UI::ResponseAPIMessage* PublicAPIController::GetAPIMessage()
    {
        return &m_rAPIMessage;
    }

    void PublicAPIController::Init()
    {
        SKSE::GetMessagingInterface()->RegisterListener(nullptr, [](SKSE::MessagingInterface::Message* a_msg) {
            if (std::strcmp(a_msg->sender, "SKSE") == 0)
            {
                return;
            }

            auto& controller = PublicAPIController::GetSingleton();
            switch (a_msg->type)
            {
            case NL::UI::APIMessageType::RequestVersion:
                spdlog::info("{}: Request version data from \"{}\"", NameOf(PublicAPIController), a_msg->sender);
                SKSE::GetMessagingInterface()->Dispatch(NL::UI::APIMessageType::ResponseVersion,
                                                        static_cast<void*>(controller.GetVersionMessage()),
                                                        sizeof(*controller.GetVersionMessage()),
                                                        a_msg->sender);
                break;
            case NL::UI::APIMessageType::RequestAPI: {
                spdlog::info("{}: Request api from \"{}\"", NameOf(PublicAPIController), a_msg->sender);
                auto& platformService = NL::Services::UIPlatformService::GetSingleton();
                if (!platformService.IsInited() && !platformService.InitWithDefaultParams())
                {
                    spdlog::error("{}: Can't response API because ui platform failed to init", NameOf(PublicAPIController));
                    break;
                }
                SKSE::GetMessagingInterface()->Dispatch(NL::UI::APIMessageType::ResponseAPI,
                                                        static_cast<void*>(controller.GetAPIMessage()),
                                                        sizeof(*controller.GetAPIMessage()),
                                                        a_msg->sender);
                break;
            }
            default:
                break;
            }
        });
    }

#pragma region NL::UI::IUIPlatformAPI

    BrowserRefHandle __cdecl PublicAPIController::AddOrGetBrowser(const char* a_browserName,
                                                                  const NL::JS::JSFuncInfo** a_funcInfoArr,
                                                                  const std::uint32_t a_funcInfoArrSize,
                                                                  const char* a_startUrl,
                                                                  NL::CEF::IBrowser*& a_outBrowser)
    {
        const auto mlMenu = GetMultiLayerMenu();
        if (mlMenu == nullptr)
        {
            a_outBrowser = nullptr;
            return NL::UI::IUIPlatformAPI::InvalidBrowserRefHandle;
        }

        std::lock_guard lock(m_mapMutex);
        const auto browserIt = m_browserNameMap.find(a_browserName);
        if (browserIt != m_browserNameMap.cend())
        {
            const auto refHandle = m_currentRefHandle++;
            browserIt->second.refHandles.insert(refHandle);
            m_browserHandleMap.insert({refHandle, a_browserName});

            a_outBrowser = browserIt->second.browser;
            if (a_funcInfoArr != nullptr)
            {
                for (std::uint32_t i = 0; i < a_funcInfoArrSize; ++i)
                {
                    auto funcInfo = a_funcInfoArr[i];
                    a_outBrowser->AddFunctionCallback(funcInfo->objectName, funcInfo->objectName, funcInfo->callbackData);
                }
            }

            return refHandle;
        }

        const auto cefMenu = mlMenu->GetSubMenu(a_browserName);
        if (cefMenu == nullptr)
        {
            auto jsFuncStorage = std::make_shared<NL::JS::JSFunctionStorage>();
            if (a_funcInfoArr != nullptr)
            {
                for (std::uint32_t i = 0; i < a_funcInfoArrSize; ++i)
                {
                    auto funcInfo = a_funcInfoArr[i];
                    jsFuncStorage->AddFunctionCallback(funcInfo->objectName, funcInfo->objectName, funcInfo->callbackData);
                }
            }

            auto newCefMenu = NL::Services::UIPlatformService::GetSingleton().CreateCefMenu(jsFuncStorage);
            newCefMenu->StartBrowser(a_startUrl);
            if (!mlMenu->AddSubMenu(a_browserName, newCefMenu))
            {
                spdlog::error("{}: failed to add cef menu with name \"\"", NameOf(PublicAPIController), a_browserName);
                a_outBrowser = nullptr;
                return NL::UI::IUIPlatformAPI::InvalidBrowserRefHandle;
            }

            a_outBrowser = newCefMenu->GetBrowser().get();
        }
        else
        {
            if (cefMenu->GetMenuType() != NL::Menus::SubMenuType::CEFMenu)
            {
                spdlog::error("{}: trying to get not a browser menu", NameOf(PublicAPIController));
                a_outBrowser = nullptr;
                return NL::UI::IUIPlatformAPI::InvalidBrowserRefHandle;
            }

            a_outBrowser = std::static_pointer_cast<NL::Menus::CEFMenu>(cefMenu)->GetBrowser().get();
            if (a_funcInfoArr != nullptr)
            {
                for (std::uint32_t i = 0; i < a_funcInfoArrSize; ++i)
                {
                    auto funcInfo = a_funcInfoArr[i];
                    a_outBrowser->AddFunctionCallback(funcInfo->objectName, funcInfo->objectName, funcInfo->callbackData);
                }
            }
        }

        const auto refHandle = m_currentRefHandle++;

        auto& handleData = m_browserNameMap.insert({a_browserName, {}}).first->second;
        handleData.browser = a_outBrowser;
        handleData.refHandles.insert(refHandle);

        m_browserHandleMap.insert({refHandle, a_browserName});

        return refHandle;
    }

    void __cdecl PublicAPIController::ReleaseBrowserHandle(BrowserRefHandle a_handle)
    {
        std::lock_guard lock(m_mapMutex);
        const auto handleIt = m_browserHandleMap.find(a_handle);
        if (handleIt == m_browserHandleMap.cend())
        {
            spdlog::warn("{}: not found browser ref handle", NameOf(PublicAPIController));
            return;
        }

        const auto browserName = handleIt->second;
        m_browserHandleMap.erase(handleIt);
        const auto browserIt = m_browserNameMap.find(browserName);
        if (browserIt == m_browserNameMap.cend())
        {
            spdlog::warn("{}: not found browser pointer", NameOf(PublicAPIController));
            return;
        }

        browserIt->second.refHandles.erase(a_handle);
        if (browserIt->second.refHandles.empty())
        {
            m_browserNameMap.erase(browserIt);
            const auto mlMenu = GetMultiLayerMenu();
            if (mlMenu != nullptr)
            {
                mlMenu->RemoveSubMenu(browserName);
            }
        }
    }

#pragma endregion
}
