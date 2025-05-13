#pragma once

#include "PCH.h"
#include "CEF/CEFV8ContextGuard.h"

namespace NL::JS
{
    class CEFEventFunctionHandler : public CefV8Handler
    {
        IMPLEMENT_REFCOUNTING(CEFEventFunctionHandler);

    protected:
        CefRefPtr<CefBrowser> m_browser = nullptr;

        static inline std::mutex s_eventFuncMapMutex;
        static inline std::map<std::pair<std::string, int>, std::tuple<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value>, CefRefPtr<CefV8Value>>> s_eventFuncMap;

    public:
        static void CallEventFunc(const CefString& a_eventName, CefRefPtr<CefBrowser> a_browser, const CefString& a_data);
        static void RemoveEventFunc(CefRefPtr<CefV8Context> a_context);

        // CefV8Handler
        bool Execute(const CefString& name,
                     CefRefPtr<CefV8Value> object,
                     const CefV8ValueList& arguments,
                     CefRefPtr<CefV8Value>& retval,
                     CefString& exception) override;
    };
}
