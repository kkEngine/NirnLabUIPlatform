#pragma once

#include "LoadYoutubeTestCase.h"
#include "LocalTestPage.h"

using namespace NL::UI::TestCase;

namespace NL::UI::TestCase
{
    static std::shared_ptr<LoadYoutubeTestCase> s_youtubeTestCase = nullptr;
    static std::shared_ptr<LocalTestPage> s_localTestPage = nullptr;

    void StartTests(NL::UI::IUIPlatformAPI* a_api)
    {
        s_youtubeTestCase = std::make_shared<LoadYoutubeTestCase>();
        s_youtubeTestCase->Start(a_api);

        s_localTestPage = std::make_shared<LocalTestPage>();
        s_localTestPage->Start(a_api);
    }
}
