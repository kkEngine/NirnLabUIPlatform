#pragma once

#ifndef NL_UI_SUBPROC_NAME
    #define NL_UI_SUBPROC_NAME ""
#endif // !NL_UI_SUBPROC_NAME

/* disable headers in Windows.h */
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOMINMAX

// CommonLibSSE
#include <RE/Skyrim.h>
#include <REL/Relocation.h>
#include <SKSE/Impl/Stubs.h>
#include <SKSE/SKSE.h>

// std
#include <wrl.h>
#include <cmath>
#include <sstream>
#include <shlobj.h>
#include <cassert>
#include <filesystem>
#include <span>
#include <unordered_set>

// Win
#include <Windows.h>
#include <d3d11.h>
#include <d3d11_3.h>

static std::string GetLastErrorAsString()
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

// spdlog
#include <spdlog/spdlog.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

using namespace std::literals;
using namespace std::string_literals;

namespace logger = SKSE::log;
using SKSE::stl::adjust_pointer;
using SKSE::stl::not_null;
using SKSE::stl::unrestricted_cast;

namespace stl
{
    using SKSE::stl::report_and_fail;
    using std::span;
}

// CEF
#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_client.h>
#include <include/cef_version.h>

// nlohmann-json (https://github.com/nlohmann/json)
#include "nlohmann/json.hpp"

// sigslots (https://github.com/palacaze/sigslot)
#include "sigslot/signal.hpp"

#define DLLEXPORT __declspec(dllexport)
inline constexpr int MOUSE_WHEEL_DELTA = 120;
inline constexpr float KEY_FIRST_CHAR_DELAY = 0.45f;
inline constexpr float KEY_CHAR_REPEAT_DELAY = 0.06f;

#include "IPC.h"

#include "NirnLabUIPlatformAPI/API.h"
#include "Utils/BSTArrayExtension.h"
#include "Utils/CheckHresult.h"
#include "Utils/PathUtils.h"

// utils
#define NameOf(name) #name
#define ThrowIfNullptr(type, arg)                                                                      \
    do                                                                                                 \
    {                                                                                                  \
        if (arg == nullptr)                                                                            \
            throw std::runtime_error(fmt::format("{} has null {}", typeid(type).name(), NameOf(arg))); \
    } while (0)
