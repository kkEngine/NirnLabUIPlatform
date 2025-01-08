#pragma once

#include "PCH.h"
#include "NirnLabUIPlatformAPI/Settings.h"

namespace NL::Providers
{
    class ICEFSettingsProvider
    {
    public:
        virtual ~ICEFSettingsProvider() = default;
        virtual CefSettings GetCefSettings() = 0;
        virtual CefBrowserSettings GetCefBrowserSettings() = 0;
        virtual CefBrowserSettings MergeAndGetCefBrowserSettings(NL::UI::BrowserSettings* a_settings) = 0;
        virtual CefWindowInfo GetCefWindowInfo() = 0;
    };
}
