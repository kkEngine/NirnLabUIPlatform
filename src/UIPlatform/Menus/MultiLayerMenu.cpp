#include "MultiLayerMenu.h"

namespace NL::Menus
{
    MultiLayerMenu::MultiLayerMenu(std::shared_ptr<NL::Render::IRenderer> a_renderer)
    {
        ThrowIfNullptr(MultiLayerMenu, a_renderer);
        renderer = a_renderer;

        // Fill render data
        const auto device = reinterpret_cast<ID3D11Device*>(RE::BSGraphics::Renderer::GetDevice());
        ThrowIfNullptr(MultiLayerMenu, device);

        ID3D11DeviceContext* immediateContext = nullptr;
        device->GetImmediateContext(&immediateContext);
        ThrowIfNullptr(MultiLayerMenu, immediateContext);

        renderData.spriteBatch = std::make_shared<::DirectX::SpriteBatch>(immediateContext);
        renderData.commonStates = std::make_shared<::DirectX::CommonStates>(device);
        renderData.texture = RE::BSGraphics::Renderer::GetRendererData()->renderTargets[RE::RENDER_TARGETS::kMENUBG].SRV;

        // Init renderer with this menu data
        renderer->Init(&renderData);

        // IMenu props
        depthPriority = 8;
        menuFlags.set(RE::UI_MENU_FLAGS::kAlwaysOpen);
        menuFlags.set(RE::UI_MENU_FLAGS::kCustomRendering);
        inputContext = Context::kNone;
    }

    MultiLayerMenu::~MultiLayerMenu()
    {
    }

    void MultiLayerMenu::PostDisplay()
    {
        renderer->Draw();
    }

    RE::UI_MESSAGE_RESULTS MultiLayerMenu::ProcessMessage(RE::UIMessage& a_message)
    {
        return RE::UI_MESSAGE_RESULTS::kPassOn;
    }

    RE::BSEventNotifyControl MultiLayerMenu::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource)
    {
        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl MultiLayerMenu::ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource)
    {
        return RE::BSEventNotifyControl::kContinue;
    }
}
