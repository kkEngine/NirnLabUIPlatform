#pragma once

#include "PCH.h"
#include "Log/IPCLogSink.hpp"
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

        std::shared_ptr<NL::Log::IPCLogSink_mt> m_logSink = nullptr;
        void InitLog(CefRefPtr<CefBrowser> a_browser);

      public:
        NirnLabSubprocessCefApp() = default;

        // CefApp
        void OnBeforeCommandLineProcessing(CefString const& process_type, CefRefPtr<CefCommandLine> command_line) override;
        CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

        // CefRenderProcessHandler
        void OnBrowserCreated(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefDictionaryValue> extra_info) override;
        void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) override;
        void OnContextCreated(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefV8Context> context) override;
        void OnContextReleased(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               CefRefPtr<CefV8Context> context) override;
    };
}
