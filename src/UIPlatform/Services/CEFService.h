#pragma once

#include "PCH.h"
#include "Providers/ICEFSettingsProvider.h"

namespace NL::Services
{
    class CEFService
    {
      protected:
        std::shared_ptr<spdlog::logger> m_logger = nullptr;
        std::shared_ptr<NL::Providers::ICEFSettingsProvider> m_cefSettingsProvider = nullptr;

        static inline std::mutex s_cefInitMutex;
        static inline bool s_isCefInited = false;

      public:
        CEFService(
            const std::shared_ptr<spdlog::logger> a_logger,
            const std::shared_ptr<NL::Providers::ICEFSettingsProvider> a_cefSettingsProvider);
        virtual ~CEFService() = default;

        bool CEFInitialize(const CefRefPtr<CefApp> a_cefApp);
        void CEFShutdown();
        bool CreateBrowser(const CefRefPtr<CefClient> a_client, const CefString a_url);
        bool CreateBrowser(const CefRefPtr<CefClient> a_client, const CefString a_url, const std::shared_ptr<NL::Providers::ICEFSettingsProvider> a_cefSettingsProvider);
    };
}
