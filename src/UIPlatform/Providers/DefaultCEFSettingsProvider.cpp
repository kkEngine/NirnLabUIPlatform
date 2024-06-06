#include "DefaultCEFSettingsProvider.h"

namespace NL::Providers
{
    CefSettings DefaultCEFSettingsProvider::GetCefSettings()
    {
        CefSettings settings;
        settings.no_sandbox = true;
        settings.multi_threaded_message_loop = true;
        settings.windowless_rendering_enabled = true;
        // CefString(&settings.user_agent).FromString(L"NirnLabUIPlatform");
#ifdef _DEBUG
        settings.log_severity = LOGSEVERITY_VERBOSE;
        settings.remote_debugging_port = 9000;
#else
        settings.log_severity = LOGSEVERITY_INFO;
#endif

        const auto currentPath = std::filesystem::current_path();
        const auto cefAppDataPath = NL::Utils::GetLocalAppDataPath() / L"CEF";
        const auto cefLogFilePath = cefAppDataPath / L"cef.log";
        const auto cefFilesRoot = currentPath / NL_UI_REL_PATH;
        const auto subprocPath = cefFilesRoot / NL_UI_SUBPROC_NAME;

        CefString(&settings.log_file).FromWString(cefLogFilePath.wstring());
        CefString(&settings.cache_path).FromWString(cefAppDataPath.wstring());
        CefString(&settings.browser_subprocess_path).FromWString(subprocPath.wstring());
        CefString(&settings.framework_dir_path).FromWString(cefFilesRoot / "CEF");
        CefString(&settings.resources_dir_path).FromWString(cefFilesRoot / "CEF");
        CefString(&settings.locales_dir_path).FromWString(cefFilesRoot / "CEF" / "locales");

        return settings;
    }

    CefBrowserSettings DefaultCEFSettingsProvider::GetCefBrowserSettings()
    {
        CefBrowserSettings browserSettings{};
        browserSettings.windowless_frame_rate = 60;
        browserSettings.background_color = 0x00;

        return browserSettings;
    }

    CefWindowInfo DefaultCEFSettingsProvider::GetCefWindowInfo()
    {
        CefWindowInfo info{};
        info.SetAsWindowless(nullptr);
        info.windowless_rendering_enabled = true;
        info.external_begin_frame_enabled = false;
        info.shared_texture_enabled = true;

        return CefWindowInfo(info);
    }
}
