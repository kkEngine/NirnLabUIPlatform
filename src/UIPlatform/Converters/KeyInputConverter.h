#pragma once

#include "PCH.h"

namespace NL::Converters
{
    class KeyInputConverter
    {
    protected:
        static inline HKL s_currentHKL = GetKeyboardLayout(0);

        std::uint32_t m_currentModifiers = 0;
        std::uint32_t m_lastScanCode = 0;
        float m_lastKeyHeldDuration = 0.0f;
        RE::ButtonEvent* m_fakeAltTabButtonEvent = nullptr;

        void KeyDown(const std::uint32_t a_scanCode, const std::uint32_t a_vkCode);

    public:
        static std::uint32_t GetVirtualKey(const std::uint32_t a_scanCode);
        static void NextKeyboardLayout();
        static wchar_t VkCodeToChar(const std::uint32_t a_scanCode, const std::uint32_t a_vkCode, const bool a_shift);

        sigslot::signal_st<CefKeyEvent&> OnKeyDown;
        sigslot::signal_st<CefKeyEvent&> OnKeyUp;
        sigslot::signal_st<CefKeyEvent&> OnChar;

        void Clear();
        void UpdateCefKeyModifiers(const cef_event_flags_t a_flags, bool a_isKeyDown);
        void UpdateModifiersFromVK(const std::uint32_t a_vkCode, bool a_isKeyDown);
        std::uint32_t GetCurrentModifiers();
        void ProcessButton(const RE::ButtonEvent* a_event);
        void ProcessAltTab();
    };
}
