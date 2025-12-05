#pragma once

#include "../PCH.h"
#include "Converters/KeyInputConverter.h"

namespace NL::Hooks
{
    class CharGeneratorHook
    {
    protected:
        static int ToUnicodeCall(UINT wVirtKey, UINT wScanCode, const BYTE* lpKeyState, LPWSTR pwszBuff, int cchBuff, UINT wFlags)
        {
            return ToUnicodeEx(wVirtKey, wScanCode, lpKeyState, pwszBuff, cchBuff, wFlags, NL::Converters::KeyInputConverter::GetCurrentKeyboardLayout());
        }
        static inline REL::Relocation<decltype(&ToUnicodeCall)> _ToUnicodeCall;

    public:
        static bool Install()
        {
            REL::Relocation<std::uintptr_t> toUnicodeFunc{RELOCATION_ID(0, 68782), REL::VariantOffset(0, 0x2CB, 0)};
            _ToUnicodeCall = SKSE::GetTrampoline().write_call<6>(toUnicodeFunc.address(), &ToUnicodeCall);

            return true;
        }
    };
}
