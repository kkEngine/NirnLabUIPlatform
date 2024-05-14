#include "CEFBrowser.h"

namespace NL::CEF
{
    CEFBrowser::CEFBrowser(
        std::shared_ptr<spdlog::logger> a_logger,
        std::shared_ptr<NL::Services::CEFService> a_cefService,
        const std::wstring_view a_startUrl)
    {
        ThrowIfNullptr(CEFBrowser, a_logger);
        m_logger = a_logger;

        ThrowIfNullptr(CEFBrowser, a_cefService);
        m_cefService = a_cefService;

        m_cefClient = CefRefPtr<NirnLabCefClient>(new NirnLabCefClient());
        if (!m_cefService->CreateBrowser(m_cefClient, CefString(a_startUrl.data())))
        {
            spdlog::error("{}: failed to create browser", NameOf(CEFBrowser));
        }
    }

    CefRefPtr<NirnLabCefClient> CEFBrowser::GetCefClient()
    {
        return m_cefClient;
    }

    bool CEFBrowser::IsReadyAndLog()
    {
        const auto result = IsReady();
        if (!result)
        {
            m_logger->info("{}: browser is still loading, try later", NameOf(CEFBrowser));
        }
        return result;
    }

    bool __cdecl CEFBrowser::IsReady()
    {
        return m_cefClient && m_cefClient->IsBrowserReady();
    }

    void __cdecl CEFBrowser::SetVisible(bool a_value)
    {
        m_cefClient->GetCefRenderLayer()->SetVisible(a_value);
    }

    bool __cdecl CEFBrowser::IsVisible()
    {
        return m_cefClient->GetCefRenderLayer()->GetVisible();
    }

    void __cdecl CEFBrowser::SetFocused(bool a_value)
    {
        if (!IsReadyAndLog())
        {
            return;
        }

        m_cefClient->GetBrowser()->GetHost()->SetFocus(a_value);
    }

    bool __cdecl CEFBrowser::IsFocused()
    {
        if (!IsReadyAndLog())
        {
            return false;
        }

        const auto frame = m_cefClient->GetBrowser()->GetMainFrame();
        return frame != nullptr && frame->IsFocused();
    }

    void __cdecl CEFBrowser::LoadURL(const char* a_url)
    {
        if (!IsReadyAndLog())
        {
            return;
        }

        const auto frame = m_cefClient->GetBrowser()->GetMainFrame();
        if (frame)
        {
            frame->LoadURL(CefString(a_url));
        }
        else
        {
            m_logger->error("{}: can't get main frame to load url \"{}\"", NameOf(CEFBrowser), a_url);
        }
    }

    void __cdecl CEFBrowser::SendMsg()
    {
        if (!IsReadyAndLog())
        {
            return;
        }
    }
}
