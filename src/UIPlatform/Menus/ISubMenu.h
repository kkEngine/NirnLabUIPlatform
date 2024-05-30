#pragma once

#include "PCH.h"
#include "Render/IRenderLayer.h"

namespace NL::Menus
{
    class ISubMenu : public NL::Render::IRenderLayer,
                     public RE::MenuEventHandler
    {
      public:
        virtual ~ISubMenu() override = default;
    };
}
