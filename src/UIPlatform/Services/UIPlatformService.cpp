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

        m_mlMenu = std::make_shared<NL::Menus::MultiLayerMenu>(m_logger);
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

        RE::UI::GetSingleton()->Register(NL::Menus::MultiLayerMenu::MENU_NAME, []() {
            const auto mlMenu = g_uiPlatfromService->GetNativeMenu();
            mlMenu->AddRef();
            return static_cast<RE::IMenu*>(mlMenu.get());
        });

        return true;
    }

    void UIPlatformService::Shutdown()
    {
        m_cefService->CEFShutdown();
    }

    std::shared_ptr<NL::Menus::MultiLayerMenu> UIPlatformService::GetNativeMenu()
    {
        return m_mlMenu;
    }
}
