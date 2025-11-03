#include "CEFMenu.h"

namespace NL::Menus
{
    CEFMenu::CEFMenu(std::shared_ptr<spdlog::logger> a_logger,
                     std::shared_ptr<NL::JS::JSFunctionStorage> a_jsFuncStorage,
                     NL::JS::JSEventFuncInfo& a_eventFuncInfo)
    {
        ThrowIfNullptr(CEFMenu, a_logger);
        m_logger = a_logger;

        m_jsFuncStorage = a_jsFuncStorage == nullptr ? std::make_shared<NL::JS::JSFunctionStorage>() : a_jsFuncStorage;
        m_eventFuncInfo = a_eventFuncInfo;

        const auto cefClient = CefRefPtr<NL::CEF::NirnLabCefClient>(new NL::CEF::NirnLabCefClient());
        m_browser = std::make_shared<NL::CEF::DefaultBrowser>(m_logger, cefClient, m_jsFuncStorage);
        m_cefRenderLayer = m_browser->GetCefClient()->GetRenderLayer();
    }

    CEFMenu::~CEFMenu()
    {
        SetVisible(false);
    }

    bool CEFMenu::LoadBrowser(std::string_view a_url,
                              const CefWindowInfo& a_cefWindowInfo,
                              const CefBrowserSettings& a_cefBrowserSettings)
    {
        std::lock_guard locker(m_startBrowserMutex);

        if (!m_started)
        {
            auto eventFuncInfo = CefListValue::Create();
            eventFuncInfo->SetString(0, m_eventFuncInfo.objectName);
            eventFuncInfo->SetString(1, m_eventFuncInfo.funcName);

            auto jsFuncInfo = CefDictionaryValue::Create();
            jsFuncInfo->SetList(IPC_JS_EVENT_FUNCTION_ADD_NAME, eventFuncInfo);

            const auto createBrowserResult =
                NL::Services::CEFService::CreateBrowser(m_browser->GetCefClient(),
                                                        jsFuncInfo,
                                                        CefString(a_url.data()),
                                                        a_cefWindowInfo,
                                                        a_cefBrowserSettings);
            if (!createBrowserResult)
            {
                m_logger->error("{}: failed to create browser", NameOf(CEFMenu));
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
