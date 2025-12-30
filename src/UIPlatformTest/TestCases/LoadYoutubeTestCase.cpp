#include "LoadYoutubeTestCase.h"

namespace NL::UI::TestCase
{
    void LoadYoutubeTestCase::Start(NL::UI::IUIPlatformAPI* a_api)
    {
        NL::UI::BrowserSettings bSettings{};
        // bSettings.frameRate = 30;

        // m_browserHandle = a_api->AddOrGetBrowser("YOUTUBE_TEST_CEF", nullptr, 0, "https://youtube.com", m_browser);
        m_browserHandle = a_api->AddOrGetBrowser("YOUTUBE_TEST_CEF", nullptr, 0, "https://google.com", &bSettings, m_browser);

        if (m_browserHandle == NL::UI::IUIPlatformAPI::InvalidBrowserRefHandle)
        {
            spdlog::error("{}: browser handle is invalid", NameOf(LoadYoutubeTestCase));
            return;
        }

        if (m_browser == nullptr)
        {
            spdlog::error("{}: browser is nullptr", NameOf(LoadYoutubeTestCase));
            return;
        }

        m_browser->ToggleBrowserFocusByKeys(RE::BSKeyboardDevice::Keys::kF6, 0);
        m_browser->ToggleBrowserVisibleByKeys(RE::BSKeyboardDevice::Keys::kF7, 0);
        m_browser->SetBrowserFocused(true);
    }
}
