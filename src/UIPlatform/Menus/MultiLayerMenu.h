#pragma once

#include "PCH.h"
#include "Render/IRenderer.h"
#include "Render/RenderData.h"

namespace NL::Menus
{
    class MultiLayerMenu final : public RE::IMenu,
                                 public RE::BSTEventSink<RE::MenuOpenCloseEvent>,
                                 public RE::BSTEventSink<RE::InputEvent*>
    {
      private:
        NL::Render::RenderData renderData;
        std::shared_ptr<NL::Render::IRenderLayer> renderer = nullptr;

      public:
        MultiLayerMenu(std::shared_ptr<NL::Render::IRenderer> a_renderer);
        ~MultiLayerMenu() override;

      public:
        constexpr static std::string_view MENU_NAME = "NirnLabMultiLayerMenu";

        // RE::IMenu
        void PostDisplay() override;
        RE::UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& a_message) override;

        // RE::BSTEventSink<RE::MenuOpenCloseEvent>
        RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) override;

        // RE::BSTEventSink<RE::InputEvent*>
        RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;
    };
}
