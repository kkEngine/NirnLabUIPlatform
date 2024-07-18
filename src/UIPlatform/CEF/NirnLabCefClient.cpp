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
        if (browser->IsSame(m_cefBrowser) && frame->IsMain())
        {
            onMainFrameLoadEnd();
        }
    }
}
