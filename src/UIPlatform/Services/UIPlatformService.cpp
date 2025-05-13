#include "UIPlatformService.h"

namespace NL::Services
{
    UIPlatformService::UIPlatformService()
    {
        m_logger = spdlog::default_logger();
    }

    bool UIPlatformService::IsInited()
    {
        return s_isUIPInited;
    }

    bool UIPlatformService::Init(std::shared_ptr<spdlog::logger> a_logger,
                                 std::shared_ptr<NL::Providers::ICEFSettingsProvider> a_settingsProvider)
    {
        std::lock_guard locker(s_uipInitMutex);
        if (s_isUIPInited)
        {
            m_logger->warn("{}: already inited", NameOf(UIPlatformService));
            return false;
        }

        if (a_logger == nullptr)
        {
            spdlog::error("{}: has null {}", NameOf(UIPlatformService), NameOf(a_logger));
            return false;
        }
        m_logger = a_logger;

        try
        {
            m_mlMenu = std::make_shared<NL::Menus::MultiLayerMenu>(m_logger);
        }
        catch (const std::runtime_error& error)
        {
            m_logger->error(error.what());
            return false;
        }

        const auto app = CefRefPtr<NL::CEF::NirnLabCefApp>(new NL::CEF::NirnLabCefApp());
        try
        {
            NL::Services::CEFService::CEFInitialize(app, a_settingsProvider->GetCefSettings());
        }
        catch (const std::exception& error)
        {
            m_logger->error("{}: failed to CEFInitialize, {}", NameOf(UIPlatformService), error.what());
            return false;
        }
        catch (...)
        {
            m_logger->error("{}: failed to CEFInitialize", NameOf(UIPlatformService));
            return false;
        }

        RE::UI::GetSingleton()->Register(NL::Menus::MultiLayerMenu::MENU_NAME, []() {
            const auto mlMenu = UIPlatformService::GetSingleton().GetMultiLayerMenu();
            mlMenu->AddRef();
            return static_cast<RE::IMenu*>(mlMenu.get());
        });

        s_isUIPInited = true;
        return true;
    }

    bool UIPlatformService::InitAndShowMenuWithSettings(std::shared_ptr<NL::Providers::ICEFSettingsProvider> a_settingsProvider)
    {
        const auto logger = spdlog::default_logger();
        if (Init(logger, a_settingsProvider))
        {
            SKSE::GetTaskInterface()->AddTask([]() {
                auto msgQ = RE::UIMessageQueue::GetSingleton();
                if (msgQ)
                {
                    msgQ->AddMessage(NL::Menus::MultiLayerMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, NULL);
                }
            });
            return true;
        }

        return false;
    }

    void UIPlatformService::Shutdown()
    {
        try
        {
            NL::Services::CEFService::CEFShutdown();
        }
        catch (const std::exception& error)
        {
            m_logger->error("{}: error while CEFShutdown, {}", NameOf(UIPlatformService), error.what());
        }
        catch (...)
        {
            m_logger->error("{}: error while CEFShutdown", NameOf(UIPlatformService));
        }
    }

    std::shared_ptr<NL::Menus::MultiLayerMenu> UIPlatformService::GetMultiLayerMenu()
    {
        return m_mlMenu;
    }

    std::shared_ptr<NL::Menus::CEFMenu> UIPlatformService::CreateCefMenu(std::shared_ptr<NL::JS::JSFunctionStorage> a_funcStorage,
                                                                         NL::JS::JSEventFuncInfo& a_eventFuncInfo)
    {
        return std::make_shared<NL::Menus::CEFMenu>(m_logger, a_funcStorage, a_eventFuncInfo);
    }
}
