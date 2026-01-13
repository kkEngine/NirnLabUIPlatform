#pragma once

#include "PCH.h"
#include "ICEFSettingsProvider.h"
#include "DefaultCEFSettingsProvider.h"
#include "NirnLabUIPlatformAPI/Settings.h"

namespace NL::Providers
{
    class CustomCEFSettingsProvider : public ICEFSettingsProvider
    {
    protected:
        std::unique_ptr<DefaultCEFSettingsProvider> m_defaultSettings = std::make_unique<DefaultCEFSettingsProvider>();
        NL::UI::Settings m_settings;

    public:
        CustomCEFSettingsProvider(NL::UI::Settings a_settings);
        virtual ~CustomCEFSettingsProvider() override = default;

        virtual NL::UI::Settings GetGlobalSettings() override;
        virtual CefSettings GetCefSettings() override;
        virtual CefBrowserSettings GetCefBrowserSettings() override;
        virtual CefBrowserSettings MergeAndGetCefBrowserSettings(NL::UI::BrowserSettings* a_settings) override;
        virtual CefWindowInfo GetCefWindowInfo() override;
    };
}
