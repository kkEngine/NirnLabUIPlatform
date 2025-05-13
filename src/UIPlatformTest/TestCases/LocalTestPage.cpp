#include "LocalTestPage.h"

namespace NL::UI::TestCase
{
    LocalTestPage::LocalTestPage()
    {
        std::lock_guard locker(s_thisVectorMutex);
        s_thisVector.push_back(this);
    }

    LocalTestPage::~LocalTestPage()
    {
        std::lock_guard locker(s_thisVectorMutex);
        for (auto it = s_thisVector.begin(); it != s_thisVector.end(); ++it)
        {
            if (*it == this)
            {
                s_thisVector.erase(it);
                break;
            }
        }
        Shutdown();
    }

    void LocalTestPage::Start(NL::UI::IUIPlatformAPI* a_api)
    {
        a_api->RegisterOnShutdown(&LocalTestPage::StaticShutdown);

        // func1
        auto func1 = new JS::JSFuncInfo();
        func1->objectName = "NL";
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
        func2->objectName = "NL";
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

        // Event func
        auto func3 = new JS::JSFuncInfo();
        func3->objectName = "NL";
        func3->funcName = "addEventListener";
        func3->callbackData.isEventFunction = true;

        m_funcInfoVector.push_back(func1);
        m_funcInfoVector.push_back(func2);
        m_funcInfoVector.push_back(func3);

        m_browserHandle = a_api->AddOrGetBrowser("LOCAL_TEST_PAGE",
                                                 m_funcInfoVector.data(),
                                                 static_cast<std::uint32_t>(m_funcInfoVector.size()),
                                                 "https://www.youtube.com",
                                                 m_browser);

        if (m_browserHandle == NL::UI::IUIPlatformAPI::InvalidBrowserRefHandle)
        {
            spdlog::error("{}: browser handle is invalid", NameOf(LocalTestPage));
            return;
        }

        if (m_browser == nullptr)
        {
            spdlog::error("{}: browser is nullptr", NameOf(LocalTestPage));
            return;
        }

        m_printThread = std::make_shared<std::jthread>([=]() {
            const auto stopToken = m_printThread->get_stop_source().get_token();
            std::this_thread::sleep_for(12s);

            m_browser->LoadBrowserURL("file:///_testLocalPage.html", true);

            // func1
            JS::JSFuncInfoString strFunInfo("NL", "func1");
            strFunInfo.callbackData.executeInGameThread = false;
            strFunInfo.callbackData.callback = [](const char** a_args, int a_argsCount) {
                std::string argsStr = "";
                for (auto i = 0; i < a_argsCount; ++i)
                {
                    argsStr += fmt::format("{}{}", (i > 0 ? ", " : ""), a_args[i]);
                }

                spdlog::info("func1__ callback. params: {}", argsStr);
            };
            m_browser->AddFunctionCallback(strFunInfo);

            while (!m_browser->IsPageLoaded())
            {
                std::this_thread::sleep_for(100ms);
            }
            // m_browser->RemoveFunctionCallback(strFunInfo.objectName, strFunInfo.funcName);

            int i = 0;
            while (i < 10 && !stopToken.stop_requested())
            {
                std::this_thread::sleep_for(1s);
                m_browser->ExecEventFunction("on:message", "EVENT_FUNC WORKS!");
                m_browser->ExecuteJavaScript(fmt::format("NL.func1({})", std::to_string(++i).c_str()).c_str());
            }

            a_api->ReleaseBrowserHandle(m_browserHandle);
        });
    }

    void LocalTestPage::Shutdown()
    {
        spdlog::info("LocalTestPage::Shutdown");

        auto jthread = m_printThread;
        if (jthread != nullptr)
        {
            jthread->request_stop();
            jthread->join();
        }
    }
}
