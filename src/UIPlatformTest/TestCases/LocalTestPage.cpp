#include "LocalTestPage.h"

namespace NL::UI::TestCase
{
    void LocalTestPage::Start(NL::UI::IUIPlatformAPI* a_api)
    {
        // func1
        auto func1 = new JS::JSFuncInfo();
        func1->objectName = "window";
        func1->funcName = "func1";
        func1->callbackData.executeInGameThread = false;
        func1->callbackData.callback = [](const char** a_args, int a_argsCount) {
            std::string argsStr = "";
            for (auto i = 0; i < a_argsCount; ++i)
            {
                argsStr += fmt::format("{}{}", (i > 0 ? ", " : ""), a_args[i]);
            }

            spdlog::info("func1 callback. params: {}", argsStr);
        };

        // func2
        auto func2 = new JS::JSFuncInfo();
        func2->objectName = "window";
        func2->funcName = "func2";
        func2->callbackData.executeInGameThread = true;
        func2->callbackData.callback = [](const char** a_args, int a_argsCount) {
            std::string argsStr = "";
            for (auto i = 0; i < a_argsCount; ++i)
            {
                argsStr += fmt::format("{}{}", (i > 0 ? ", " : ""), a_args[i]);
            }

            spdlog::info("func2 callback. params: {}", argsStr);
        };

        m_funcInfoVector.push_back(func1);
        m_funcInfoVector.push_back(func2);

        m_browserHandle = a_api->AddOrGetBrowser("LOCAL_TEST_PAGE",
                                                 m_funcInfoVector.data(),
                                                 static_cast<std::uint32_t>(m_funcInfoVector.size()),
                                                 "https://www.youtube.com",
                                                 m_browser);

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

        m_pingThread = std::make_shared<std::thread>([=]() {
            std::this_thread::sleep_for(12s);

            m_browser->LoadBrowserURL("file:///_testLocalPage.html");
            while (!m_browser->IsPageLoaded())
            {
                std::this_thread::sleep_for(100ms);
            }

            int i = 0;
            while (i < 10)
            {
                std::this_thread::sleep_for(1s);
                m_browser->ExecuteJavaScript(fmt::format("window.func1({})", std::to_string(++i).c_str()).c_str());
            }

            a_api->ReleaseBrowserHandle(m_browserHandle);
        });
    }
}
