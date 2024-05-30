#pragma once

#include "PCH.h"
#include "IBrowser.h"
#include "Render/CEFRenderLayer.h"
#include "CEF/NirnLabCefClient.h"
#include "Services/CEFService.h"

namespace NL::CEF
{
    class CEFBrowser : public IBrowser
    {
      protected:
        std::shared_ptr<spdlog::logger> m_logger = nullptr;
        CefRefPtr<NirnLabCefClient> m_cefClient = nullptr;

      public:
        CEFBrowser(
            std::shared_ptr<spdlog::logger> a_logger,
            CefRefPtr<NirnLabCefClient> a_cefClient);
        ~CEFBrowser() override = default;

        CefRefPtr<NirnLabCefClient> GetCefClient();
        bool IsReadyAndLog();

        // IBrowser
        bool __cdecl IsBrowserReady() override;

        void __cdecl SetBrowserVisible(bool a_value) override;
        bool __cdecl IsBrowserVisible() override;

        void __cdecl SetBrowserFocused(bool a_value) override;
        bool __cdecl IsBrowserFocused() override;

        void __cdecl LoadBrowserURL(const char* a_url) override;
        void __cdecl SendBrowserMsg() override;
    };
}
