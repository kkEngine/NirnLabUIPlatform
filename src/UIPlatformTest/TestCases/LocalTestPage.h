#pragma once

#include "PCH.h"

namespace NL::UI::TestCase
{
    class LocalTestPage
    {
        static inline std::mutex s_thisVectorMutex;
        static inline std::vector<LocalTestPage*> s_thisVector;

        static void StaticShutdown()
        {
            std::lock_guard locker(s_thisVectorMutex);
            for (auto& ptr : s_thisVector)
            {
                ptr->Shutdown();
            }
            s_thisVector.clear();
        }

        NL::CEF::IBrowser* m_browser = nullptr;
        NL::UI::IUIPlatformAPI::BrowserRefHandle m_browserHandle = NL::UI::IUIPlatformAPI::InvalidBrowserRefHandle;

        std::vector<JS::JSFuncInfo*> m_funcInfoVector;

        std::shared_ptr<std::jthread> m_printThread = nullptr;

    public:
        LocalTestPage();
        ~LocalTestPage();

        void Start(NL::UI::IUIPlatformAPI* a_api);
        void Shutdown();
    };
}
