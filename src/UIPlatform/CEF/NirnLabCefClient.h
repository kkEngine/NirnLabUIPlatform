#pragma once

#include "PCH.h"
#include "Render/CEFCopyRenderLayer.h"
#include "Render/CEFRenderLayer.h"
#include "CEF/IPCEventMediator.h"

namespace NL::CEF
{
    class NirnLabCefClient : public CefClient,
                             public CefLifeSpanHandler
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

        // CefClient
        CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
        CefRefPtr<CefRenderHandler> GetRenderHandler() override;
        bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefProcessId source_process,
                                      CefRefPtr<CefProcessMessage> message) override;

        // CefLifeSpanHandler
        void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    };
}
