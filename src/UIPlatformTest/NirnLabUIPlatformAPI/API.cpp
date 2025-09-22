#include "API.h"

HMODULE g_nirnLabUILibHandle = nullptr;
std::mutex g_nirnLabUILibMutex;

HMODULE GetOrLoadNirnLabUILib()
{
    std::lock_guard locker(g_nirnLabUILibMutex);
    if (g_nirnLabUILibHandle != nullptr)
    {
        return g_nirnLabUILibHandle;
    }

    g_nirnLabUILibHandle = LoadLibrary(NL::UI::LibVersion::PROJECT_NAME);
    if (g_nirnLabUILibHandle == nullptr)
    {
        const auto errCode = GetLastError();
        switch (errCode)
        {
        case 126:
            throw std::runtime_error(std::format("{}.dll not found", NL::UI::LibVersion::PROJECT_NAME));
            break;
        default:
            throw std::runtime_error(std::format("Error while loading {}.dll, code: {}", NL::UI::LibVersion::PROJECT_NAME, errCode));
            break;
        }
    }

    return g_nirnLabUILibHandle;
}

namespace NL::UI
{
    ResponseVersionMessage GetUIPlatformAPIVersion()
    {
        const auto libHandle = GetOrLoadNirnLabUILib();
        const auto funcPtr = reinterpret_cast<decltype(&GetUIPlatformAPIVersion)>(GetProcAddress(libHandle, NameOf(GetUIPlatformAPIVersion)));
        return funcPtr();
    }

    bool CreateOrGetUIPlatformAPI(IUIPlatformAPI** a_outApi, NL::UI::Settings* a_settings)
    {
        const auto libHandle = GetOrLoadNirnLabUILib();
        const auto funcPtr = reinterpret_cast<decltype(&CreateOrGetUIPlatformAPI)>(GetProcAddress(libHandle, NameOf(CreateOrGetUIPlatformAPI)));
        return funcPtr(a_outApi, a_settings);
    }

    bool CreateOrGetUIPlatformAPIWithVersionCheck(NL::UI::IUIPlatformAPI** a_outApi,
                                                  NL::UI::Settings* a_settings,
                                                  std::uint32_t a_requestLibVersion,
                                                  const char* a_requestLibName)
    {
        const auto libHandle = GetOrLoadNirnLabUILib();
        const auto funcPtr = reinterpret_cast<decltype(&CreateOrGetUIPlatformAPIWithVersionCheck)>(GetProcAddress(libHandle, NameOf(CreateOrGetUIPlatformAPIWithVersionCheck)));
        return funcPtr(a_outApi, a_settings, a_requestLibVersion, a_requestLibName);
    }
}
