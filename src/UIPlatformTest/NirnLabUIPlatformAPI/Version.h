#pragma once

#include <cstdint>

namespace NL::UI::LibVersion
{
    inline constexpr std::uint32_t MAJOR = 2;
    inline constexpr std::uint32_t MINOR = 2;
    inline constexpr auto PROJECT_NAME = "NirnLabUIPlatform";

    inline constexpr auto MAJOR_MULT = 100000;
    inline constexpr auto AS_STRING = "2.2";
    inline constexpr std::uint32_t AS_INT = (static_cast<std::uint32_t>(MAJOR * MAJOR_MULT + MINOR));
	
    inline std::uint32_t GetMajorVersion(std::uint32_t a_version)
    {
        return a_version / MAJOR_MULT;
    }

    inline std::uint32_t GetMinorVersion(std::uint32_t a_version)
    {
        return a_version - GetMajorVersion(a_version) * MAJOR_MULT;
    }
}

namespace NL::UI::APIVersion
{
    inline constexpr std::uint32_t MAJOR = 2;
    inline constexpr std::uint32_t MINOR = 2;

    inline constexpr auto MAJOR_MULT = 100000;
    inline constexpr auto AS_STRING = "2.2";
    inline constexpr std::uint32_t AS_INT = (static_cast<std::uint32_t>(MAJOR * MAJOR_MULT + MINOR));
	
    inline std::uint32_t GetMajorVersion(std::uint32_t a_version)
    {
        return a_version / MAJOR_MULT;
    }

    inline std::uint32_t GetMinorVersion(std::uint32_t a_version)
    {
        return a_version - GetMajorVersion(a_version) * MAJOR_MULT;
    }
}
