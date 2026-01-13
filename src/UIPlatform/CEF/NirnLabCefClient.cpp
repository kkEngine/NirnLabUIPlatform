#include "NirnLabCefClient.h"

namespace NL::CEF
{
    NirnLabCefClient::NirnLabCefClient(std::shared_ptr<NL::Providers::ICEFSettingsProvider> a_settingsProvider)
    {
        const auto rendererType = a_settingsProvider->GetGlobalSettings().rendererType;
        switch (rendererType)
        {
        case NL::UI::RendererType::DeferredContext:
        default: {
            using RenderLayer = NL::Render::CEFCopyRenderLayer;
            static_assert(std::is_base_of_v<CefRenderHandler, RenderLayer>);

            auto renderLayer = new RenderLayer();
            m_cefRenderLayer = CefRefPtr<NL::Render::IRenderLayer>(renderLayer);
            m_cefRenderHandler = CefRefPtr<CefRenderHandler>(static_cast<CefRenderHandler*>(renderLayer));
            break;
        }
        case NL::UI::RendererType::SyncCopy: {
            using RenderLayer = NL::Render::CEFSyncCopyRenderLayer;
            static_assert(std::is_base_of_v<CefRenderHandler, RenderLayer>);

            auto renderLayer = new RenderLayer();
            m_cefRenderLayer = CefRefPtr<NL::Render::IRenderLayer>(renderLayer);
            m_cefRenderHandler = CefRefPtr<CefRenderHandler>(static_cast<CefRenderHandler*>(renderLayer));
            break;
        }
        }
    }

    CefRefPtr<NL::Render::IRenderLayer> NirnLabCefClient::GetRenderLayer()
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
        return m_cefRenderHandler;
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
        spdlog::info("{}: browser with id {} using \"{}\" render layer", NameOf(NirnLabCefClient::OnAfterCreated), browser->GetIdentifier(), m_cefRenderLayer->GetName());
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
