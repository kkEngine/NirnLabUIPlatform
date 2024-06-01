#include "CEFBrowser.h"

namespace NL::CEF
{
    CEFBrowser::CEFBrowser(
        std::shared_ptr<spdlog::logger> a_logger,
        CefRefPtr<NirnLabCefClient> a_cefClient)
    {
        ThrowIfNullptr(CEFBrowser, a_logger);
        m_logger = a_logger;

        ThrowIfNullptr(CEFBrowser, a_cefClient);
        m_cefClient = a_cefClient;

        ZeroMemory(&m_LastCefMouseEvent, sizeof(CefMouseEvent));
    }

    CefRefPtr<NirnLabCefClient> CEFBrowser::GetCefClient()
    {
        return m_cefClient;
    }

    bool CEFBrowser::IsReadyAndLog()
    {
        const auto result = IsBrowserReady();
        if (!result)
        {
            m_logger->info("{}: browser is still loading, try later", NameOf(CEFBrowser));
        }
        return result;
    }

#pragma region IBrowser

    bool __cdecl CEFBrowser::IsBrowserReady()
    {
        return m_cefClient && m_cefClient->IsBrowserReady();
    }

    void __cdecl CEFBrowser::SetBrowserVisible(bool a_value)
    {
        m_cefClient->GetRenderLayer()->SetVisible(a_value);
    }

    bool __cdecl CEFBrowser::IsBrowserVisible()
    {
        return m_cefClient->GetRenderLayer()->GetVisible();
    }

    void __cdecl CEFBrowser::SetBrowserFocused(bool a_value)
    {
        if (!IsReadyAndLog())
        {
            return;
        }

        if (a_value)
        {
            m_cursorMenu = static_cast<RE::CursorMenu*>(RE::UI::GetSingleton()->GetMenu(RE::CursorMenu::MENU_NAME).get());
        }

        m_cefClient->GetBrowser()->GetHost()->SetFocus(a_value);
        m_isFocused = a_value;
    }

    bool __cdecl CEFBrowser::IsBrowserFocused()
    {
        return m_isFocused;
    }

    void __cdecl CEFBrowser::LoadBrowserURL(const char* a_url)
    {
        if (!IsReadyAndLog())
        {
            return;
        }

        const auto frame = m_cefClient->GetBrowser()->GetMainFrame();
        if (frame)
        {
            frame->LoadURL(CefString(a_url));
        }
        else
        {
            m_logger->error("{}: can't get main frame to load url \"{}\"", NameOf(CEFBrowser), a_url);
        }
    }

    void __cdecl CEFBrowser::SendBrowserMsg()
    {
        if (!IsReadyAndLog())
        {
            return;
        }
    }

#pragma endregion

#pragma region RE::MenuEventHandler

    bool CEFBrowser::CanProcess(RE::InputEvent* a_event)
    {
        return IsBrowserFocused();
    }

    bool CEFBrowser::ProcessMouseMove(RE::MouseMoveEvent* a_event)
    {
        if (!IsBrowserFocused() || m_cursorMenu == nullptr)
        {
            return false;
        }

        //a_event->mouseInputX /= RE::MenuCursor::GetSingleton()->defaultMouseSpeed;
        //a_event->mouseInputY /= RE::MenuCursor::GetSingleton()->defaultMouseSpeed;
        //m_cursorMenu->ProcessMouseMove(a_event);

        m_LastCefMouseEvent.x = static_cast<int>(m_CurrentMousePosX);
        m_LastCefMouseEvent.y = static_cast<int>(m_CurrentMousePosY);
        m_cefClient->GetBrowser()->GetHost()->SendMouseMoveEvent(m_LastCefMouseEvent, false);

        return true;
    }

    bool CEFBrowser::ProcessButton(RE::ButtonEvent* a_event)
    {
        if (a_event->GetDevice() == RE::INPUT_DEVICE::kKeyboard && a_event->GetIDCode() == 0x40 && a_event->IsUp())
        {
            SetBrowserFocused(!IsBrowserFocused());

            RE::ConsoleLog::GetSingleton()->Print(
                fmt::format("{}__{}", RE::MenuCursor::GetSingleton()->defaultMouseSpeed, RE::MenuCursor::GetSingleton()->cursorSensitivity).c_str()
            );
        }
        if (a_event->GetDevice() == RE::INPUT_DEVICE::kKeyboard && a_event->GetIDCode() == 0x41 && a_event->IsUp())
        {
            SetBrowserVisible(!IsBrowserVisible());
        }


        if (!IsBrowserFocused())
        {
            return false;
        }

        auto browserHost = m_cefClient->GetBrowser()->GetHost();
        switch (a_event->GetDevice())
        {
        case RE::INPUT_DEVICE::kMouse:
            if (!a_event->IsDown() && !a_event->IsUp())
                return true;

            switch (a_event->GetIDCode())
            {
            case RE::BSWin32MouseDevice::Keys::kWheelUp:
                browserHost->SendMouseWheelEvent(m_LastCefMouseEvent, 0, MOUSE_WHEEL_DELTA);
                break;
            case RE::BSWin32MouseDevice::Keys::kWheelDown:
                browserHost->SendMouseWheelEvent(m_LastCefMouseEvent, 0, -MOUSE_WHEEL_DELTA);
                break;
            case RE::BSWin32MouseDevice::Key::kLeftButton:
                UPDATE_MODIFIER_FLAG(a_event, m_CefKeyModifiers, EVENTFLAG_LEFT_MOUSE_BUTTON);
                m_LastCefMouseEvent.modifiers = m_CefKeyModifiers;
                browserHost->SendMouseClickEvent(m_LastCefMouseEvent, CefBrowserHost::MouseButtonType::MBT_LEFT, a_event->IsUp(), 1);
                break;
            case RE::BSWin32MouseDevice::Key::kRightButton:
                UPDATE_MODIFIER_FLAG(a_event, m_CefKeyModifiers, EVENTFLAG_RIGHT_MOUSE_BUTTON);
                m_LastCefMouseEvent.modifiers = m_CefKeyModifiers;
                browserHost->SendMouseClickEvent(m_LastCefMouseEvent, CefBrowserHost::MouseButtonType::MBT_RIGHT, a_event->IsUp(), 1);
                break;
            case RE::BSWin32MouseDevice::Key::kMiddleButton:
                UPDATE_MODIFIER_FLAG(a_event, m_CefKeyModifiers, EVENTFLAG_MIDDLE_MOUSE_BUTTON);
                m_LastCefMouseEvent.modifiers = m_CefKeyModifiers;
                browserHost->SendMouseClickEvent(m_LastCefMouseEvent, CefBrowserHost::MouseButtonType::MBT_MIDDLE, a_event->IsUp(), 1);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }

        /////////////////////////////////////////////
        if (a_event->IsUp())
        {

            std::uint32_t keyCode = 0;
            if (RE::BSInputDeviceManager::GetSingleton()->GetDeviceMappedKeycode(a_event->GetDevice(), a_event->GetIDCode(), keyCode))
            {
                RE::ConsoleLog::GetSingleton()->Print(fmt::format("GetDeviceMappedKeycode: {}", keyCode).c_str());
            }
            else
            {
                m_logger->info("NOT GetDeviceMappedKeycode");
            }
        }

        return true;
    }

#pragma endregion

}
