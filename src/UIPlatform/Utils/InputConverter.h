#pragma once

namespace Utils
{
    class InputConverter
    {
      public:
        static std::uint32_t GetVirtualKey(const std::uint32_t a_scanCode);
        static bool shouldConvertToChar(const std::uint32_t a_scanCode, const std::uint32_t a_vkCode);
        static wchar_t VkCodeToChar(const std::uint32_t a_scanCode, const std::uint32_t a_vkCode, bool a_isShift);

        static void SetNextKeyboardLayout();
    };
}
