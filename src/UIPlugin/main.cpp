#define DLLEXPORT __declspec(dllexport)

/* disable headers in Windows.h */
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOMINMAX

// std
#include <string>
#include <filesystem>

using namespace std::literals;
using namespace std::string_literals;

// Fmt
#include "fmt/format.h"

// CommonLibSSE
#include <RE/Skyrim.h>
#include <REL/Relocation.h>
#include <SKSE/Impl/Stubs.h>
#include <SKSE/SKSE.h>

// Win
#include <Windows.h>

// this
#include "Version.h"

using EntryFunc = bool (*)(const SKSE::LoadInterface* a_skse);
using PreloadFunc = void (*)();

inline void ShowMessageBox(const char* a_msg)
{
    MessageBoxA(0, a_msg, "ERROR", MB_ICONERROR);
}

std::string GetLastErrorAsString()
{
    // Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
    {
        return std::string(); // No error message has been recorded
    }

    LPSTR messageBuffer = nullptr;

    // Ask Win32 to give us the string version of that message ID.
    // The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL,
                                 errorMessageID,
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                 (LPSTR)&messageBuffer,
                                 0,
                                 NULL);

    // Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    // Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    return message;
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

static inline std::filesystem::path GetUIRelPath()
{
    return std::filesystem::current_path() / L"" NL_UI_REL_PATH;
}

bool g_isEnvPathSet = false;
static inline void SetUIEnvPath()
{
    if (!g_isEnvPathSet)
    {
        SetEnvPath(GetUIRelPath());
        g_isEnvPathSet = true;
    }
}

HMODULE g_nirnLabUILib = nullptr;
static inline void LoadNirnLabUILib()
{
    if (g_nirnLabUILib == nullptr)
    {
        auto nirnLabUILib = LoadLibraryA(NL_UI_LIB_NAME);
        if (!nirnLabUILib)
        {
            std::string errDesc;
            const auto errCode = GetLastError();
            switch (errCode)
            {
            // Not found
            case 126:
                errDesc = NL_UI_LIB_NAME " not found";
                break;
            default:
                errDesc.append(fmt::format("Error code: {}, desc: \"{}\"", errCode, GetLastErrorAsString()));
                break;
            }

            ShowMessageBox(errDesc.data());
            throw std::runtime_error(errDesc);
        }
        else
        {
            g_nirnLabUILib = nirnLabUILib;
        }
    }
}

template<class TFunc>
static inline TFunc ExecLibFunc(const char* a_funcName)
{
    SetUIEnvPath();
    LoadNirnLabUILib();

    auto func = reinterpret_cast<TFunc>(GetProcAddress(g_nirnLabUILib, a_funcName));
    if (!func)
    {
        auto funcNotFoundMsg = fmt::format("{} \"{}\" function not found", NL_UI_LIB_NAME, a_funcName);
        ShowMessageBox(funcNotFoundMsg.data());
        throw std::runtime_error(funcNotFoundMsg);
    }

    return func;
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
    SKSE::PluginVersionData v{};
    v.pluginVersion = Version::AS_INT;
    v.PluginName(Version::PROJECT_NAME);
    v.AuthorName("kkEngine"sv);
    v.CompatibleVersions({SKSE::RUNTIME_SSE_1_6_640});
    v.UsesAddressLibrary(true);
    v.UsesStructsPost629(true);
    return v;
}();

extern "C" void DLLEXPORT APIENTRY Initialize()
{
    auto preload = ExecLibFunc<PreloadFunc>("Initialize");
    preload();
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
    try
    {
        auto entry = ExecLibFunc<EntryFunc>("Entry");
        return entry(a_skse);
    }
    catch (const std::exception& e)
    {
        ShowMessageBox(e.what());
        return false;
    }
}
