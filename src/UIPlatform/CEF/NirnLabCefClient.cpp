#include "NirnLabCefClient.h"

namespace NL::CEF
{
    NirnLabCefClient::NirnLabCefClient()
    {
        m_cefRenderLayer = NL::Render::CEFCopyRenderLayer::make_shared();
    }

    std::shared_ptr<NL::Render::IRenderLayer> NirnLabCefClient::GetRenderLayer()
    {
        return m_cefRenderLayer;
    }

    CefRefPtr<CefBrowser> NirnLabCefClient::GetBrowser()
    {
        return m_cefBrowser;
    }

    bool NirnLabCefClient::IsBrowserReady()
    {
        return m_cefBrowser != nullptr;
    }

    CefRefPtr<CefLifeSpanHandler> NirnLabCefClient::GetLifeSpanHandler()
    {
        return this;
    }

    CefRefPtr<CefLoadHandler> NirnLabCefClient::GetLoadHandler()
    {
        return this;
    }

    CefRefPtr<CefRenderHandler> NirnLabCefClient::GetRenderHandler()
    {
        return m_cefRenderLayer.get();
    }

    CefRefPtr<CefJSDialogHandler> NirnLabCefClient::GetJSDialogHandler()
    {
        return this;
    }

    bool NirnLabCefClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                                    CefRefPtr<CefFrame> frame,
                                                    CefProcessId source_process,
                                                    CefRefPtr<CefProcessMessage> message)
    {
        onIPCMessageReceived(message);
        return true;
    }

    void NirnLabCefClient::OnAfterCreated(CefRefPtr<CefBrowser> browser)
    {
        m_cefBrowser = browser;
        onAfterBrowserCreated(browser);
    }

    void NirnLabCefClient::OnBeforeClose(CefRefPtr<CefBrowser> browser)
    {
        onBeforeBrowserClose(browser);
        m_cefBrowser = nullptr;
    }

    void NirnLabCefClient::OnLoadStart(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       TransitionType transition_type)
    {
        if (browser->IsSame(m_cefBrowser) && frame->IsMain())
        {
            onMainFrameLoadStart();
        }
    }

    void NirnLabCefClient::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     int httpStatusCode)
    {
        // YES, httpStatusCode can be negative o_O
        if (browser->IsSame(m_cefBrowser) && frame->IsMain() && httpStatusCode >= 0)
        {
            onMainFrameLoadEnd();
        }
    }

    void NirnLabCefClient::OnLoadError(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       ErrorCode errorCode,
                                       const CefString& errorText,
                                       const CefString& failedUrl)
    {
        spdlog::error("NirnLabCefClient::OnLoadError, url {}, text {}", failedUrl.ToString().data(), errorText.ToString().data());
    }

    bool NirnLabCefClient::OnJSDialog(CefRefPtr<CefBrowser> browser,
                                      const CefString& origin_url,
                                      JSDialogType dialog_type,
                                      const CefString& message_text,
                                      const CefString& default_prompt_text,
                                      CefRefPtr<CefJSDialogCallback> callback,
                                      bool& suppress_message)
    {
        suppress_message = false;
        return false;
    }

}
