#pragma once

#include "PCH.h"

namespace NL::UI::TestCase
{
    class LocalTestPage
    {
        NL::CEF::IBrowser* m_browser = nullptr;
        NL::UI::IUIPlatformAPI::BrowserRefHandle m_browserHandle = NL::UI::IUIPlatformAPI::InvalidBrowserRefHandle;

        std::vector<JS::JSFuncInfo*> m_funcInfoVector;

        std::shared_ptr<std::thread> m_pingThread = nullptr;

      public:
        void Start(NL::UI::IUIPlatformAPI* a_api);
    };
}
