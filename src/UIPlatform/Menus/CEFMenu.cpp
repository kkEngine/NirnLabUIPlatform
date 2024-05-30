#include "CEFMenu.h"

namespace NL::Menus
{
    CEFMenu::CEFMenu(
        std::shared_ptr<spdlog::logger> a_logger,
        std::shared_ptr<NL::Services::CEFService> a_cefService,
        std::wstring_view a_startUrl)
    {
        ThrowIfNullptr(CEFMenu, a_logger);
        m_logger = a_logger;

        ThrowIfNullptr(CEFMenu, a_cefService);
        m_cefService = a_cefService;

        const auto cefClient = CefRefPtr<NL::CEF::NirnLabCefClient>(new NL::CEF::NirnLabCefClient());
        if (m_cefService->CreateBrowser(cefClient, CefString(a_startUrl.data())))
        {
            m_cefBrowser = std::make_shared<NL::CEF::CEFBrowser>(m_logger, cefClient);
            m_cefRenderLayer = m_cefBrowser->GetCefClient()->GetCefRenderLayer();
        }
        else
        {
            m_logger->error("{}: failed to create browser", NameOf(CEFBrowser));
        }
    }

    std::shared_ptr<NL::CEF::IBrowser> CEFMenu::GetBrowser()
    {
        return m_cefBrowser;
    }

#pragma region NL::Render::IRenderLayer

    void CEFMenu::Draw()
    {
        m_cefRenderLayer->Draw();
    }

    void CEFMenu::Init(NL::Render::RenderData* a_renderData)
    {
        IRenderLayer::Init(a_renderData);
        m_cefRenderLayer->Init(a_renderData);
    }

    void CEFMenu::SetVisible(bool a_visible)
    {
        IRenderLayer::SetVisible(a_visible);
        m_cefRenderLayer->SetVisible(a_visible);
    }

    bool CEFMenu::GetVisible()
    {
        IRenderLayer::GetVisible();
        return m_cefRenderLayer->GetVisible();
    }

#pragma endregion

#pragma region RE::MenuEventHandler

    bool CEFMenu::CanProcess(RE::InputEvent* a_event)
    {
        return false;
    }

#pragma endregion
}
