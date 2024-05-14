#pragma once

#include <filesystem>
#include <KnownFolders.h>
#include <shlobj.h>
#include <iostream>

#include "Version.h"

namespace Utils
{
    static inline std::filesystem::path GetTempAppDataPath()
    {
        auto appPath = std::filesystem::temp_directory_path() / Version::PROJECT_NAME;
        CreateDirectoryW(appPath.wstring().c_str(), 0);
        return appPath;
    }

    static inline std::filesystem::path GetLocalAppDataPath()
    {
        PWSTR pwStr;
        std::filesystem::path fsPath;

        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &pwStr)))
        {
            fsPath = std::filesystem::path(pwStr) / Version::PROJECT_NAME;
        }
        CoTaskMemFree(pwStr);
        return fsPath;
    }

    static inline std::filesystem::path GetPathToMyDocuments()
    {
        PWSTR pwStr;
        std::filesystem::path fsPath;

        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pwStr)))
        {
            fsPath = std::filesystem::path(pwStr);
        }
        CoTaskMemFree(pwStr);
        return fsPath;
    }

    static inline std::filesystem::path GetPathToSSESaves()
    {
        return GetPathToMyDocuments() / "My Games" / "Skyrim Special Edition" / "Saves";
    }

    static inline void SetEnvPath(std::filesystem::path a_path)
    {
        if (!std::filesystem::exists(a_path))
            throw std::logic_error("Directory does not exist: " + a_path.string());
        if (!std::filesystem::is_directory(a_path))
            throw std::logic_error("Path is not a directory: " + a_path.string());
        if (!a_path.is_absolute())
            throw std::logic_error("An absolute path expected: " + a_path.string());

        std::vector<wchar_t> path;
        path.resize(GetEnvironmentVariableW(L"PATH", nullptr, 0));
        GetEnvironmentVariableW(L"PATH", &path[0], static_cast<DWORD>(path.size()));

        std::wstring newPath = path.data();
        newPath += L';';
        newPath += a_path.wstring();

        if (!SetEnvironmentVariableW(L"PATH", newPath.data()))
        {
            throw std::runtime_error("Failed to execute \"SetEnvironmentVariableW\": " + std::to_string(GetLastError()));
        }
    }
}
