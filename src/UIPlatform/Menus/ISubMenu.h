#pragma once

#include "PCH.h"
#include "Render/IRenderLayer.h"

namespace NL::Menus
{
    enum class SubMenuType : std::uint16_t
    {
        CEFMenu = 0,

        Total,
    };

    class ISubMenu : public NL::Render::IRenderLayer,
                     public RE::MenuEventHandler
    {
      public:
        virtual ~ISubMenu() override = default;
        virtual SubMenuType GetMenuType() = 0;
    };
}
