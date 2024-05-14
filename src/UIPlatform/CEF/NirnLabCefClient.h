#pragma once

#include "PCH.h"
#include "Render/CEFRenderLayer.h"

namespace NL::CEF
{
    class NirnLabCefClient : public CefClient,
                             public CefLifeSpanHandler
    {
        IMPLEMENT_REFCOUNTING(NirnLabCefClient);

      protected:
        std::shared_ptr<NL::Render::CEFRenderLayer> m_cefRenderLayer = nullptr;
        CefRefPtr<CefBrowser> m_pCefBrowser = nullptr;

      public:
        NirnLabCefClient();
        virtual ~NirnLabCefClient() override = default;

        std::shared_ptr<NL::Render::CEFRenderLayer> GetCefRenderLayer();
        CefRefPtr<CefBrowser> GetBrowser();
        bool IsBrowserReady();

        // CefClient
        CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
        CefRefPtr<CefRenderHandler> GetRenderHandler() override;

        // CefLifeSpanHandler
        void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    };
}
