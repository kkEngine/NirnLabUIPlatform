#include "CEFSyncCopyRenderLayer.h"

namespace NL::Render
{
    std::shared_ptr<CEFSyncCopyRenderLayer> CEFSyncCopyRenderLayer::make_shared()
    {
        const auto cefRender = new CEFSyncCopyRenderLayer();
        cefRender->AddRef();
        return std::shared_ptr<CEFSyncCopyRenderLayer>(cefRender, CEFSyncCopyRenderLayer::release_shared);
    }

    void CEFSyncCopyRenderLayer::release_shared(CEFSyncCopyRenderLayer* a_render)
    {
        a_render->Release();
    }

    void CEFSyncCopyRenderLayer::CopySharedTexure()
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> sharedTexture = nullptr;
        auto hr = m_device1->OpenSharedResource1(m_acceleratedPaintInfo->shared_texture_handle, IID_PPV_ARGS(sharedTexture.ReleaseAndGetAddressOf()));
        FAST_CHECK_HRESULT_LOG_AND_RETURN(hr, "CEFSyncCopyRenderLayer::CopySharedTexure() - OpenSharedResource1()");

        if (m_cefSRV == nullptr)
        {
            D3D11_TEXTURE2D_DESC sharedTextureDesc = {};
            sharedTexture->GetDesc(&sharedTextureDesc);

            hr = m_renderData->device->CreateTexture2D(&sharedTextureDesc, nullptr, m_cefTexture.ReleaseAndGetAddressOf());
            FAST_CHECK_HRESULT_LOG_AND_RETURN(hr, "CEFSyncCopyRenderLayer::CopySharedTexure() - CreateTexture2D()");

            D3D11_SHADER_RESOURCE_VIEW_DESC sharedResourceViewDesc = {};
            sharedResourceViewDesc.Format = sharedTextureDesc.Format;
            sharedResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            sharedResourceViewDesc.Texture2D.MostDetailedMip = 0;
            sharedResourceViewDesc.Texture2D.MipLevels = 1;

            hr = m_renderData->device->CreateShaderResourceView(m_cefTexture.Get(), &sharedResourceViewDesc, m_cefSRV.ReleaseAndGetAddressOf());
            FAST_CHECK_HRESULT_LOG_AND_RETURN(hr, "CEFSyncCopyRenderLayer::CopySharedTexure() - CreateShaderResourceView()");

            spdlog::info("CEFSyncCopyRenderLayer: texture created");
        }

        spdlog::info("Dirty rect - {}:{}:{}:{}",
                     m_acceleratedPaintInfo->extra.capture_update_rect.x,
                     m_acceleratedPaintInfo->extra.capture_update_rect.y,
                     m_acceleratedPaintInfo->extra.capture_update_rect.width,
                     m_acceleratedPaintInfo->extra.capture_update_rect.height);

        D3D11_BOX dirtyRect{};
        dirtyRect.left = m_acceleratedPaintInfo->extra.capture_update_rect.x;
        dirtyRect.top = m_acceleratedPaintInfo->extra.capture_update_rect.y;
        dirtyRect.right = m_acceleratedPaintInfo->extra.capture_update_rect.x + m_acceleratedPaintInfo->extra.capture_update_rect.width;
        dirtyRect.bottom = m_acceleratedPaintInfo->extra.capture_update_rect.y + m_acceleratedPaintInfo->extra.capture_update_rect.height;
        dirtyRect.front = 0;
        dirtyRect.back = 1;

        m_renderData->deviceContext->CopySubresourceRegion(m_cefTexture.Get(),
                                                           0,
                                                           m_acceleratedPaintInfo->extra.capture_update_rect.x,
                                                           m_acceleratedPaintInfo->extra.capture_update_rect.y,
                                                           0,
                                                           sharedTexture.Get(),
                                                           0,
                                                           &dirtyRect);
        // The CopyResource call is asynchronous by default
        m_renderData->deviceContext->Flush();

        D3D11_QUERY_DESC queryDesc = {};
        queryDesc.Query = D3D11_QUERY_EVENT;
        queryDesc.MiscFlags = 0;

        Microsoft::WRL::ComPtr<ID3D11Query> query;
        hr = m_device1->CreateQuery(&queryDesc, &query);
        FAST_CHECK_HRESULT_LOG_AND_RETURN(hr, "CEFSyncCopyRenderLayer::CopySharedTexure() - CreateQuery()");

        m_renderData->deviceContext->End(query.Get());

        while (S_FALSE == m_renderData->deviceContext->GetData(query.Get(), NULL, 0, 0))
        {
        }
    }

    void CEFSyncCopyRenderLayer::Init(RenderData* a_renderData)
    {
        IRenderLayer::Init(a_renderData);

        const auto hr = m_renderData->device->QueryInterface(IID_PPV_ARGS(&m_device1));
        if (FAILED(hr))
        {
            spdlog::error("CEFSyncCopyRenderLayer::Init() - failed QueryInterface(), code {:X}", hr);
        }
    }

    const inline ::DirectX::SimpleMath::Vector2 _Cef_Menu_Draw_Vector = {0.f, 0.f};
    void CEFSyncCopyRenderLayer::Draw()
    {
        if (m_acceleratedPaintReady.test(std::memory_order_acquire))
        {
            CopySharedTexure();
            m_acceleratedPaintReady.clear(std::memory_order_release);
            m_acceleratedPaintReady.notify_all();
        }

        if (m_isVisible && m_cefSRV != nullptr)
        {
            m_renderData->spriteBatch->Draw(
                m_cefSRV.Get(),
                _Cef_Menu_Draw_Vector,
                nullptr,
                ::DirectX::Colors::White,
                0.f);
        }
    }

    void CEFSyncCopyRenderLayer::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
    {
        if (m_renderData)
        {
            rect.x = 0;
            rect.y = 0;
            rect.width = m_renderData->width;
            rect.height = m_renderData->height;
        }
        else
        {
            rect.x = 0;
            rect.y = 0;
            rect.width = 800;
            rect.height = 600;
        }
    }

    void CEFSyncCopyRenderLayer::OnPaint(CefRefPtr<CefBrowser> browser,
                                         PaintElementType type,
                                         const RectList& dirtyRects,
                                         const void* buffer,
                                         int width,
                                         int height)
    {
    }

    void CEFSyncCopyRenderLayer::OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                                                    PaintElementType type,
                                                    const RectList& dirtyRects,
                                                    const CefAcceleratedPaintInfo& info)
    {
        if (type == PaintElementType::PET_POPUP)
        {
            return;
        }

        if (m_renderData == nullptr ||
            m_device1 == nullptr)
        {
            spdlog::error("CEFSyncCopyRenderLayer::OnAcceleratedPaint() - device or renderData is nullptr");
            return;
        }

        m_acceleratedPaintInfo = &info;
        m_acceleratedPaintReady.test_and_set(std::memory_order_acquire);
        m_acceleratedPaintReady.wait(true, std::memory_order_acquire);
    }
}
