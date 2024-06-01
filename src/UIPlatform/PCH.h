#pragma once

#define NameOf(name) #name
#define ThrowIfNullptr(type, arg) \
    if (arg == nullptr)           \
        throw std::runtime_error(fmt::format("{} has null {}", typeid(type).name(), NameOf(arg)));

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

// Win
#include <Windows.h>
#include <d3d11.h>

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

// sigslots (https://github.com/palacaze/sigslot)
#include "sigslot/signal.hpp"

#define DLLEXPORT __declspec(dllexport)
inline constexpr int MOUSE_WHEEL_DELTA = 120;
inline constexpr float KEY_FIRST_CHAR_DELAY = 0.45f;
inline constexpr float KEY_CHAR_REPEAT_DELAY = 0.06f;

#include "Version.h"
#include "Utils/PathUtils.h"
#include "Utils/BSTArrayExtension.h"
