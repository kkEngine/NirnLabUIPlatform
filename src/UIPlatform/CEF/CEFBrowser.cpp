#include "CEFBrowser.h"

namespace NL::CEF
{
    CEFBrowser::CEFBrowser(
        std::shared_ptr<spdlog::logger> a_logger,
        CefRefPtr<NirnLabCefClient> a_cefClient,
        std::shared_ptr<NL::JS::JSFunctionStorage> a_jsFuncStorage)
    {
        ThrowIfNullptr(CEFBrowser, a_logger);
        m_logger = a_logger;

        ThrowIfNullptr(CEFBrowser, a_cefClient);
        m_cefClient = a_cefClient;

        ThrowIfNullptr(CEFBrowser, a_jsFuncStorage);
        m_jsFuncStorage = a_jsFuncStorage;

        ZeroMemory(&m_lastCefMouseEvent, sizeof(CefMouseEvent));
        ZeroMemory(&m_lastCharCefKeyEvent, sizeof(CefKeyEvent));

        onWndInactiveConnection = NL::Hooks::WinProcHook::OnWndInactive.connect([&]() {
            ClearCefKeyModifiers();
        });

        m_cefClient->onIPCMessageReceived.connect([&](CefRefPtr<CefProcessMessage> a_message) {
            const auto ipcArgs = a_message->GetArgumentList();
            const auto objName = ipcArgs->GetString(0).ToString();
            const auto funcName = ipcArgs->GetString(1).ToString();
            const auto argList = ipcArgs->GetList(2);

            const auto params = NL::Converters::CefValueToJSONConverter::ConvertToJSONStringArgs(argList);
            m_jsFuncStorage->ExecuteFunctionCallback(objName, funcName, params);
        });
    }

    void CEFBrowser::UpdateCefKeyModifiers(const RE::ButtonEvent* a_event, const cef_event_flags_t a_flags)
    {
        if (a_event->IsDown())
        {
            m_cefKeyModifiers |= a_flags;
        }
        else if (a_event->IsUp())
        {
            m_cefKeyModifiers &= ~a_flags;
        }
        m_lastCefMouseEvent.modifiers = m_cefKeyModifiers;
        m_lastCharCefKeyEvent.modifiers = m_cefKeyModifiers;
    }

    void CEFBrowser::ClearCefKeyModifiers()
    {
        m_cefKeyModifiers = 0;
        m_lastCefMouseEvent.modifiers = 0;
        m_lastCharCefKeyEvent.modifiers = 0;
    }

    void CEFBrowser::UpdateCefKeyModifiersFromVK(const RE::ButtonEvent* a_event, const std::uint32_t a_vkCode)
    {
        if (a_vkCode >= VK_NUMPAD0 && a_vkCode <= VK_DIVIDE)
        {
            UpdateCefKeyModifiers(a_event, EVENTFLAG_IS_KEY_PAD);
        }
        else
        {
            switch (a_vkCode)
            {
            case VK_CAPITAL:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_CAPS_LOCK_ON);
                break;
            case VK_SHIFT:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_SHIFT_DOWN);
                break;
            case VK_CONTROL:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_CONTROL_DOWN);
                break;
            case VK_MENU:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_ALT_DOWN);
                break;
            case VK_NUMLOCK:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_NUM_LOCK_ON);
                break;
            case VK_LCONTROL:
            case VK_LMENU:
            case VK_LSHIFT:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_IS_LEFT);
                break;
            case VK_RCONTROL:
            case VK_RMENU:
            case VK_RSHIFT:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_IS_RIGHT);
                break;
            default:
                break;
            }
        }
    }

    void CEFBrowser::CheckToggleFocusKeys(const RE::ButtonEvent* a_event)
    {
        if (!a_event->IsDown() || m_toggleFocusKeyCode1 == 0 && m_toggleFocusKeyCode2 == 0)
        {
            return;
        }

        const auto keyboard = RE::BSInputDeviceManager::GetSingleton()->GetKeyboard();
        const std::uint8_t* keyboardState = keyboard == nullptr ? nullptr : keyboard->curState;
        if (
            keyboardState != nullptr &&
            (m_toggleFocusKeyCode1 == 0 || (keyboardState[m_toggleFocusKeyCode1] & 0x80) != 0) &&
            (m_toggleFocusKeyCode2 == 0 || (keyboardState[m_toggleFocusKeyCode2] & 0x80) != 0))
        {
            SetBrowserFocused(!IsBrowserFocused());
        }
    }

    void CEFBrowser::CheckToggleVisibleKeys(const RE::ButtonEvent* a_event)
    {
        if (!a_event->IsDown() || m_toggleVisibleKeyCode1 == 0 && m_toggleVisibleKeyCode2 == 0)
        {
            return;
        }

        const auto keyboard = RE::BSInputDeviceManager::GetSingleton()->GetKeyboard();
        const std::uint8_t* keyboardState = keyboard == nullptr ? nullptr : keyboard->curState;
        if (
            keyboardState != nullptr &&
            (m_toggleVisibleKeyCode1 == 0 || (keyboardState[m_toggleVisibleKeyCode1] & 0x80) != 0) &&
            (m_toggleVisibleKeyCode2 == 0 || (keyboardState[m_toggleVisibleKeyCode2] & 0x80) != 0))
        {
            SetBrowserVisible(!IsBrowserVisible());
        }
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
        if (!a_value)
        {
            SetBrowserFocused(false);
        }
    }

    bool __cdecl CEFBrowser::IsBrowserVisible()
    {
        return m_cefClient->GetRenderLayer()->GetVisible();
    }

    void __cdecl CEFBrowser::ToggleBrowserVisibleByKeys(const std::uint32_t a_keyCode1, const std::uint32_t a_keyCode2)
    {
        m_toggleVisibleKeyCode1 = a_keyCode1 < sizeof(RE::BSInputDeviceManager::GetSingleton()->GetKeyboard()->curState) ? a_keyCode1 : 0;
        m_toggleVisibleKeyCode2 = a_keyCode2 < sizeof(RE::BSInputDeviceManager::GetSingleton()->GetKeyboard()->curState) ? a_keyCode2 : 0;
    }

    void __cdecl CEFBrowser::SetBrowserFocused(bool a_value)
    {
        if (!IsReadyAndLog() || a_value == m_isFocused)
        {
            return;
        }

        const auto uiMsgQ = RE::UIMessageQueue::GetSingleton();
        if (a_value)
        {
            m_wasCursorOpen = RE::UI::GetSingleton()->pad17D;
            if (uiMsgQ != nullptr)
            {
                uiMsgQ->AddMessage(RE::CursorMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, NULL);
            }
        }
        else
        {
            if (uiMsgQ != nullptr && !m_wasCursorOpen)
            {
                uiMsgQ->AddMessage(RE::CursorMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, NULL);
            }
        }

        m_cefClient->GetBrowser()->GetHost()->SetFocus(a_value);
        m_isFocused = a_value;
    }

    bool __cdecl CEFBrowser::IsBrowserFocused()
    {
        return m_isFocused;
    }

    void __cdecl CEFBrowser::ToggleBrowserFocusByKeys(const std::uint32_t a_keyCode1, const std::uint32_t a_keyCode2)
    {
        m_toggleFocusKeyCode1 = a_keyCode1 < sizeof(RE::BSInputDeviceManager::GetSingleton()->GetKeyboard()->curState) ? a_keyCode1 : 0;
        m_toggleFocusKeyCode2 = a_keyCode2 < sizeof(RE::BSInputDeviceManager::GetSingleton()->GetKeyboard()->curState) ? a_keyCode2 : 0;
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

    void __cdecl CEFBrowser::ExecuteJavaScript(const char* a_script, const char* a_scriptUrl)
    {
        if (!IsReadyAndLog())
        {
            return;
        }

        m_cefClient->GetBrowser()->GetMainFrame()->ExecuteJavaScript(a_script, a_scriptUrl, 0);
    }

    void __cdecl CEFBrowser::AddFunctionCallback(const char* a_objectName, const char* a_funcName, NL::JS::JSFuncCallbackData a_callbackData)
    {
        m_jsFuncStorage->AddFunctionCallback(a_objectName, a_funcName, a_callbackData);
        // send IPC message to RenderProcess if started
    }

#pragma endregion

#pragma region RE::MenuEventHandler

    bool CEFBrowser::CanProcess(RE::InputEvent* a_event)
    {
        return IsBrowserFocused();
    }

    bool CEFBrowser::ProcessMouseMove(RE::MouseMoveEvent* a_event)
    {
        if (!IsBrowserFocused())
        {
            return false;
        }

        RE::MenuControls::GetSingleton()->ProcessEvent(reinterpret_cast<RE::InputEvent**>(&a_event), nullptr);

        m_lastCefMouseEvent.x = static_cast<int>(m_currentMousePosX);
        m_lastCefMouseEvent.y = static_cast<int>(m_currentMousePosY);
        m_cefClient->GetBrowser()->GetHost()->SendMouseMoveEvent(m_lastCefMouseEvent, false);

        return true;
    }

    bool CEFBrowser::ProcessButton(RE::ButtonEvent* a_event)
    {
        CheckToggleFocusKeys(a_event);
        CheckToggleVisibleKeys(a_event);

        if (!IsBrowserFocused())
        {
            return false;
        }

        const auto scanCode = a_event->GetIDCode();
        const auto browserHost = m_cefClient->GetBrowser()->GetHost();
        switch (a_event->GetDevice())
        {
        case RE::INPUT_DEVICE::kMouse:
            if (!a_event->IsDown() && !a_event->IsUp())
            {
                return true;
            }

            switch (scanCode)
            {
            case RE::BSWin32MouseDevice::Keys::kWheelUp:
                browserHost->SendMouseWheelEvent(m_lastCefMouseEvent, 0, MOUSE_WHEEL_DELTA);
                break;
            case RE::BSWin32MouseDevice::Keys::kWheelDown:
                browserHost->SendMouseWheelEvent(m_lastCefMouseEvent, 0, -MOUSE_WHEEL_DELTA);
                break;
            case RE::BSWin32MouseDevice::Key::kLeftButton:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_LEFT_MOUSE_BUTTON);
                browserHost->SendMouseClickEvent(m_lastCefMouseEvent, CefBrowserHost::MouseButtonType::MBT_LEFT, a_event->IsUp(), 1);
                break;
            case RE::BSWin32MouseDevice::Key::kRightButton:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_RIGHT_MOUSE_BUTTON);
                browserHost->SendMouseClickEvent(m_lastCefMouseEvent, CefBrowserHost::MouseButtonType::MBT_RIGHT, a_event->IsUp(), 1);
                break;
            case RE::BSWin32MouseDevice::Key::kMiddleButton:
                UpdateCefKeyModifiers(a_event, EVENTFLAG_MIDDLE_MOUSE_BUTTON);
                browserHost->SendMouseClickEvent(m_lastCefMouseEvent, CefBrowserHost::MouseButtonType::MBT_MIDDLE, a_event->IsUp(), 1);
                break;
            default:
                break;
            }
            break;
        case RE::INPUT_DEVICE::kVirtualKeyboard:
        case RE::INPUT_DEVICE::kKeyboard: {
            const auto isKeyStateChanged = a_event->IsDown() || a_event->IsUp();
            if (isKeyStateChanged)
            {
                const auto vkCode = NL::Utils::InputConverter::GetVirtualKey(scanCode);
                UpdateCefKeyModifiersFromVK(a_event, vkCode);
                m_lastCharCefKeyEvent.native_key_code = scanCode;

                if (a_event->IsDown())
                {
                    m_lastScanCode = scanCode;
                    m_lastCharCefKeyEvent.type = cef_key_event_type_t::KEYEVENT_KEYDOWN;
                    m_lastCharCefKeyEvent.windows_key_code = vkCode;
                    m_keyHeldDuration = KEY_FIRST_CHAR_DELAY;
                    browserHost->SendKeyEvent(m_lastCharCefKeyEvent);

                    if (NL::Utils::InputConverter::ShouldConvertToChar(scanCode, vkCode))
                    {
                        m_lastCharCefKeyEvent.type = cef_key_event_type_t::KEYEVENT_CHAR;
                        m_lastCharCefKeyEvent.windows_key_code = NL::Utils::InputConverter::VkCodeToChar(scanCode, vkCode, m_cefKeyModifiers & (EVENTFLAG_SHIFT_DOWN | EVENTFLAG_CAPS_LOCK_ON));
                        browserHost->SendKeyEvent(m_lastCharCefKeyEvent);
                    }

                    //  Shortcuts
                    if (m_cefKeyModifiers & EVENTFLAG_CONTROL_DOWN)
                    {
                        switch (vkCode)
                        {
                        case 0x43:
                            // CTRL-C
                            m_cefClient->GetBrowser()->GetFocusedFrame()->Copy();
                            break;
                        case 0x58:
                            // CTRL-X
                            m_cefClient->GetBrowser()->GetFocusedFrame()->Cut();
                            break;
                        case 0x56:
                            // CTRL-V
                            m_cefClient->GetBrowser()->GetFocusedFrame()->Paste();
                            break;
                        case 0x41:
                            // CTRL-A
                            m_cefClient->GetBrowser()->GetFocusedFrame()->SelectAll();
                            break;
                        default:
                            break;
                        }
                    }
                }
                else if (a_event->IsUp())
                {
                    m_lastCharCefKeyEvent.type = cef_key_event_type_t::KEYEVENT_KEYUP;
                    m_lastCharCefKeyEvent.windows_key_code = vkCode;
                    browserHost->SendKeyEvent(m_lastCharCefKeyEvent);
                    m_lastScanCode = 0;
                }
            }

            if (scanCode == m_lastScanCode && a_event->IsPressed() && (a_event->HeldDuration() - m_keyHeldDuration) > KEY_CHAR_REPEAT_DELAY)
            {
                m_keyHeldDuration = a_event->HeldDuration();

                const auto vkCode = NL::Utils::InputConverter::GetVirtualKey(scanCode);
                m_lastScanCode = scanCode;
                m_lastCharCefKeyEvent.type = cef_key_event_type_t::KEYEVENT_KEYDOWN;
                m_lastCharCefKeyEvent.windows_key_code = vkCode;
                browserHost->SendKeyEvent(m_lastCharCefKeyEvent);

                if (NL::Utils::InputConverter::ShouldConvertToChar(scanCode, vkCode))
                {
                    m_lastCharCefKeyEvent.type = cef_key_event_type_t::KEYEVENT_CHAR;
                    m_lastCharCefKeyEvent.windows_key_code = NL::Utils::InputConverter::VkCodeToChar(scanCode, vkCode, m_cefKeyModifiers & (EVENTFLAG_SHIFT_DOWN | EVENTFLAG_CAPS_LOCK_ON));
                    browserHost->SendKeyEvent(m_lastCharCefKeyEvent);
                }
            }
            break;
        }
        default:
            break;
        }

        return true;
    }

#pragma endregion

}
