#pragma once

#include "RenderData.h"

namespace NL::Render
{
    class IRenderLayer
    {
      protected:
        RenderData* renderData = nullptr;

      public:
        virtual ~IRenderLayer() = default;

        virtual void Init(RenderData* a_renderData)
        {
            ThrowIfNullptr(IRenderLayer, a_renderData);
            renderData = a_renderData;
        };

        virtual void Draw(){};
    };
}
