#pragma once

#include "Version.h"
#include "JSTypes.h"
#include "IBrowser.h"

namespace NL::UI
{
    enum class APIMessageType : std::uint32_t
    {
        kPreload = 2048,

    };

    struct PreloadMessage
    {
        std::uint32_t libVersion = LibVersion::AS_INT;
        std::uint32_t apiVersion = APIVersion::AS_INT;
        bool stopLoad = false;
    };
}
