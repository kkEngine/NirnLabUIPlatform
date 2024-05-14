#include "MultiLayerMenu.h"

namespace NL::Menus
{
    MultiLayerMenu::MultiLayerMenu()
    {
        m_renderer = std::make_shared<NL::Render::MultiLayerRenderer>();

        // Fill render data
        const auto device = reinterpret_cast<ID3D11Device*>(RE::BSGraphics::Renderer::GetDevice());
        ThrowIfNullptr(MultiLayerMenu, device);

        ID3D11DeviceContext* immediateContext = nullptr;
        device->GetImmediateContext(&immediateContext);
        ThrowIfNullptr(MultiLayerMenu, immediateContext);

        const auto nativeMenuRenderData = RE::BSGraphics::Renderer::GetRendererData()->renderTargets[RE::RENDER_TARGETS::kMENUBG];
        D3D11_TEXTURE2D_DESC textDesc;
        nativeMenuRenderData.texture->GetDesc(&textDesc);

        m_renderData.device = device;
        m_renderData.spriteBatch = std::make_shared<::DirectX::SpriteBatch>(immediateContext);
        m_renderData.commonStates = std::make_shared<::DirectX::CommonStates>(device);
        m_renderData.texture = nativeMenuRenderData.SRV;
        m_renderData.width = textDesc.Width;
        m_renderData.height = textDesc.Height;

        // Init renderer with this menu data
        m_renderer->Init(&m_renderData);

        // IMenu props
        depthPriority = 8;
        menuFlags.set(RE::UI_MENU_FLAGS::kAlwaysOpen);
        menuFlags.set(RE::UI_MENU_FLAGS::kCustomRendering);
        inputContext = Context::kNone;
    }

    MultiLayerMenu::~MultiLayerMenu()
    {
    }

    std::shared_ptr<NL::Render::MultiLayerRenderer> MultiLayerMenu::GetRenderer()
    {
        return m_renderer;
    }

    void MultiLayerMenu::PostDisplay()
    {
        m_renderer->Draw();
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
