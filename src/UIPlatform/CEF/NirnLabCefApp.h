#pragma once

#include "Services/UIPlatformService.h"

namespace NL::CEF
{
    class NirnLabCefApp : public CefApp,
                          public CefBrowserProcessHandler
    {
        IMPLEMENT_REFCOUNTING(NirnLabCefApp);

    public:
        ~NirnLabCefApp() override = default;

        // CefApp
        void OnBeforeCommandLineProcessing(CefString const& process_type, CefRefPtr<CefCommandLine> command_line) override;
        CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;

        // CefBrowserProcessHandler
        void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) override;
    };
}
