#include "WinProcHook.h"

namespace NL::Hooks
{
    LRESULT WinProcHook::WinProcFuncHook(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_ACTIVATE && wParam == WA_INACTIVE)
        {
            OnWindowInactive(ShutdownHook::IsGameClosing);
        }

        if (uMsg == WM_INPUTLANGCHANGE || uMsg == WM_INPUTLANGCHANGEREQUEST)
        {
            OnWndInputLangChange();
        }

        return s_winProcFuncOrigin(hwnd, uMsg, wParam, lParam);
    }

    ATOM WinProcHook::RegisterClassAHook(WNDCLASSA* lpWndClass)
    {
        s_winProcFuncOrigin = lpWndClass->lpfnWndProc;
        lpWndClass->lpfnWndProc = &WinProcHook::WinProcFuncHook;

        return reinterpret_cast<decltype(&WinProcHook::RegisterClassAHook)>(s_registerClassAOrigin)(lpWndClass);
    }

    bool WinProcHook::Install()
    {
        s_registerClassAOrigin = SKSE::PatchIAT(&WinProcHook::RegisterClassAHook, "user32.dll", "RegisterClassA");

        return true;
    }
}
