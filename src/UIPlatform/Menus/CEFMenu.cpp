#include "CEFMenu.h"

namespace NL::Menus
{
    CEFMenu::CEFMenu(
        std::shared_ptr<spdlog::logger> a_logger,
        std::shared_ptr<NL::Services::CEFService> a_cefService,
        std::shared_ptr<NL::JS::JSFunctionStorage> a_jsFuncStorage)
    {
        ThrowIfNullptr(CEFMenu, a_logger);
        m_logger = a_logger;

        ThrowIfNullptr(CEFMenu, a_cefService);
        m_cefService = a_cefService;

        m_jsFuncStorage = a_jsFuncStorage == nullptr ? std::make_shared<NL::JS::JSFunctionStorage>() : a_jsFuncStorage;

        const auto cefClient = CefRefPtr<NL::CEF::NirnLabCefClient>(new NL::CEF::NirnLabCefClient());
        m_browser = std::make_shared<NL::CEF::DefaultBrowser>(m_logger, cefClient, m_jsFuncStorage);
        m_cefRenderLayer = m_browser->GetCefClient()->GetRenderLayer();
    }

    bool CEFMenu::LoadBrowser(std::string_view a_url)
    {
        std::lock_guard<std::mutex> lock(m_startBrowserMutex);
        if (!m_started)
        {
            if (!m_cefService->CreateBrowser(m_browser->GetCefClient(), m_jsFuncStorage->ConvertToCefDictionary(), CefString(a_url.data())))
            {
                m_logger->error("{}: failed to create browser", NameOf(DefaultBrowser));
                return false;
            }
            m_started = true;
        }
        else
        {
            m_browser->LoadBrowserURL(a_url.data());
        }

        return true;
    }

    std::shared_ptr<NL::CEF::IBrowser> CEFMenu::GetBrowser()
    {
        return m_browser;
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
        return m_browser->CanProcess(a_event);
    }

    bool CEFMenu::ProcessMouseMove(RE::MouseMoveEvent* a_event)
    {
        return m_browser->ProcessMouseMove(a_event);
    }

    bool CEFMenu::ProcessButton(RE::ButtonEvent* a_event)
    {
        return m_browser->ProcessButton(a_event);
    }

#pragma endregion

    SubMenuType CEFMenu::GetMenuType()
    {
        return SubMenuType::CEFMenu;
    }
}
