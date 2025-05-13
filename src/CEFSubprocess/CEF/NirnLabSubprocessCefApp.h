#pragma once

#define RENDER_PROCESS_TYPE "renderer"

#include "PCH.h"
#include "CEFV8ContextGuard.h"
#include "Log/IPCLogSink.hpp"
#include "JS/CEFFunctionQueue.h"
#include "JS/CEFFunctionHandler.h"
#include "JS/CEFEventFunctionHandler.h"

namespace NL::CEF
{
    class NirnLabSubprocessCefApp final : public CefApp,
                                          public CefRenderProcessHandler
    {
        IMPLEMENT_REFCOUNTING(NirnLabSubprocessCefApp);

    private:
        std::shared_ptr<NL::Log::IPCLogSink_mt> m_logSink = nullptr;
        CefString m_processType;
        CefRefPtr<CefDictionaryValue> m_extraInfo = nullptr;

        void InitLog(CefRefPtr<CefBrowser> a_browser);

    public:
        NirnLabSubprocessCefApp() = default;

        CefRefPtr<CefV8Value> GetOrCreateObject(CefRefPtr<CefV8Value> a_parent, const CefString& a_objectName);
        size_t AddFunctionHandlers(CefRefPtr<CefBrowser> a_browser,
                                   CefRefPtr<CefFrame> a_frame,
                                   CefProcessId a_sourceProcess,
                                   CefRefPtr<CefDictionaryValue> a_funcDict);
        size_t RemoveFunctionHandlers(CefRefPtr<CefBrowser> a_browser,
                                      CefRefPtr<CefFrame> a_frame,
                                      CefProcessId a_sourceProcess,
                                      CefRefPtr<CefDictionaryValue> a_funcDict);

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
        bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefProcessId source_process,
                                      CefRefPtr<CefProcessMessage> message) override;
    };
}
