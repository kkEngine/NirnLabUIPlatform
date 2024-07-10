#include "LoadYoutubeTestCase.h"

namespace NL::UI::TestCase
{
    void LoadYoutubeTestCase::Start(NL::UI::IUIPlatformAPI* a_api)
    {
        m_browserHandle = a_api->AddOrGetBrowser("YOUTUBE_TEST_CEF", nullptr, 0, "https://youtube.com", m_browser);

        if (m_browserHandle == NL::UI::IUIPlatformAPI::InvalidBrowserRefHandle)
        {
            spdlog::error("LoadYoutubeTestCase: browser handle is invalid");
            return;
        }

        if (m_browser == nullptr)
        {
            spdlog::error("LoadYoutubeTestCase: browser is nullptr");
            return;
        }
    }
}
