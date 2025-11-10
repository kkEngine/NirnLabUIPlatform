#include "KeyInputConverter.h"

namespace NL::Converters
{
    void KeyInputConverter::KeyDown(const std::uint32_t a_scanCode, const std::uint32_t a_vkCode)
    {
        CefKeyEvent keyEvent{};
        keyEvent.windows_key_code = a_vkCode;
        keyEvent.native_key_code = a_scanCode;
        keyEvent.modifiers = m_currentModifiers;
        keyEvent.type = KEYEVENT_RAWKEYDOWN;
        OnKeyDown(keyEvent);

        const auto wchar = VkCodeToChar(a_scanCode, a_vkCode, m_currentModifiers & (EVENTFLAG_SHIFT_DOWN | EVENTFLAG_CAPS_LOCK_ON));
        if (wchar != 0)
        {
            keyEvent.type = KEYEVENT_CHAR;
            keyEvent.windows_key_code = wchar;
            OnChar(keyEvent);
        }
    }

    std::uint32_t KeyInputConverter::GetVirtualKey(const std::uint32_t a_scanCode)
    {
        if (a_scanCode == RE::BSKeyboardDevice::Keys::kKP_Enter)
        {
            return VK_RETURN;
        }

        std::uint32_t vkCode = 0;
        RE::BSInputDeviceManager::GetSingleton()->GetDeviceMappedKeycode(RE::INPUT_DEVICES::kKeyboard, a_scanCode, vkCode);
        return vkCode;
    }

    void KeyInputConverter::NextKeyboardLayout()
    {
        s_currentHKL = ActivateKeyboardLayout((HKL)HKL_NEXT, 0);
    }

    wchar_t KeyInputConverter::VkCodeToChar(const std::uint32_t a_scanCode, const std::uint32_t a_vkCode, const bool a_shift)
    {
        // auto keyboard = RE::BSInputDeviceManager::GetSingleton()->GetKeyboard();
        static uint8_t s_state[256] = {0};
        s_state[VK_SHIFT] = a_shift ? 0xFF : 0;

        wchar_t unicodeChar;
        if (ToUnicodeEx(a_vkCode, a_scanCode, s_state, &unicodeChar, 1, 0, s_currentHKL) != 1)
        {
            return 0;
        }

        return unicodeChar;
    }

    void KeyInputConverter::Clear()
    {
        m_currentModifiers = 0;
        m_lastScanCode = 0;
        m_lastKeyHeldDuration = 0.0f;
    }

    void KeyInputConverter::UpdateCefKeyModifiers(const cef_event_flags_t a_flags, bool a_isKeyDown)
    {
        if (a_isKeyDown)
        {
            m_currentModifiers |= a_flags;
        }
        else
        {
            m_currentModifiers &= ~a_flags;
        }
    }

    void KeyInputConverter::UpdateModifiersFromVK(const std::uint32_t a_vkCode, bool a_isKeyDown)
    {
        if (a_vkCode >= VK_NUMPAD0 && a_vkCode <= VK_DIVIDE)
        {
            UpdateCefKeyModifiers(EVENTFLAG_IS_KEY_PAD, a_isKeyDown);
        }
        else
        {
            switch (a_vkCode)
            {
            case VK_CAPITAL:
                UpdateCefKeyModifiers(EVENTFLAG_CAPS_LOCK_ON, a_isKeyDown);
                break;
            case VK_SHIFT:
                UpdateCefKeyModifiers(EVENTFLAG_SHIFT_DOWN, a_isKeyDown);
                break;
            case VK_CONTROL:
                UpdateCefKeyModifiers(EVENTFLAG_CONTROL_DOWN, a_isKeyDown);
                break;
            case VK_MENU:
                UpdateCefKeyModifiers(EVENTFLAG_ALT_DOWN, a_isKeyDown);
                break;
            case VK_NUMLOCK:
                UpdateCefKeyModifiers(EVENTFLAG_NUM_LOCK_ON, a_isKeyDown);
                break;
            case VK_LCONTROL:
                UpdateCefKeyModifiers(EVENTFLAG_CONTROL_DOWN, a_isKeyDown);
                UpdateCefKeyModifiers(EVENTFLAG_IS_LEFT, a_isKeyDown);
                break;
            case VK_LMENU:
                UpdateCefKeyModifiers(EVENTFLAG_ALT_DOWN, a_isKeyDown);
                UpdateCefKeyModifiers(EVENTFLAG_IS_LEFT, a_isKeyDown);
                break;
            case VK_LSHIFT:
                UpdateCefKeyModifiers(EVENTFLAG_SHIFT_DOWN, a_isKeyDown);
                UpdateCefKeyModifiers(EVENTFLAG_IS_LEFT, a_isKeyDown);
                break;
            case VK_RCONTROL:
                UpdateCefKeyModifiers(EVENTFLAG_CONTROL_DOWN, a_isKeyDown);
                UpdateCefKeyModifiers(EVENTFLAG_IS_RIGHT, a_isKeyDown);
                break;
            case VK_RMENU:
                UpdateCefKeyModifiers(EVENTFLAG_ALT_DOWN, a_isKeyDown);
                UpdateCefKeyModifiers(EVENTFLAG_IS_RIGHT, a_isKeyDown);
                break;
            case VK_RSHIFT:
                UpdateCefKeyModifiers(EVENTFLAG_SHIFT_DOWN, a_isKeyDown);
                UpdateCefKeyModifiers(EVENTFLAG_IS_RIGHT, a_isKeyDown);
                break;
            default:
                break;
            }
        }
    }

    std::uint32_t KeyInputConverter::GetCurrentModifiers()
    {
        return m_currentModifiers;
    }

    void KeyInputConverter::ProcessButton(const RE::ButtonEvent* a_event)
    {
        const auto isKeyStateChanged = a_event->IsDown() || a_event->IsUp();
        if (isKeyStateChanged)
        {
            const auto scanCode = a_event->GetIDCode();
            const auto vkCode = GetVirtualKey(scanCode);
            UpdateModifiersFromVK(vkCode, a_event->IsDown());

            if (a_event->IsDown())
            {
                m_lastScanCode = scanCode;
                m_lastKeyHeldDuration = KEY_FIRST_CHAR_DELAY;

                KeyDown(scanCode, vkCode);
            }
            else
            {
                CefKeyEvent keyEvent{};
                keyEvent.windows_key_code = vkCode;
                keyEvent.native_key_code = scanCode;
                keyEvent.modifiers = m_currentModifiers;
                keyEvent.type = KEYEVENT_KEYUP;

                OnKeyUp(keyEvent);
            }
        }
        else if (a_event->GetIDCode() == m_lastScanCode && (a_event->HeldDuration() - m_lastKeyHeldDuration) > KEY_CHAR_REPEAT_DELAY)
        {
            m_lastKeyHeldDuration = a_event->HeldDuration();

            KeyDown(a_event->GetIDCode(), GetVirtualKey(m_lastScanCode));
        }
    }

    void KeyInputConverter::ProcessAltTab()
    {
        Clear();

        if (!m_fakeAltTabButtonEvent)
        {
            m_fakeAltTabButtonEvent = RE::ButtonEvent::Create(RE::INPUT_DEVICE::kKeyboard,
                                                              "",
                                                              0,
                                                              0.0f,
                                                              1.0f);
        }

        m_fakeAltTabButtonEvent->idCode = RE::BSKeyboardDevice::Keys::kLeftAlt;
        ProcessButton(m_fakeAltTabButtonEvent);

        m_fakeAltTabButtonEvent->idCode = RE::BSKeyboardDevice::Keys::kTab;
        ProcessButton(m_fakeAltTabButtonEvent);
    }
}
