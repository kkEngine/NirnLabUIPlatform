#define DLLEXPORT __declspec(dllexport)
#define PLUGIN_NAME "NirnLabUIPlatformTest"

// std
#include <string>
#include <filesystem>

using namespace std::literals;
using namespace std::string_literals;

// CommonLibSSE
#include <RE/Skyrim.h>
#include <REL/Relocation.h>
#include <SKSE/Impl/Stubs.h>
#include <SKSE/SKSE.h>

namespace logger = SKSE::log;
using SKSE::stl::adjust_pointer;
using SKSE::stl::not_null;
using SKSE::stl::unrestricted_cast;

namespace stl
{
    using SKSE::stl::report_and_fail;
    using std::span;
}

// spdlog
#include <spdlog/spdlog.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

// API
#include "NirnLabUIPlatformAPI/API.h"
