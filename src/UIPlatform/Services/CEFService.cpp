#include "CEFService.h"

namespace NL::Services
{
    CEFService::CEFService(
        const std::shared_ptr<spdlog::logger> a_logger,
        const std::shared_ptr<NL::Providers::ICEFSettingsProvider> a_cefSettingsProvider)
    {
        ThrowIfNullptr(CEFService, a_logger);
        m_logger = a_logger;

        ThrowIfNullptr(CEFService, a_cefSettingsProvider);
        m_cefSettingsProvider = a_cefSettingsProvider;
    }

    bool CEFService::CEFInitialize(CefRefPtr<CefApp> a_cefApp)
    {
        std::lock_guard lock(s_cefInitMutex);
        if (s_cefApp != nullptr)
        {
            m_logger->warn("{}: CEF already inited", NameOf(CEFService));
            return false;
        }

        CefMainArgs args(GetModuleHandleA(nullptr));
        if (!CefInitialize(args, m_cefSettingsProvider->GetCefSettings(), a_cefApp, nullptr))
        {
            m_logger->error("{}: failed to initialize CEF, code {}", NameOf(CEFService), CefGetExitCode());
            return false;
        }

        s_cefApp = a_cefApp;
        return true;
    }

    void CEFService::CEFShutdown()
    {
        std::lock_guard lock(s_cefInitMutex);
        if (s_cefApp == nullptr)
        {
            return;
        }

        CefShutdown();
        s_cefApp = nullptr;
    }

    bool CEFService::CreateBrowser(const CefRefPtr<CefClient> a_client, const CefString a_url)
    {
        return CreateBrowser(a_client, a_url, m_cefSettingsProvider);
    }

    bool CEFService::CreateBrowser(const CefRefPtr<CefClient> a_client, const CefString a_url, const std::shared_ptr<NL::Providers::ICEFSettingsProvider> a_cefSettingsProvider)
    {
        if (a_cefSettingsProvider == nullptr)
        {
            m_logger->error("{}: settings provider is nullptr", NameOf(CEFService));
            return false;
        }

        const auto result = CefBrowserHost::CreateBrowser(
            a_cefSettingsProvider->GetCefWindowInfo(),
            a_client,
            a_url,
            a_cefSettingsProvider->GetCefBrowserSettings(),
            nullptr,
            nullptr);

        if (!result)
        {
            m_logger->error("{}: failed to create browser", NameOf(CEFService));
        }

        return result;
    }
}
