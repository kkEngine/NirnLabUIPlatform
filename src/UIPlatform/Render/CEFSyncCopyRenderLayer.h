#pragma once

#include "PCH.h"
#include "IRenderLayer.h"
#include "Hooks/ShutdownHook.hpp"

namespace NL::Render
{
    class CEFSyncCopyRenderLayer : public IRenderLayer,
                                   public CefRenderHandler
    {
        IMPLEMENT_REFCOUNTING(CEFSyncCopyRenderLayer);

    protected:
        struct AtomicFlagGuard
        {
            std::atomic_flag& m_flag;

            AtomicFlagGuard(std::atomic_flag& a_flag);
            ~AtomicFlagGuard();
        };

        Microsoft::WRL::ComPtr<ID3D11Device1> m_device1 = nullptr;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_cefTexture = nullptr;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cefSRV = nullptr;
        const CefAcceleratedPaintInfo* m_acceleratedPaintInfo = nullptr;
        std::atomic_flag m_acceleratedPaintReady = ATOMIC_FLAG_INIT;

        void CopySharedTexure();

    public:
        virtual ~CEFSyncCopyRenderLayer() override = default;

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
