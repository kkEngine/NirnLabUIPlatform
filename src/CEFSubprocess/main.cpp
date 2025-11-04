#include "CEF/NirnLabSubprocessCefApp.h"
#include "ProcessDownDetector.h"

std::unique_ptr<NL::ProcessDownDetector> g_mainProcessDownDetector = nullptr;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    void* sandbox_info = nullptr;
#if defined(CEF_USE_SANDBOX)
    // Manage the life span of the sandbox information object. This is necessary
    // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
    CefScopedSandboxInfo scoped_sandbox;
    sandbox_info = scoped_sandbox.sandbox_info();
#endif

    auto cmdLine = CefCommandLine::CreateCommandLine();
    cmdLine->InitFromString(GetCommandLineW());

    DWORD mainProcessId = std::stoi(cmdLine->GetSwitchValue(IPC_CL_PROCESS_ID_NAME).ToWString());
    if (mainProcessId > 0)
    {
        g_mainProcessDownDetector = std::make_unique<NL::ProcessDownDetector>(mainProcessId, []() {
            CefShutdown();
        });
    }

    CefMainArgs main_args(hInstance);
    CefRefPtr<NL::CEF::NirnLabSubprocessCefApp> app(new NL::CEF::NirnLabSubprocessCefApp());

    return CefExecuteProcess(main_args, app, sandbox_info);
}
