#include "CEFService.h"

namespace NL::Services
{
    void CEFService::CEFInitialize(CefRefPtr<CefApp> a_cefApp, const CefSettings& a_cefSettings)
    {
        std::lock_guard locker(s_cefInitMutex);

        if (s_cefApp != nullptr)
        {
            throw std::runtime_error(fmt::format("{}: CEF already inited", NameOf(CEFService)));
        }

        CefMainArgs args(GetModuleHandleA(nullptr));
        if (!CefInitialize(args, a_cefSettings, a_cefApp, nullptr))
        {
            throw std::runtime_error(fmt::format("{}: failed to initialize CEF, code {}", NameOf(CEFService), CefGetExitCode()));
        }

        spdlog::info("CEFService::CEFInitialize successfully");
        s_cefApp = a_cefApp;
    }

    void CEFService::CEFShutdown()
    {
        std::lock_guard locker(s_cefInitMutex);

        if (s_cefApp == nullptr)
        {
            return;
        }

        CefShutdown();
        s_cefApp = nullptr;
    }

    bool CEFService::CreateBrowser(const CefRefPtr<CefClient> a_client,
                                   const CefRefPtr<CefDictionaryValue> a_jsFuncInfo,
                                   const CefString a_url,
                                   const CefWindowInfo& a_cefWindowInfo,
                                   const CefBrowserSettings& a_cefBrowserSettings)
    {
        return CefBrowserHost::CreateBrowser(
            a_cefWindowInfo,
            a_client,
            a_url,
            a_cefBrowserSettings,
            a_jsFuncInfo,
            nullptr);
    }
}
