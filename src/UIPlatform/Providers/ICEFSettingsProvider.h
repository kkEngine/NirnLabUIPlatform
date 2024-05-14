#pragma once

#include "PCH.h"

namespace NL::Providers
{
    class ICEFSettingsProvider
    {
      public:
        virtual ~ICEFSettingsProvider() = default;
        virtual CefSettings GetCefSettings() = 0;
        virtual CefBrowserSettings GetCefBrowserSettings() = 0;
        virtual CefWindowInfo GetCefWindowInfo() = 0;
    };
}
