#pragma once

namespace NL::CEF
{
    class NirnLabCefApp : public CefApp
    {
        IMPLEMENT_REFCOUNTING(NirnLabCefApp);

      public:
        virtual ~NirnLabCefApp() = default;
        void OnBeforeCommandLineProcessing(CefString const& process_type, CefRefPtr<CefCommandLine> command_line) override;
    };
}
