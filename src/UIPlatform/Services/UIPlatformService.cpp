#include "UIPlatformService.h"

namespace NL::Services
{
    UIPlatformService::UIPlatformService(
        std::shared_ptr<spdlog::logger> a_logger,
        std::shared_ptr<CEFService> a_cefService)
    {
        ThrowIfNullptr(UIPlatformService, a_logger);
        m_logger = a_logger;

        ThrowIfNullptr(UIPlatformService, a_cefService);
        m_cefService = a_cefService;
    }

    UIPlatformService::~UIPlatformService()
    {
        Shutdown();
    }

    bool UIPlatformService::Init()
    {
        const auto app = CefRefPtr<NL::CEF::NirnLabCefApp>(new NL::CEF::NirnLabCefApp());
        if (!m_cefService->CEFInitialize(app))
        {
            return false;
        }

        m_defaultCefBrowser = std::make_shared<NL::CEF::CEFBrowser>(m_logger, m_cefService, L"");
        RE::UI::GetSingleton()->Register(NL::Menus::MultiLayerMenu::MENU_NAME, []() {
            const auto mlMenu = new NL::Menus::MultiLayerMenu();
            mlMenu->GetRenderer()->AddLayer(NL::Services::g_uiPlatfromService->GetBrowser()->GetCefClient()->GetCefRenderLayer());
            return static_cast<RE::IMenu*>(mlMenu);
        });

        return true;
    }

    void UIPlatformService::Shutdown()
    {
        m_cefService->CEFShutdown();
    }

    std::shared_ptr<NL::CEF::CEFBrowser> UIPlatformService::GetBrowser()
    {
        return m_defaultCefBrowser;
    }

    std::shared_ptr<NL::CEF::IBrowser> UIPlatformService::GetBrowserInterface()
    {
        return m_defaultCefBrowser;
    }
}
