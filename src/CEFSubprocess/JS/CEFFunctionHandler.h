#pragma once

#include "PCH.h"
#include "Converters/CEFValueConverter.h"

namespace NL::JS
{
    class CEFFunctionHandler : public CefV8Handler
    {
        IMPLEMENT_REFCOUNTING(CEFFunctionHandler);

    protected:
        CefRefPtr<CefBrowser> m_browser = nullptr;
        CefString m_objectName = "";

    public:
        CEFFunctionHandler(CefRefPtr<CefBrowser> a_browser, CefString a_objectName);

        // CefV8Handler
        bool Execute(const CefString& name,
                     CefRefPtr<CefV8Value> object,
                     const CefV8ValueList& arguments,
                     CefRefPtr<CefV8Value>& retval,
                     CefString& exception) override;
    };
}
