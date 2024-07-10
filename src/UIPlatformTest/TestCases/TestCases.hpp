#pragma once

#include "LoadYoutubeTestCase.h"

using namespace NL::UI::TestCase;

namespace NL::UI::TestCase
{
    static std::shared_ptr<LoadYoutubeTestCase> s_youtubeTestCase = nullptr;

    void StartTests(NL::UI::IUIPlatformAPI* a_api)
    {
        s_youtubeTestCase = std::make_shared<LoadYoutubeTestCase>();
        s_youtubeTestCase->Start(a_api);
    }
}
