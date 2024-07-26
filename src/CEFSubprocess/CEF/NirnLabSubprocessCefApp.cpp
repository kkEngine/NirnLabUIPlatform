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

    size_t NirnLabSubprocessCefApp::AddFunctionHandlers(CefRefPtr<CefBrowser> a_browser,
                                                        CefRefPtr<CefFrame> a_frame,
                                                        CefProcessId a_sourceProcess,
                                                        CefRefPtr<CefDictionaryValue> a_funcDict)
    {
        size_t addedFuncCount = 0;

        const auto v8Context = a_frame->GetV8Context();
        if (!v8Context->Enter())
        {
            spdlog::error("{}[{}]: can't enter v8 context", NameOf(NirnLabSubprocessCefApp::AddFunctionHandlers), ::GetCurrentProcessId());
            return addedFuncCount;
        }

        CefDictionaryValue::KeyList keyList;
        if (!a_funcDict->GetKeys(keyList))
        {
            spdlog::error("{}[{}]: can't get keys from function dictionary", NameOf(NirnLabSubprocessCefApp::AddFunctionHandlers), ::GetCurrentProcessId());
        }
        else
        {
            for (const auto& objectName : keyList)
            {
                auto currentObjectValue = v8Context->GetGlobal();
                const auto funcList = a_funcDict->GetList(objectName);
                for (auto i = 0; i < funcList->GetSize(); ++i)
                {
                    const auto& funcName = funcList->GetString(i);
                    if (funcName.empty())
                    {
                        continue;
                    }

                    ++addedFuncCount;

                    if (!objectName.empty() || objectName != IPC_JS_WINDOW_OBJECT_NAME)
                    {
                        auto objectValue = currentObjectValue->GetValue(objectName);
                        if (objectValue == nullptr || objectValue->IsNull() || objectValue->IsUndefined())
                        {
                            objectValue = CefV8Value::CreateObject(nullptr, nullptr);
                            currentObjectValue->SetValue(objectName, objectValue, V8_PROPERTY_ATTRIBUTE_NONE);
                        }
                        currentObjectValue = objectValue;
                    }

                    CefRefPtr<NL::JS::CEFFunctionHandler> funcHandler = new NL::JS::CEFFunctionHandler(a_browser, objectName);
                    CefRefPtr<CefV8Value> funcValue = CefV8Value::CreateFunction(funcName, funcHandler);
                    currentObjectValue->SetValue(funcName, funcValue, V8_PROPERTY_ATTRIBUTE_NONE);
                }
            }
        }

        v8Context->Exit();
        return addedFuncCount;
    }

    void NirnLabSubprocessCefApp::OnBeforeCommandLineProcessing(CefString const& process_type,
                                                                CefRefPtr<CefCommandLine> command_line)
    {
        m_processType = process_type;
        InitLog(nullptr);

        DWORD mainProcessId = std::stoi(command_line->GetSwitchValue(IPC_CL_PROCESS_ID_NAME).ToWString());
        if (mainProcessId && process_type == RENDER_PROCESS_TYPE)
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

                std::this_thread::sleep_for(1.42s);
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
        spdlog::info("{}[{}]: browser created with id {}", NameOf(NirnLabSubprocessCefApp), ::GetCurrentProcessId(), browser->GetIdentifier());
    }

    void NirnLabSubprocessCefApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser)
    {
        spdlog::info("{}[{}]: browser destroyed with id {}", NameOf(NirnLabSubprocessCefApp), ::GetCurrentProcessId(), browser->GetIdentifier());
        m_logSink->SetBrowser(nullptr);
    }

    void NirnLabSubprocessCefApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                                   CefRefPtr<CefFrame> frame,
                                                   CefRefPtr<CefV8Context> context)
    {
        if (frame->IsMain())
        {
            auto message = CefProcessMessage::Create(IPC_JS_CONTEXT_CREATED);
            frame->SendProcessMessage(PID_BROWSER, message);
        }
    }

    void NirnLabSubprocessCefApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                                    CefRefPtr<CefFrame> frame,
                                                    CefRefPtr<CefV8Context> context)
    {
    }

    bool NirnLabSubprocessCefApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                                           CefRefPtr<CefFrame> frame,
                                                           CefProcessId source_process,
                                                           CefRefPtr<CefProcessMessage> message)
    {
        if (message->GetName() == IPC_JS_FUNCION_ADD_EVENT)
        {
            const auto funcDict = message->GetArgumentList()->GetDictionary(0);
            if (funcDict == nullptr)
            {
                return true;
            }

            const auto addedFuncCount = AddFunctionHandlers(browser, frame, source_process, funcDict);
            spdlog::info("{}[{}]: registered {} functions for the browser with id {}", NameOf(NirnLabSubprocessCefApp::OnProcessMessageReceived), ::GetCurrentProcessId(), addedFuncCount, browser->GetIdentifier());
        }

        return false;
    }
}
