#include "UIPlatformService.h"

namespace NL::Services
{
    UIPlatformService::UIPlatformService()
    {
    }

    UIPlatformService::~UIPlatformService()
    {
        Shutdown();
    }

    bool UIPlatformService::IsInited()
    {
        return s_isUIPInited;
    }

    bool UIPlatformService::Init(std::shared_ptr<spdlog::logger> a_logger, std::shared_ptr<CEFService> a_cefService)
    {
        std::lock_guard<std::mutex> lock(s_uipInitMutex);
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

        if (a_cefService = nullptr)
        {
            m_logger->error("{}: has null {}", NameOf(UIPlatformService), NameOf(a_cefService));
            return false;
        }
        m_cefService = a_cefService;

        m_mlMenu = std::make_shared<NL::Menus::MultiLayerMenu>(m_logger);

        const auto app = CefRefPtr<NL::CEF::NirnLabCefApp>(new NL::CEF::NirnLabCefApp());
        if (!m_cefService->CEFInitialize(app))
        {
            return false;
        }

        RE::UI::GetSingleton()->Register(NL::Menus::MultiLayerMenu::MENU_NAME, []() {
            const auto mlMenu = UIPlatformService::GetSingleton().GetMultiLayerMenu();
            mlMenu->AddRef();
            return static_cast<RE::IMenu*>(mlMenu.get());
        });

        return true;
    }

    bool UIPlatformService::InitWithDefaultParams()
    {
        const auto logger = spdlog::default_logger();
        const auto cefSettingsProvider = std::make_shared<NL::Providers::DefaultCEFSettingsProvider>();
        const auto cefService = std::make_shared<NL::Services::CEFService>(logger, cefSettingsProvider);

        if (Init(logger, cefService))
        {
            auto msgQ = RE::UIMessageQueue::GetSingleton();
            if (msgQ)
            {
                msgQ->AddMessage(NL::Menus::MultiLayerMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, NULL);
            }
            return true;
        }

        return false;
    }

    void UIPlatformService::Shutdown()
    {
        m_cefService->CEFShutdown();
    }

    std::shared_ptr<NL::Menus::MultiLayerMenu> UIPlatformService::GetMultiLayerMenu()
    {
        return m_mlMenu;
    }

    std::shared_ptr<NL::Menus::CEFMenu> UIPlatformService::CreateCefMenu(std::shared_ptr<NL::JS::JSFunctionStorage> a_funcStorage)
    {
        return std::make_shared<NL::Menus::CEFMenu>(m_logger, m_cefService, a_funcStorage);
    }
}
