#pragma once

#include "PCH.h"
#include "Menus/ISubMenu.h"
#include "Render/RenderData.h"

namespace NL::Menus
{
    class MultiLayerMenu final : public RE::IMenu,
                                 public RE::MenuEventHandler,
                                 public RE::BSTEventSink<RE::MenuOpenCloseEvent>,
                                 public RE::BSTEventSink<RE::InputEvent*>
    {
      private:
        std::shared_ptr<spdlog::logger> m_logger = nullptr;

        NL::Render::RenderData m_renderData;
        std::mutex m_mapMenuMutex;
        std::unordered_map<std::string, std::shared_ptr<ISubMenu>> m_menuMap;

      public:
        using RE::IMenu::operator new;
        using RE::IMenu::operator delete;
        MultiLayerMenu(std::shared_ptr<spdlog::logger> a_logger);
        ~MultiLayerMenu() override;

        bool AddSubMenu(std::string_view a_menuName, std::shared_ptr<ISubMenu> a_subMenu);
        bool RemoveSubMenu(std::string_view a_menuName);
        void ClearAllSubMenu();

      public:
        constexpr static std::string_view MENU_NAME = "NirnLabMultiLayerMenu";

        // RE::IMenu
        void PostDisplay() override;
        RE::UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& a_message) override;

        // RE::MenuEventHandler
        bool CanProcess(RE::InputEvent* a_event) override;
        bool ProcessMouseMove(RE::MouseMoveEvent* a_event) override;
        bool ProcessButton(RE::ButtonEvent* a_event) override;

        // RE::BSTEventSink<RE::MenuOpenCloseEvent>
        RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) override;

        // RE::BSTEventSink<RE::InputEvent*>
        RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;
    };
}
