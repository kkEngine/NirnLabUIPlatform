#include "MultiLayerMenu.h"

namespace NL::Menus
{
    MultiLayerMenu::MultiLayerMenu(std::shared_ptr<spdlog::logger> a_logger)
    {
        ThrowIfNullptr(MultiLayerMenu, a_logger);
        m_logger = a_logger;

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

        // IMenu props
        depthPriority = 8;
        menuFlags.set(RE::UI_MENU_FLAGS::kAlwaysOpen);
        menuFlags.set(RE::UI_MENU_FLAGS::kCustomRendering);
        inputContext = Context::kNone;

        RE::UI::GetSingleton()->AddEventSink(static_cast<RE::BSTEventSink<RE::MenuOpenCloseEvent>*>(this));
        //RE::MenuControls::GetSingleton()->AddHandler(this);
    }

    MultiLayerMenu::~MultiLayerMenu()
    {
        ClearAllSubMenu();
    }

    bool MultiLayerMenu::AddSubMenu(std::string_view a_menuName, std::shared_ptr<ISubMenu> a_subMenu)
    {
        std::lock_guard<std::mutex> lock(m_mapMenuMutex);
        if (m_menuMap.contains(a_menuName.data()))
        {
            return false;
        }

        m_menuMap.insert({a_menuName.data(), a_subMenu});
        a_subMenu->Init(&m_renderData);
        return true;
    }

    bool MultiLayerMenu::RemoveSubMenu(std::string_view a_menuName)
    {
        std::lock_guard<std::mutex> lock(m_mapMenuMutex);
        const auto menuIt = m_menuMap.find(a_menuName.data());
        return m_menuMap.erase(a_menuName.data()) > 0;
    }

    void MultiLayerMenu::ClearAllSubMenu()
    {
        std::lock_guard<std::mutex> lock(m_mapMenuMutex);
        m_menuMap.clear();
    }

    void MultiLayerMenu::PostDisplay()
    {
        std::lock_guard<std::mutex> lock(m_mapMenuMutex);
        if (m_menuMap.empty())
        {
            return;
        }

        m_renderData.spriteBatch->Begin(::DirectX::SpriteSortMode_Deferred, m_renderData.commonStates->NonPremultiplied());
        m_renderData.drawLock.Lock();
        try
        {
            for (const auto& subMenu : m_menuMap)
            {
                subMenu.second->Draw();
            }
        }
        catch (const std::exception& err)
        {
            m_logger->error("{}: {}", NameOf(MultiLayerMenu), err.what());
        }
        m_renderData.spriteBatch->End();
        m_renderData.drawLock.Unlock();
    }

    RE::UI_MESSAGE_RESULTS MultiLayerMenu::ProcessMessage(RE::UIMessage& a_message)
    {
        RE::ConsoleLog::GetSingleton()->Print(
            fmt::format("UI msg \"{}\": {}", a_message.menu.c_str(), a_message.type.underlying()).c_str());

        if (a_message.type.get() == RE::UI_MESSAGE_TYPE::kUserEvent)
        {
            const auto msg = static_cast<RE::BSUIMessageData*>(a_message.data);
            RE::ConsoleLog::GetSingleton()->Print(msg->fixedStr.c_str());
        }

        return RE::UI_MESSAGE_RESULTS::kPassOn;
    }

    bool MultiLayerMenu::CanProcess(RE::InputEvent* a_event)
    {
        return true;
    }

    bool MultiLayerMenu::ProcessMouseMove(RE::MouseMoveEvent* a_event)
    {
        RE::ConsoleLog::GetSingleton()->Print("mmove");
        return false;
    }

    bool MultiLayerMenu::ProcessButton(RE::ButtonEvent* a_event)
    {
        RE::ConsoleLog::GetSingleton()->Print("button");
        return false;
    }

    RE::BSEventNotifyControl MultiLayerMenu::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource)
    {
        if (a_event->menuName == MultiLayerMenu::MENU_NAME && !a_event->opening)
        {
            auto msgQ = RE::UIMessageQueue::GetSingleton();
            if (msgQ)
            {
                msgQ->AddMessage(NL::Menus::MultiLayerMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, NULL);
            }

            return RE::BSEventNotifyControl::kContinue;
        }

        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl MultiLayerMenu::ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource)
    {
        return RE::BSEventNotifyControl::kContinue;
    }
}