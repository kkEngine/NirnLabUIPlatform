#pragma once

#include "PCH.h"
#include "Render/IRenderLayer.h"
#include "Menus/ISubMenu.h"
#include "CEF/DefaultBrowser.h"
#include "CEF/NirnLabCefClient.h"
#include "JS/JSFunctionStorage.h"
#include "JS/JSEventFuncInfo.h"
#include "Services/CEFService.h"

namespace NL::Menus
{
    class CEFMenu : public ISubMenu
    {
    protected:
        std::mutex m_startBrowserMutex;
        bool m_started = false;

        NL::JS::JSEventFuncInfo m_eventFuncInfo{};

        std::shared_ptr<spdlog::logger> m_logger = nullptr;
        std::shared_ptr<NL::JS::JSFunctionStorage> m_jsFuncStorage = nullptr;
        std::shared_ptr<NL::Render::IRenderLayer> m_cefRenderLayer = nullptr;
        std::shared_ptr<NL::CEF::DefaultBrowser> m_browser = nullptr;

    public:
        CEFMenu(
            std::shared_ptr<spdlog::logger> a_logger,
            std::shared_ptr<NL::JS::JSFunctionStorage> a_jsFuncStorage,
            NL::JS::JSEventFuncInfo& a_eventFuncInfo);
        ~CEFMenu() override;

        bool LoadBrowser(std::string_view a_url,
                         const CefWindowInfo& a_cefWindowInfo,
                         const CefBrowserSettings& a_cefBrowserSettings);
        std::shared_ptr<NL::CEF::IBrowser> GetBrowser();

        // NL::Render::IRenderLayer
        void Draw() override;
        void Init(NL::Render::RenderData* a_renderData) override;
        void SetVisible(bool a_visible) override;
        bool GetVisible() override;

        // RE::MenuEventHandler
        bool CanProcess(RE::InputEvent* a_event) override;
        bool ProcessMouseMove(RE::MouseMoveEvent* a_event) override;
        bool ProcessButton(RE::ButtonEvent* a_event) override;

        // NL::Menus::ISubMenu
        SubMenuType GetMenuType() override;
    };
}
