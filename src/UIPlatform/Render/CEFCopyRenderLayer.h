#pragma once

#include "PCH.h"
#include "IRenderLayer.h"
#include "Common/SpinLock.h"

namespace NL::Render
{
    class CEFCopyRenderLayer : public IRenderLayer,
                               public CefRenderHandler
    {
        IMPLEMENT_REFCOUNTING(CEFCopyRenderLayer);

    public:
        static std::shared_ptr<CEFCopyRenderLayer> make_shared();
        static void release_shared(CEFCopyRenderLayer* a_render);

    protected:
        HANDLE m_sharedTextureHandle = nullptr;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_cefTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cefSRV;
        Microsoft::WRL::ComPtr<ID3D11Device1> m_device1 = nullptr;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deferredContext;

    public:
        ~CEFCopyRenderLayer() override = default;

        // IRenderLayer
        void Init(RenderData* a_renderData) override;
        void Draw() override;

        // CefRenderHandler
        void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
        void OnPaint(
            CefRefPtr<CefBrowser> browser,
            PaintElementType type,
            const RectList& dirtyRects,
            const void* buffer,
            int width,
            int height) override;
        void OnAcceleratedPaint(
            CefRefPtr<CefBrowser> browser,
            PaintElementType type,
            const RectList& dirtyRects,
            const CefAcceleratedPaintInfo& info) override;
    };
}
