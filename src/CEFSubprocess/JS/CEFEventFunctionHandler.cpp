#include "CEFEventFunctionHandler.h"

namespace NL::JS
{
    void CEFEventFunctionHandler::CallEventFunc(const CefString& a_eventName, CefRefPtr<CefBrowser> a_browser, const CefString& a_data)
    {
        std::lock_guard locker(s_eventFuncMapMutex);
        if (s_eventFuncMap.empty())
        {
            return;
        }

        const auto it = s_eventFuncMap.find(std::make_pair(a_eventName.ToString(), a_browser->GetIdentifier()));
        if (it != s_eventFuncMap.end())
        {
            const auto context = std::get<0>(it->second);
            auto thisObject = std::get<1>(it->second);
            auto callback = std::get<2>(it->second);

            NL::CEF::CEFV8ContextGuard v8ContextGuard(context);
            if (!v8ContextGuard.IsEntered())
            {
                spdlog::error("{}[{}]: can't enter v8 context", NameOf(CEFEventFunctionHandler::CallEventFunc), ::GetCurrentProcessId());
                return;
            }

            CefV8ValueList arguments;
            arguments.push_back(CefV8Value::CreateString(a_data));
            callback->ExecuteFunction(thisObject, arguments);
        }
    }

    void CEFEventFunctionHandler::RemoveEventFunc(CefRefPtr<CefV8Context> a_context)
    {
        // Remove any JavaScript callbacks registered for the context that has been released.
        std::lock_guard locker(s_eventFuncMapMutex);
        if (s_eventFuncMap.empty())
        {
            return;
        }

        for (auto it = s_eventFuncMap.begin(); it != s_eventFuncMap.end();)
        {
            if (std::get<0>(it->second)->IsSame(a_context))
            {
                it = s_eventFuncMap.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    bool CEFEventFunctionHandler::Execute(const CefString& name,
                                          CefRefPtr<CefV8Value> object,
                                          const CefV8ValueList& arguments,
                                          CefRefPtr<CefV8Value>& retval,
                                          CefString& exception)
    {
        if (arguments.size() != 2 || !arguments[0]->IsString() || !arguments[1]->IsFunction())
        {
            exception = "This function has wrong signature! First argument should be event name (string), second should be function callback";
            spdlog::error("{}: trying to call event function with wrong signature, \"{}\"", NameOf(CEFEventFunctionHandler), name.ToString().c_str());
            return true;
        }

        const auto eventName = arguments[0]->GetStringValue();
        const auto context = CefV8Context::GetCurrentContext();
        const auto browserId = context->GetBrowser()->GetIdentifier();

        std::lock_guard locker(s_eventFuncMapMutex);
        s_eventFuncMap.erase(std::make_pair(eventName, browserId));
        s_eventFuncMap.insert(std::make_pair(std::make_pair(eventName, browserId),
                                             std::make_tuple(context, object, arguments[1])));

        return true;
    }
}
