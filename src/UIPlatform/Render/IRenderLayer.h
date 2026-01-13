#pragma once

#include "PCH.h"
#include "RenderData.h"

namespace NL::Render
{
    class IRenderLayer : public virtual CefBaseRefCounted
    {
    protected:
        bool m_isVisible = true;
        RenderData* m_renderData = nullptr;

    public:
        virtual ~IRenderLayer() = default;

        virtual const char* GetName()
        {
            return "Unknown";
        }

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

        virtual void Draw() {};
    };
}
