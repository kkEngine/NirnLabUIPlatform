#pragma once

#include "PCH.h"
#include "Render/CEFCopyRenderLayer.h"
#include "Render/CEFRenderLayer.h"

namespace NL::CEF
{
    class NirnLabCefClient : public CefClient,
                             public CefLifeSpanHandler,
                             public CefLoadHandler,
                             public CefJSDialogHandler
    {
        IMPLEMENT_REFCOUNTING(NirnLabCefClient);

    protected:
        std::shared_ptr<NL::Render::CEFCopyRenderLayer> m_cefRenderLayer = nullptr;
        CefRefPtr<CefBrowser> m_cefBrowser = nullptr;

    public:
        NirnLabCefClient();
        virtual ~NirnLabCefClient() override = default;

        std::shared_ptr<NL::Render::IRenderLayer> GetRenderLayer();
        CefRefPtr<CefBrowser> GetBrowser();
        bool IsBrowserReady();

        sigslot::signal<CefRefPtr<CefProcessMessage>> onIPCMessageReceived;
        sigslot::signal<CefRefPtr<CefBrowser>> onAfterBrowserCreated;
        sigslot::signal<CefRefPtr<CefBrowser>> onBeforeBrowserClose;
        sigslot::signal<> onMainFrameLoadStart;
        sigslot::signal<> onMainFrameLoadEnd;

        // CefClient
        CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
        CefRefPtr<CefLoadHandler> GetLoadHandler() override;
        CefRefPtr<CefRenderHandler> GetRenderHandler() override;
        CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override;
        bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefProcessId source_process,
                                      CefRefPtr<CefProcessMessage> message) override;

        // CefLifeSpanHandler
        void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
        void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

        // CefLoadHandler
        void OnLoadStart(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         TransitionType transition_type) override;
        void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       int httpStatusCode) override;
        virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 ErrorCode errorCode,
                                 const CefString& errorText,
                                 const CefString& failedUrl) override;

        // CefJSDialogHandler
        virtual bool OnJSDialog(CefRefPtr<CefBrowser> browser,
                                const CefString& origin_url,
                                JSDialogType dialog_type,
                                const CefString& message_text,
                                const CefString& default_prompt_text,
                                CefRefPtr<CefJSDialogCallback> callback,
                                bool& suppress_message) override;
    };
}
