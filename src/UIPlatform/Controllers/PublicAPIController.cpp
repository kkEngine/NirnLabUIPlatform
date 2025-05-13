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
        m_onShutdownConnection = NL::Hooks::ShutdownHook::OnShutdown.connect([&]() {
            spdlog::default_logger()->flush();
            while (!m_onShutdownFuncs.empty())
            {
                m_onShutdownFuncs.front()();
                m_onShutdownFuncs.pop();
            }
            NL::Services::UIPlatformService::GetSingleton().Shutdown();
        });

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
                if (a_msg->data == nullptr || a_msg->dataLen != sizeof(NL::UI::RequestAPIMessage))
                {
                    spdlog::error("{}: INVALID request api from \"{}\". No data.", NameOf(PublicAPIController), a_msg->sender);
                    return;
                }
                else
                {
                    spdlog::info("{}: Request api from \"{}\"", NameOf(PublicAPIController), a_msg->sender);
                }

                controller.SetSettingsProvider(static_cast<NL::UI::Settings*>(a_msg->data));

                auto& platformService = NL::Services::UIPlatformService::GetSingleton();
                if (!platformService.IsInited() && !platformService.InitAndShowMenuWithSettings(controller.GetSettingsProvider()))
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

    void PublicAPIController::SetSettingsProvider(const NL::UI::Settings* a_settings)
    {
        if (a_settings == nullptr || m_settingsProvider != nullptr)
        {
            return;
        }

        m_settingsProvider = std::make_shared<NL::Providers::CustomCEFSettingsProvider>(*a_settings);
    }

    std::shared_ptr<NL::Providers::ICEFSettingsProvider> PublicAPIController::GetSettingsProvider()
    {
        return m_settingsProvider;
    }

#pragma region NL::UI::IUIPlatformAPI

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

    BrowserRefHandle __cdecl PublicAPIController::AddOrGetBrowser(const char* a_browserName,
                                                                  NL::JS::JSFuncInfo* const* a_funcInfoArr,
                                                                  const std::uint32_t a_funcInfoArrSize,
                                                                  const char* a_startUrl,
                                                                  NL::CEF::IBrowser*& a_outBrowser)
    {
        NL::UI::BrowserSettings defaultSettings;
        return AddOrGetBrowser(a_browserName, a_funcInfoArr, a_funcInfoArrSize, a_startUrl, &defaultSettings, a_outBrowser);
    }

    BrowserRefHandle __cdecl PublicAPIController::AddOrGetBrowser(const char* a_browserName,
                                                                  NL::JS::JSFuncInfo* const* a_funcInfoArr,
                                                                  const std::uint32_t a_funcInfoArrSize,
                                                                  const char* a_startUrl,
                                                                  NL::UI::BrowserSettings* a_settings,
                                                                  NL::CEF::IBrowser*& a_outBrowser)
    {
        const auto mlMenu = GetMultiLayerMenu();
        if (mlMenu == nullptr)
        {
            a_outBrowser = nullptr;
            return NL::UI::IUIPlatformAPI::InvalidBrowserRefHandle;
        }

        if (m_settingsProvider == nullptr)
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
                    if (!a_funcInfoArr[i]->callbackData.isEventFunction)
                    {
                        a_outBrowser->AddFunctionCallback(*a_funcInfoArr[i]);
                    }
                }
            }

            return refHandle;
        }

        const auto cefMenu = mlMenu->GetSubMenu(a_browserName);
        if (cefMenu == nullptr)
        {
            NL::JS::JSEventFuncInfo a_eventFuncInfo{"", ""};
            auto jsFuncStorage = std::make_shared<NL::JS::JSFunctionStorage>();
            if (a_funcInfoArr != nullptr)
            {
                for (std::uint32_t i = 0; i < a_funcInfoArrSize; ++i)
                {
                    if (a_funcInfoArr[i]->callbackData.isEventFunction)
                    {
                        a_eventFuncInfo = NL::JS::JSEventFuncInfo::CreateFromFuncInfo(*a_funcInfoArr[i]);
                    }
                    else
                    {
                        jsFuncStorage->AddFunctionCallback(*a_funcInfoArr[i]);
                    }
                }
            }

            auto newCefMenu = NL::Services::UIPlatformService::GetSingleton().CreateCefMenu(jsFuncStorage, a_eventFuncInfo);
            if (!newCefMenu->LoadBrowser(a_startUrl, m_settingsProvider->GetCefWindowInfo(), m_settingsProvider->MergeAndGetCefBrowserSettings(a_settings)))
            {
                spdlog::error("{}: failed to load browser ({}) with name \"{}\"", NameOf(PublicAPIController), a_startUrl, a_browserName);
                a_outBrowser = nullptr;
                return NL::UI::IUIPlatformAPI::InvalidBrowserRefHandle;
            }
            if (!mlMenu->AddSubMenu(a_browserName, newCefMenu))
            {
                spdlog::error("{}: failed to add cef menu with name \"{}\"", NameOf(PublicAPIController), a_browserName);
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
                    a_outBrowser->AddFunctionCallback(*a_funcInfoArr[i]);
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

    void PublicAPIController::RegisterOnShutdown(OnShutdownFunc_t a_callback)
    {
        m_onShutdownFuncs.push(a_callback);
    }

#pragma endregion
}
