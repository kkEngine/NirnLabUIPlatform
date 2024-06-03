#include "InputConverter.h"

namespace Utils
{
    std::uint32_t InputConverter::GetVirtualKey(const std::uint32_t a_scanCode)
    {
        std::uint32_t vkCode = 0;
        RE::BSInputDeviceManager::GetSingleton()->GetDeviceMappedKeycode(RE::INPUT_DEVICES::kKeyboard, a_scanCode, vkCode);
        return vkCode;
    }

    bool InputConverter::shouldConvertToChar(const std::uint32_t a_scanCode, const std::uint32_t a_vkCode)
    {
        if (a_vkCode >= VK_F1 && a_vkCode <= VK_F24)
        {
            return false;
        }

        switch (a_vkCode)
        {
        case VK_BACK:
        case VK_LEFT:
        case VK_RIGHT:
        case VK_UP:
        case VK_DOWN:
        case VK_DELETE:
            return false;
        default:
            break;
        }

        return true;
    }

    wchar_t InputConverter::VkCodeToChar(const std::uint32_t a_scanCode, const std::uint32_t a_vkCode, bool a_isShift)
    {
        static uint8_t state[256] = {0};
        state[VK_SHIFT] = a_isShift << 7;

        wchar_t unicodeChar;
        if (ToUnicodeEx(a_vkCode, a_scanCode, state, &unicodeChar, 1, 0, GetKeyboardLayout(0)) != 1)
            return L'\0';

        if (!std::iswprint(unicodeChar))
            return L'\0';

        return unicodeChar;
    }

    void InputConverter::SetNextKeyboardLayout()
    {
        ActivateKeyboardLayout((HKL)HKL_NEXT, KLF_SETFORPROCESS);
    }
}
