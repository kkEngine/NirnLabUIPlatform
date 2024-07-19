#include "NirnLabSubprocessCefApp.h"

namespace NL::CEF
{
    void NirnLabSubprocessCefApp::InitLog(CefRefPtr<CefBrowser> a_browser)
    {
        auto level = spdlog::level::info;
        m_logSink = std::make_shared<NL::Log::IPCLogSink_mt>(a_browser);
        auto logger = std::make_shared<spdlog::logger>("global log"s, m_logSink);

#ifdef _DEBUG
        level = spdlog::level::trace;
        logger->sinks().push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif

        logger->set_level(level);
        logger->flush_on(level);
        logger->set_pattern("[%T.%e] [%^%l%$] : %v"s);

        spdlog::set_default_logger(logger);
    }

    void NirnLabSubprocessCefApp::OnBeforeCommandLineProcessing(CefString const& process_type,
                                                                CefRefPtr<CefCommandLine> command_line)
    {
        InitLog(nullptr);

        DWORD mainProcessId = std::stoi(command_line->GetSwitchValue(IPC_CL_PROCESS_ID_NAME).ToWString());
        if (mainProcessId)
        {
            new std::thread([=]() {
                const auto procHandle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, mainProcessId);
                ::WaitForSingleObject(procHandle, INFINITE);

                // Commented for possible future use
                // DWORD mainProcessExitCode = 0;
                // if (::GetExitCodeProcess(procHandle, &mainProcessExitCode))
                // {
                //     // log or email to sportloto?
                // }

                ::TerminateProcess(::GetCurrentProcess(), EXIT_SUCCESS);
            });
        }
    }

    CefRefPtr<CefRenderProcessHandler> NirnLabSubprocessCefApp::GetRenderProcessHandler()
    {
        return this;
    }

    void NirnLabSubprocessCefApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser,
                                                   CefRefPtr<CefDictionaryValue> extra_info)
    {
        m_logSink->SetBrowser(browser);

        if (extra_info != nullptr && extra_info->GetSize() > 0)
        {
            CefDictionaryValue::KeyList keyList;
            if (!extra_info->GetKeys(keyList))
            {
                spdlog::error("{}: error format in param {}", NameOf(NirnLabSubprocessCefApp::OnBrowserCreated), NameOf(extra_info));
            }
            else
            {
                for (const auto& key : keyList)
                {
                    const auto funcList = extra_info->GetList(key);
                    for (auto i = 0; i < funcList->GetSize(); ++i)
                    {
                        const auto funcName = funcList->GetValue(i)->GetString();
                        if (!funcName.empty())
                        {
                            m_funcQueue.AddFunction(key, funcName);
                        }
                        else
                        {
                            spdlog::error("{}: error format in param {}, key {}", NameOf(NirnLabSubprocessCefApp::OnBrowserCreated), NameOf(extra_info), key.ToString().c_str());
                        }
                    }
                }
            }
        }
    }

    void NirnLabSubprocessCefApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser)
    {
        m_logSink->SetBrowser(nullptr);
    }

    void NirnLabSubprocessCefApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                                   CefRefPtr<CefFrame> frame,
                                                   CefRefPtr<CefV8Context> context)
    {
        if (!frame->IsMain())
        {
            return;
        }

        std::uint32_t functionsCount = 0;
        auto currentObjectValue = context->GetGlobal();

        auto addFuncInfo = m_funcQueue.PopNext();
        while (addFuncInfo != nullptr)
        {
            ++functionsCount;
            if (!addFuncInfo->objectName.empty() || addFuncInfo->objectName != IPC_JS_WINDOW_OBJECT_NAME)
            {
                auto objectValue = currentObjectValue->GetValue(addFuncInfo->objectName);
                if (objectValue == nullptr || objectValue->IsNull() || objectValue->IsUndefined())
                {
                    objectValue = CefV8Value::CreateObject(nullptr, nullptr);
                    currentObjectValue->SetValue(addFuncInfo->objectName, objectValue, V8_PROPERTY_ATTRIBUTE_NONE);
                }
                currentObjectValue = objectValue;
            }

            CefRefPtr<NL::JS::CEFFunctionHandler> funcHandler = new NL::JS::CEFFunctionHandler(browser, addFuncInfo->objectName);
            CefRefPtr<CefV8Value> funcValue = CefV8Value::CreateFunction(addFuncInfo->functionName, funcHandler);
            currentObjectValue->SetValue(addFuncInfo->functionName, funcValue, V8_PROPERTY_ATTRIBUTE_NONE);

            addFuncInfo = m_funcQueue.PopNext();
        }

        spdlog::info("{}: registered {} functions for the browser with id {}", NameOf(OnContextCreated), functionsCount, browser->GetIdentifier());
    }

    void NirnLabSubprocessCefApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                                    CefRefPtr<CefFrame> frame,
                                                    CefRefPtr<CefV8Context> context)
    {
    }
}
