#include "CEFFunctionHandler.h"

namespace NL::JS
{
    CEFFunctionHandler::CEFFunctionHandler(CefRefPtr<CefBrowser> a_browser, CefString a_objectName)
    {
        if (a_browser == nullptr)
        {
            spdlog::error("{}: browser is nullptr", NameOf(CEFFunctionHandler));
        }

        m_browser = a_browser;
        m_objectName = a_objectName;
    }

    bool CEFFunctionHandler::Execute(const CefString& name,
                                     CefRefPtr<CefV8Value> object,
                                     const CefV8ValueList& arguments,
                                     CefRefPtr<CefV8Value>& retval,
                                     CefString& exception)
    {
        auto message = CefProcessMessage::Create(IPC_JS_FUNCTION_CALL_EVENT);
        m_browser->GetMainFrame()->SendProcessMessage(PID_BROWSER, message);

        return true;
    }
}
