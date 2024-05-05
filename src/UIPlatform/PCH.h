#pragma once

#ifndef NL_UI_REL_PATH
    #define NL_UI_REL_PATH ""
#endif // !NL_UI_REL_PATH

/* disable headers in Windows.h */
#define WIN32_LEAN_AND_MEAN
#define NOGDI

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

#define DLLEXPORT __declspec(dllexport)

#include "Version.h"
