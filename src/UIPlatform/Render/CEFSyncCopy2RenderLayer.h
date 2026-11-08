#pragma once

#include "PCH.h"
#include "IRenderLayer.h"
#include "Hooks/ShutdownHook.hpp"

namespace NL::Render
{
    class CEFSyncCopy2RenderLayer : public IRenderLayer,
                                    public CefRenderHandler
    {
        IMPLEMENT_REFCOUNTING(CEFSyncCopy2RenderLayer);

    protected:
        struct RendererLockGuard
        {
        public:
            RendererLockGuard()
            {
                RE::BSGraphics::Renderer::GetSingleton()->Lock();
            }
            ~RendererLockGuard()
            {
                RE::BSGraphics::Renderer::GetSingleton()->Unlock();
            }
        };

        Microsoft::WRL::ComPtr<ID3D11Device1> m_device1 = nullptr;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_cefTexture = nullptr;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cefSRV = nullptr;
        Microsoft::WRL::ComPtr<ID3D11Query> m_copyDoneQuery = nullptr;

        bool m_paintErrorPrint = false;
        bool m_needFullTextureCopy = true;

    public:
        virtual ~CEFSyncCopy2RenderLayer() override = default;

        // IRenderLayer
        virtual void Init(RenderData* a_renderData) override;
        virtual void Draw() override;
        virtual const char* GetName() override;

        // CefRenderHandler
        virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
        virtual void OnPaint(CefRefPtr<CefBrowser> browser,
                             PaintElementType type,
                             const RectList& dirtyRects,
                             const void* buffer,
                             int width,
                             int height) override;
        virtual void OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                                        PaintElementType type,
                                        const RectList& dirtyRects,
                                        const CefAcceleratedPaintInfo& info) override;
    };
}
