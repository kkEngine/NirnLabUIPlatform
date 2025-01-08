#pragma once

#include "PCH.h"
#include "Utils/PathUtils.h"
#include "ICEFSettingsProvider.h"

namespace NL::Providers
{
    class DefaultCEFSettingsProvider : public ICEFSettingsProvider
    {
    public:
        ~DefaultCEFSettingsProvider() override = default;
        CefSettings GetCefSettings() override;
        CefBrowserSettings GetCefBrowserSettings() override;
        CefBrowserSettings MergeAndGetCefBrowserSettings(NL::UI::BrowserSettings* a_settings) override;
        CefWindowInfo GetCefWindowInfo() override;
    };
}
