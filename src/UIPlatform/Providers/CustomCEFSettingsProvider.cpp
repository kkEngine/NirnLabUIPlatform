#include "CustomCEFSettingsProvider.h"

namespace NL::Providers
{
    CustomCEFSettingsProvider::CustomCEFSettingsProvider(NL::UI::Settings a_settings)
        : m_settings(a_settings)
    {
    }

    CefSettings CustomCEFSettingsProvider::GetCefSettings()
    {
        auto settings = m_defaultSettings->GetCefSettings();
        settings.remote_debugging_port = m_settings.remoteDebuggingPort;

        return settings;
    }

    CefBrowserSettings CustomCEFSettingsProvider::GetCefBrowserSettings()
    {
        return m_defaultSettings->GetCefBrowserSettings();
    }

    CefBrowserSettings CustomCEFSettingsProvider::MergeAndGetCefBrowserSettings(NL::UI::BrowserSettings* a_settings)
    {
        auto browserSettings = m_defaultSettings->MergeAndGetCefBrowserSettings(a_settings);
        browserSettings.windowless_frame_rate = a_settings->frameRate;

        return browserSettings;
    }

    CefWindowInfo CustomCEFSettingsProvider::GetCefWindowInfo()
    {
        return m_defaultSettings->GetCefWindowInfo();
    }
}
