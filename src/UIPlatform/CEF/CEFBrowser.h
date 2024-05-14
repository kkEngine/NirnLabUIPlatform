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
        std::shared_ptr<NL::Services::CEFService> m_cefService = nullptr;
        CefRefPtr<NirnLabCefClient> m_cefClient = nullptr;

      public:
        CEFBrowser(
            std::shared_ptr<spdlog::logger> a_logger,
            std::shared_ptr<NL::Services::CEFService> a_cefService,
            const std::wstring_view a_startUrl = L"");
        ~CEFBrowser() override = default;

        virtual CefRefPtr<NirnLabCefClient> GetCefClient();
        bool IsReadyAndLog();

        // IBrowser
        bool __cdecl IsReady() override;

        void __cdecl SetVisible(bool a_value) override;
        bool __cdecl IsVisible() override;

        void __cdecl SetFocused(bool a_value) override;
        bool __cdecl IsFocused() override;

        void __cdecl LoadURL(const char* a_url) override;
        void __cdecl SendMsg() override;
    };
}
