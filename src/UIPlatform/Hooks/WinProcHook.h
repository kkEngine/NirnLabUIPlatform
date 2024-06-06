#pragma once

#include "PCH.h"

#define WNDPROC_CONTINUE 0
#define WNDPROC_PROCESSED 1

namespace NL::Hooks
{
    class WinProcHook
    {
      public:
        static inline WNDPROC s_winProcFuncOrigin = nullptr;
        static LRESULT CALLBACK WinProcFuncHook(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        static inline std::uintptr_t s_registerClassAOrigin = NULL;
        static ATOM CALLBACK RegisterClassAHook(WNDCLASSA* lpWndClass);

      public:
        // ALT + TAB
        static inline sigslot::signal<> OnWndInactive;
        static inline sigslot::signal<> OnWndInputLangChange;
        static bool Install();
    };
}
