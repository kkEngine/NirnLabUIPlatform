#pragma once

#include "PCH.h"
#include "Utils/PathUtils.h"
#include "ICEFSettingsProvider.h"

namespace NL::Providers
{
    class DefaultCEFSettingsProvider : public ICEFSettingsProvider
    {
    public:
        virtual ~DefaultCEFSettingsProvider() override = default;

        virtual NL::UI::Settings GetGlobalSettings() override;
        virtual CefSettings GetCefSettings() override;
        virtual CefBrowserSettings GetCefBrowserSettings() override;
        virtual CefBrowserSettings MergeAndGetCefBrowserSettings(NL::UI::BrowserSettings* a_settings) override;
        virtual CefWindowInfo GetCefWindowInfo() override;
    };
}
