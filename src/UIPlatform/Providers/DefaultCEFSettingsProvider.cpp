#include "DefaultCEFSettingsProvider.h"

namespace NL::Providers
{
    CefSettings DefaultCEFSettingsProvider::GetCefSettings()
    {
        CefSettings settings{};
        CefSettingsTraits::init(&settings);
        CefSettingsTraits::clear(&settings);

        settings.no_sandbox = true;
        settings.multi_threaded_message_loop = true;
        settings.windowless_rendering_enabled = true;
        // CefString(&settings.user_agent).FromString(L"NirnLabUIPlatform");
        // To open devtools use "chrome://inspect"
        settings.remote_debugging_port = 9009;

#ifdef _DEBUG
        settings.log_severity = LOGSEVERITY_VERBOSE;
#else
        settings.log_severity = LOGSEVERITY_INFO;
#endif

        const auto currentPath = std::filesystem::current_path();
        const auto currentPathHash = std::hash<std::string>{}(std::format("{}_{}.{}.{}", currentPath.string(), CEF_VERSION_MAJOR, CEF_VERSION_MINOR, CEF_VERSION_PATCH));
        const auto cefCachePath = NL::Utils::GetTempAppDataPath() / (L"CEF" + std::to_wstring(currentPathHash));
        const auto cefLogFilePath = cefCachePath / L"cef.log";
        const auto cefFilesRoot = currentPath / NL_UI_REL_PATH;
        const auto subprocPath = cefFilesRoot / NL_UI_SUBPROC_NAME;

        CefString(&settings.log_file).FromWString(cefLogFilePath.wstring());
        CefString(&settings.cache_path).FromWString(cefCachePath.wstring());
        CefString(&settings.browser_subprocess_path).FromWString(subprocPath.wstring());
        CefString(&settings.framework_dir_path).FromWString(cefFilesRoot);
        CefString(&settings.resources_dir_path).FromWString(cefFilesRoot);
        CefString(&settings.locales_dir_path).FromWString(cefFilesRoot / L"locales");

        return settings;
    }

    CefBrowserSettings DefaultCEFSettingsProvider::GetCefBrowserSettings()
    {
        CefBrowserSettings browserSettings{};
        CefBrowserSettingsTraits::init(&browserSettings);
        CefBrowserSettingsTraits::clear(&browserSettings);

        browserSettings.windowless_frame_rate = 60;
        browserSettings.background_color = 0x00;

        return browserSettings;
    }

    CefBrowserSettings DefaultCEFSettingsProvider::MergeAndGetCefBrowserSettings(NL::UI::BrowserSettings* a_settings)
    {
        return GetCefBrowserSettings();
    }

    CefWindowInfo DefaultCEFSettingsProvider::GetCefWindowInfo()
    {
        CefWindowInfo info{};
        CefWindowInfoTraits::init(&info);
        CefWindowInfoTraits::clear(&info);

        info.SetAsWindowless(nullptr);
        info.windowless_rendering_enabled = true;
        info.external_begin_frame_enabled = false;
        info.shared_texture_enabled = true;

        return info;
    }
}
