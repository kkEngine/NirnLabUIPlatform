#pragma once

#include <cstdint>

namespace LibVersion
{
    inline constexpr std::uint32_t MAJOR = 0;
    inline constexpr std::uint32_t MINOR = 1;   
    inline constexpr auto PROJECT_NAME = "NirnLabUIPlatform";

    inline constexpr auto AS_STRING = "0.1";
    inline constexpr std::uint32_t AS_INT = (static_cast<std::uint32_t>(MAJOR * 100000 + MINOR));
}

namespace APIVersion
{
    inline constexpr std::uint32_t MAJOR = 0;
    inline constexpr std::uint32_t MINOR = 1;

    inline constexpr auto AS_STRING = "0.1";
    inline constexpr std::uint32_t AS_INT = (static_cast<std::uint32_t>(MAJOR * 100000 + MINOR));
}
