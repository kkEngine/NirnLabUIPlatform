#pragma once

#include "PCH.h"
#include "Render/IRenderLayer.h"
#include "Menus/ISubMenu.h"
#include "CEF/IBrowser.h"
#include "CEF/CEFBrowser.h"
#include "CEF/NirnLabCefClient.h"
#include "Services/CEFService.h"

namespace NL::Menus
{
    class CEFMenu : public ISubMenu
    {
      protected:
        std::shared_ptr<spdlog::logger> m_logger = nullptr;
        std::shared_ptr<NL::Services::CEFService> m_cefService = nullptr;
        std::shared_ptr<NL::Render::IRenderLayer> m_cefRenderLayer = nullptr;
        std::shared_ptr<NL::CEF::CEFBrowser> m_cefBrowser = nullptr;

      public:
        CEFMenu(
            std::shared_ptr<spdlog::logger> a_logger,
            std::shared_ptr<NL::Services::CEFService> a_cefService,
            std::wstring_view a_startUrl);
        ~CEFMenu() override = default;

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
    };
}
