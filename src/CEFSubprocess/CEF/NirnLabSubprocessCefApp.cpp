#include "NirnLabSubprocessCefApp.h"

namespace NL::CEF
{
    void NirnLabSubprocessCefApp::InitLog(std::filesystem::path a_logDirPath)
    {
        auto level = spdlog::level::info;
        a_logDirPath /= fmt::format("{}.log"sv, CEF_SUBPROCESS_PROJECT_NAME);
        auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(a_logDirPath.string(), true);
        auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifdef _DEBUG
        level = spdlog::level::trace;
        log->sinks().push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#endif

        log->set_level(level);
        log->flush_on(level);
        log->set_pattern("[%T.%e] [%^%l%$] : %v"s);

        spdlog::set_default_logger(std::move(log));
    }

    void NirnLabSubprocessCefApp::OnBeforeCommandLineProcessing(CefString const& process_type,
                                                                CefRefPtr<CefCommandLine> command_line)
    {
        const auto logDir = command_line->GetSwitchValue("log-directory");
        if (!logDir.empty())
        {
            InitLog(logDir.ToWString());
        }

        DWORD mainProcessId = std::stoi(command_line->GetSwitchValue("main-process-id").ToWString());
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

    void NirnLabSubprocessCefApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                                   CefRefPtr<CefFrame> frame,
                                                   CefRefPtr<CefV8Context> context)
    {
        std::uint32_t functionsCount = 0;
        auto currentObjectValue = context->GetGlobal();

        auto addFuncInfo = m_funcQueue.GetNext();
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

            addFuncInfo = m_funcQueue.GetNext();
        }

        // Sometimes this line doesn't print info. I don't know why (wrong thread?).
        spdlog::info("{}: registered {} functions for the browser with id {}", NameOf(OnContextCreated), functionsCount, browser->GetIdentifier());
    }

    void NirnLabSubprocessCefApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                                    CefRefPtr<CefFrame> frame,
                                                    CefRefPtr<CefV8Context> context)
    {
    }
}
