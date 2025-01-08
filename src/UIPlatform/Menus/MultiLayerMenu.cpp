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

        HRESULT hResult = 0;
        ID3D11Device3* device3 = nullptr;
        hResult = device->QueryInterface(__uuidof(ID3D11Device3), (void**)&device3);
        if (FAILED(hResult))
        {
            const auto errorMsg = fmt::format("{}: failed to QueryInterface() with {} and result {}", NameOf(MultiLayerMenu), NameOf(ID3D11Device1), hResult);
            throw std::runtime_error(errorMsg);
        }

        ID3D11DeviceContext3* immediateContext = nullptr;
        device3->GetImmediateContext3(&immediateContext);
        ThrowIfNullptr(MultiLayerMenu, immediateContext);

        const auto nativeMenuRenderData = RE::BSGraphics::Renderer::GetRendererData()->renderTargets[RE::RENDER_TARGETS::kMENUBG];
        D3D11_TEXTURE2D_DESC textDesc;
        nativeMenuRenderData.texture->GetDesc(&textDesc);

        m_renderData.device = device;
        m_renderData.deviceContext = immediateContext;
        m_renderData.spriteBatch = std::make_shared<::DirectX::SpriteBatch>(immediateContext);
        m_renderData.commonStates = std::make_shared<::DirectX::CommonStates>(device);
        m_renderData.texture = nativeMenuRenderData.SRV;
        m_renderData.width = textDesc.Width;
        m_renderData.height = textDesc.Height;

        // IMenu props
        depthPriority = 8;
        menuFlags.set(RE::UI_MENU_FLAGS::kAlwaysOpen);
        menuFlags.set(RE::UI_MENU_FLAGS::kCustomRendering);
        menuFlags.set(RE::UI_MENU_FLAGS::kAssignCursorToRenderer);
        inputContext = Context::kNone;

        RE::UI::GetSingleton()->pad17D = false;
        RE::UI::GetSingleton()->AddEventSink(static_cast<RE::BSTEventSink<RE::MenuOpenCloseEvent>*>(this));

        const auto inputEventSource = RE::BSInputDeviceManager::GetSingleton();
        inputEventSource->lock.Lock();
        NL::Utils::PushFront<RE::BSTEventSink<RE::InputEvent*>>(RE::BSInputDeviceManager::GetSingleton()->sinks, this);
        inputEventSource->lock.Unlock();

        NL::Services::InputLangSwitchService::GetSingleton().SetActive(true);
    }

    MultiLayerMenu::~MultiLayerMenu()
    {
        ClearAllSubMenu();
    }

    bool MultiLayerMenu::AddSubMenu(std::string_view a_menuName, std::shared_ptr<ISubMenu> a_subMenu)
    {
        std::lock_guard<std::mutex> lock(m_mapMenuMutex);
        const std::string menuName = a_menuName.data();
        if (m_menuMap.contains(menuName))
        {
            return false;
        }

        m_menuMap.insert({menuName, a_subMenu});
        a_subMenu->Init(&m_renderData);
        return true;
    }

    std::shared_ptr<ISubMenu> MultiLayerMenu::GetSubMenu(const std::string& a_menuName)
    {
        std::lock_guard<std::mutex> lock(m_mapMenuMutex);
        const auto menuIt = m_menuMap.find(a_menuName);
        if (menuIt != m_menuMap.end())
        {
            return menuIt->second;
        }

        return nullptr;
    }

    bool MultiLayerMenu::IsSubMenuExist(const std::string& a_menuName)
    {
        std::lock_guard<std::mutex> lock(m_mapMenuMutex);
        return m_menuMap.find(a_menuName) != m_menuMap.end();
    }

    bool MultiLayerMenu::RemoveSubMenu(const std::string& a_menuName)
    {
        std::lock_guard<std::mutex> lock(m_mapMenuMutex);
        const auto menuIt = m_menuMap.find(a_menuName);
        return m_menuMap.erase(a_menuName) > 0;
    }

    void MultiLayerMenu::ClearAllSubMenu()
    {
        std::lock_guard<std::mutex> lock(m_mapMenuMutex);
        m_menuMap.clear();
    }

#pragma region RE::IMenu

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
            m_renderData.deviceContext->Flush1(D3D11_CONTEXT_TYPE::D3D11_CONTEXT_TYPE_COPY, nullptr);
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
        return RE::UI_MESSAGE_RESULTS::kPassOn;
    }

#pragma endregion

#pragma region RE::MenuEventHandler

    bool MultiLayerMenu::CanProcess(RE::InputEvent* a_event)
    {
        return !m_menuMap.empty();
    }

    bool MultiLayerMenu::ProcessMouseMove(RE::MouseMoveEvent* a_event)
    {
        std::lock_guard<std::mutex> lock(m_mapMenuMutex);
        for (const auto& subMenu : m_menuMap)
        {
            if (subMenu.second->ProcessMouseMove(a_event))
            {
                return true;
            }
        }

        return false;
    }

    bool MultiLayerMenu::ProcessButton(RE::ButtonEvent* a_event)
    {
        std::lock_guard<std::mutex> lock(m_mapMenuMutex);
        for (const auto& subMenu : m_menuMap)
        {
            if (subMenu.second->ProcessButton(a_event))
            {
                return true;
            }
        }

        return false;
    }

#pragma endregion

#pragma region RE::BSTEventSink<RE::MenuOpenCloseEvent>

    RE::BSEventNotifyControl MultiLayerMenu::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource)
    {
        if (a_event->menuName == RE::CursorMenu::MENU_NAME)
        {
            RE::UI::GetSingleton()->pad17D = a_event->opening;
        }
        else if (m_isKeepOpen && !a_event->opening && a_event->menuName == MultiLayerMenu::MENU_NAME)
        {
            auto msgQ = RE::UIMessageQueue::GetSingleton();
            if (msgQ)
            {
                msgQ->AddMessage(NL::Menus::MultiLayerMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, NULL);
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }

#pragma endregion

#pragma region RE::BSTEventSink<RE::InputEvent*>

    RE::BSEventNotifyControl MultiLayerMenu::ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource)
    {
        if (a_event == nullptr || *a_event == nullptr)
        {
            return RE::BSEventNotifyControl::kContinue;
        }

        auto inputEvent = *a_event;
        RE::InputEvent* nextEvent = nullptr;
        auto result = RE::BSEventNotifyControl::kContinue;
        if (!CanProcess(inputEvent)) [[unlikely]]
        {
            return RE::BSEventNotifyControl::kContinue;
        }

        std::lock_guard<std::mutex> lock(m_mapMenuMutex);
        while (inputEvent != nullptr)
        {
            nextEvent = inputEvent->next;
            inputEvent->next = nullptr;

            for (const auto& subMenu : m_menuMap)
            {
                switch (inputEvent->GetEventType())
                {
                case RE::INPUT_EVENT_TYPE::kMouseMove:
                    if (subMenu.second->ProcessMouseMove(inputEvent->AsMouseMoveEvent()))
                    {
                        result = RE::BSEventNotifyControl::kStop;
                        continue;
                    }
                    break;
                case RE::INPUT_EVENT_TYPE::kButton:
                    if (subMenu.second->ProcessButton(inputEvent->AsButtonEvent()))
                    {
                        result = RE::BSEventNotifyControl::kStop;
                        continue;
                    }
                    break;
                default:
                    break;
                }
            }

            inputEvent->next = nextEvent;
            inputEvent = inputEvent->next;
        }

        return result;
    }

#pragma endregion

}
