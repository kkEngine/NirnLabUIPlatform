#pragma once

#include "PCH.h"
#include "Render/CEFCopyRenderLayer.h"
#include "Render/CEFRenderLayer.h"

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

        // CefClient
        CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
        CefRefPtr<CefRenderHandler> GetRenderHandler() override;

        // CefLifeSpanHandler
        void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    };
}
