#pragma once

#define NameOf(name) #name

/* disable headers in Windows.h */
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOMINMAX

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <iomanip>
#include <map>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>
#include <filesystem>
#include <deque>
#include <type_traits>
#include <queue>

// spdlog
#include <spdlog/spdlog.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

using namespace std::literals;
using namespace std::string_literals;

// CEF
#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_client.h>
#include <include/cef_version.h>
#include <include/wrapper/cef_helpers.h>

#include "IPC.h"
