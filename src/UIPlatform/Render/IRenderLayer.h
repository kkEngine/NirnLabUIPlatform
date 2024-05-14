#pragma once

#include "RenderData.h"

namespace NL::Render
{
    class IRenderLayer
    {
      protected:
        bool m_isVisible = true;
        RenderData* m_renderData = nullptr;

      public:
        virtual ~IRenderLayer() = default;

        virtual void Init(RenderData* a_renderData)
        {
            ThrowIfNullptr(IRenderLayer, a_renderData);
            m_renderData = a_renderData;
        };

        virtual void SetVisible(bool a_visible)
        {
            m_isVisible = a_visible;
        }

        virtual bool GetVisible()
        {
            return m_isVisible;
        }

        virtual void Draw(){};
    };
}
