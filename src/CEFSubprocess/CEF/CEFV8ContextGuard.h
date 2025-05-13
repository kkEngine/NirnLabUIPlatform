#pragma once

#include "PCH.h"

namespace NL::CEF
{
    class CEFV8ContextGuard final
    {
    private:
        CefRefPtr<CefV8Context> m_context = nullptr;

    public:
        CEFV8ContextGuard(CefRefPtr<CefV8Context> a_context)
        {
            if (a_context->Enter())
            {
                m_context = a_context;
            }
        }

        ~CEFV8ContextGuard()
        {
            if (m_context != nullptr)
            {
                m_context->Exit();
                m_context = nullptr;
            }
        }

        bool IsEntered()
        {
            return m_context != nullptr;
        }
    };
}
