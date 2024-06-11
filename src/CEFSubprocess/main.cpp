#include "CEF/NirnLabSubprocessCefApp.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    void* sandbox_info = nullptr;
#if defined(CEF_USE_SANDBOX)
    // Manage the life span of the sandbox information object. This is necessary
    // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
    CefScopedSandboxInfo scoped_sandbox;
    sandbox_info = scoped_sandbox.sandbox_info();
#endif

    CefMainArgs main_args(hInstance);
    CefRefPtr<NL::CEF::NirnLabSubprocessCefApp> app(new NL::CEF::NirnLabSubprocessCefApp());

    return CefExecuteProcess(main_args, app, sandbox_info);
}
