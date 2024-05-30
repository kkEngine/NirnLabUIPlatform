#include "CEFBrowser.h"

namespace NL::CEF
{
    CEFBrowser::CEFBrowser(
        std::shared_ptr<spdlog::logger> a_logger,
        CefRefPtr<NirnLabCefClient> a_cefClient)
    {
        ThrowIfNullptr(CEFBrowser, a_logger);
        m_logger = a_logger;

        ThrowIfNullptr(CEFBrowser, a_cefClient);
        m_cefClient = a_cefClient;
    }

    CefRefPtr<NirnLabCefClient> CEFBrowser::GetCefClient()
    {
        return m_cefClient;
    }

    bool CEFBrowser::IsReadyAndLog()
    {
        const auto result = IsBrowserReady();
        if (!result)
        {
            m_logger->info("{}: browser is still loading, try later", NameOf(CEFBrowser));
        }
        return result;
    }

    bool __cdecl CEFBrowser::IsBrowserReady()
    {
        return m_cefClient && m_cefClient->IsBrowserReady();
    }

    void __cdecl CEFBrowser::SetBrowserVisible(bool a_value)
    {
        m_cefClient->GetCefRenderLayer()->SetVisible(a_value);
    }

    bool __cdecl CEFBrowser::IsBrowserVisible()
    {
        return m_cefClient->GetCefRenderLayer()->GetVisible();
    }

    void __cdecl CEFBrowser::SetBrowserFocused(bool a_value)
    {
        if (!IsReadyAndLog())
        {
            return;
        }

        m_cefClient->GetBrowser()->GetHost()->SetFocus(a_value);
    }

    bool __cdecl CEFBrowser::IsBrowserFocused()
    {
        if (!IsReadyAndLog())
        {
            return false;
        }

        const auto frame = m_cefClient->GetBrowser()->GetMainFrame();
        return frame != nullptr && frame->IsFocused();
    }

    void __cdecl CEFBrowser::LoadBrowserURL(const char* a_url)
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

    void __cdecl CEFBrowser::SendBrowserMsg()
    {
        if (!IsReadyAndLog())
        {
            return;
        }
    }
}
