#pragma once

#include "PCH.h"

namespace NL::Services
{
    class CEFService
    {
    protected:
        static inline std::mutex s_cefInitMutex;
        static inline CefRefPtr<CefApp> s_cefApp = nullptr;

    public:
        static void CEFInitialize(CefRefPtr<CefApp> a_cefApp, const CefSettings& a_cefSettings);
        static void CEFShutdown();
        static bool CreateBrowser(const CefRefPtr<CefClient> a_client,
                                  const CefRefPtr<CefDictionaryValue> a_jsFuncInfo,
                                  const CefString a_url,
                                  const CefWindowInfo& a_cefWindowInfo,
                                  const CefBrowserSettings& a_cefBrowserSettings);
    };
}
