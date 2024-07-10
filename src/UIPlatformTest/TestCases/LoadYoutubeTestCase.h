#pragma once

#include "PCH.h"

namespace NL::UI::TestCase
{
    class LoadYoutubeTestCase
    {
        NL::CEF::IBrowser* m_browser = nullptr;
        NL::UI::IUIPlatformAPI::BrowserRefHandle m_browserHandle = NL::UI::IUIPlatformAPI::InvalidBrowserRefHandle;

      public:
        void Start(NL::UI::IUIPlatformAPI* a_api);
    };
}
