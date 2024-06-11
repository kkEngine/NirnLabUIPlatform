#pragma once

#include "PCH.h"
#include "JS/CEFFunctionQueue.h"
#include "JS/CEFFunctionHandler.h"

namespace NL::CEF
{
    class NirnLabSubprocessCefApp final : public CefApp,
                                          public CefRenderProcessHandler
    {
        IMPLEMENT_REFCOUNTING(NirnLabSubprocessCefApp);

      private:
        NL::JS::CEFFunctionQueue m_funcQueue;

        void InitLog(std::filesystem::path a_logDirPath);

      public:
        NirnLabSubprocessCefApp() = default;

        // CefApp
        void OnBeforeCommandLineProcessing(CefString const& process_type, CefRefPtr<CefCommandLine> command_line) override;
        CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

        // CefRenderProcessHandler
        void OnBrowserCreated(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefDictionaryValue> extra_info) override;
        void OnContextCreated(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefV8Context> context) override;
        void OnContextReleased(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               CefRefPtr<CefV8Context> context) override;
    };
}
