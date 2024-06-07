#pragma once

namespace NL::Utils
{
    class InputConverter
    {
      protected:
        static inline HKL m_currentHKL = GetKeyboardLayout(0);

      public:
        static std::uint32_t GetVirtualKey(const std::uint32_t a_scanCode);
        static bool ShouldConvertToChar(const std::uint32_t a_scanCode, const std::uint32_t a_vkCode);
        static wchar_t VkCodeToChar(const std::uint32_t a_scanCode, const std::uint32_t a_vkCode, bool a_isShift);
        static void NextKeyboardLayout();
    };
}
