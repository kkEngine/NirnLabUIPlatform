#include "CEFSyncCopy2RenderLayer.h"

namespace NL::Render
{
    void CEFSyncCopy2RenderLayer::Init(RenderData* a_renderData)
    {
        IRenderLayer::Init(a_renderData);

        const auto hr = m_renderData->device->QueryInterface(IID_PPV_ARGS(&m_device1));
        if (FAILED(hr))
        {
            spdlog::error("CEFSyncCopy2RenderLayer::Init() - failed QueryInterface(), code {:X}", hr);
        }
    }

    void CEFSyncCopy2RenderLayer::Draw()
    {
        if (m_isVisible && m_cefSRV != nullptr)
        {
            m_renderData->spriteBatch->Draw(
                m_cefSRV.Get(),
                ::DirectX::SimpleMath::Vector2{0.f, 0.f},
                nullptr,
                ::DirectX::Colors::White,
                0.f);
        }
    }

    const char* CEFSyncCopy2RenderLayer::GetName()
    {
        return "SyncCopy2";
    }

    void CEFSyncCopy2RenderLayer::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
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
    void CEFSyncCopy2RenderLayer::OnPaint(CefRefPtr<CefBrowser> browser,
                                          PaintElementType type,
                                          const RectList& dirtyRects,
                                          const void* buffer,
                                          int width,
                                          int height)
    {
        if (!m_paintErrorPrint)
        {
            spdlog::error("{}: should not called", NameOf(CEFSyncCopy2RenderLayer::OnPaint));
            m_paintErrorPrint = true;
        }
    }

    void CEFSyncCopy2RenderLayer::OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,
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
            spdlog::error("{}: device or renderData is nullptr", NameOf(CEFSyncCopy2RenderLayer::OnAcceleratedPaint));
            m_needFullTextureCopy = true;
            return;
        }

        if (info.extra.has_capture_update_rect &&
            (info.extra.capture_update_rect.width == 0 ||
             info.extra.capture_update_rect.height == 0))
        {
            return;
        }

        RendererLockGuard locker{};

        Microsoft::WRL::ComPtr<ID3D11Texture2D> sharedTexture = nullptr;
        auto hr = m_device1->OpenSharedResource1(info.shared_texture_handle, IID_PPV_ARGS(sharedTexture.ReleaseAndGetAddressOf()));
        if (FAILED(hr))
        {
            spdlog::error("{}", CheckHresultMessage(hr, std::format("{}: OpenSharedResource1() error", NameOf(CEFSyncCopy2RenderLayer::OnAcceleratedPaint))));
            m_needFullTextureCopy = true;
            return;
        }

        if (m_cefSRV == nullptr)
        {
            D3D11_TEXTURE2D_DESC sharedTextureDesc = {};
            sharedTexture->GetDesc(&sharedTextureDesc);

            hr = m_renderData->device->CreateTexture2D(&sharedTextureDesc, nullptr, m_cefTexture.ReleaseAndGetAddressOf());
            FAST_CHECK_HRESULT_LOG_AND_RETURN(hr, std::format("{}: CreateTexture2D() error", NameOf(CEFSyncCopy2RenderLayer::OnAcceleratedPaint)));

            D3D11_SHADER_RESOURCE_VIEW_DESC sharedResourceViewDesc = {};
            sharedResourceViewDesc.Format = sharedTextureDesc.Format;
            sharedResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            sharedResourceViewDesc.Texture2D.MostDetailedMip = 0;
            sharedResourceViewDesc.Texture2D.MipLevels = 1;

            hr = m_renderData->device->CreateShaderResourceView(m_cefTexture.Get(), &sharedResourceViewDesc, m_cefSRV.ReleaseAndGetAddressOf());
            FAST_CHECK_HRESULT_LOG_AND_RETURN(hr, std::format("{}: CreateShaderResourceView() error", NameOf(CEFSyncCopy2RenderLayer::OnAcceleratedPaint)));

            spdlog::info("{}: texture created", NameOf(CEFSyncCopy2RenderLayer::OnAcceleratedPaint));
        }

        if (m_copyDoneQuery == nullptr)
        {
            D3D11_QUERY_DESC queryDesc = {};
            queryDesc.Query = D3D11_QUERY_EVENT;
            queryDesc.MiscFlags = 0;

            hr = m_device1->CreateQuery(&queryDesc, m_copyDoneQuery.ReleaseAndGetAddressOf());
            if (FAILED(hr))
            {
                spdlog::error("{}", CheckHresultMessage(hr, std::format("{}: CreateQuery() error", NameOf(CEFSyncCopy2RenderLayer::OnAcceleratedPaint))));
                m_needFullTextureCopy = true;
                return;
            }
        }

        if (m_needFullTextureCopy)
        {
            m_renderData->deviceContext->CopyResource(m_cefTexture.Get(), sharedTexture.Get());
        }
        else
        {
            D3D11_BOX dirtyRect;
            dirtyRect.left = info.extra.capture_update_rect.x;
            dirtyRect.top = info.extra.capture_update_rect.y;
            dirtyRect.right = info.extra.capture_update_rect.x + info.extra.capture_update_rect.width;
            dirtyRect.bottom = info.extra.capture_update_rect.y + info.extra.capture_update_rect.height;
            dirtyRect.front = 0;
            dirtyRect.back = 1;

            m_renderData->deviceContext->CopySubresourceRegion(m_cefTexture.Get(),
                                                               0,
                                                               info.extra.capture_update_rect.x,
                                                               info.extra.capture_update_rect.y,
                                                               0,
                                                               sharedTexture.Get(),
                                                               0,
                                                               &dirtyRect);
        }

        m_renderData->deviceContext->End(m_copyDoneQuery.Get());
        m_renderData->deviceContext->Flush();

        while (S_FALSE == m_renderData->deviceContext->GetData(m_copyDoneQuery.Get(), NULL, 0, D3D11_ASYNC_GETDATA_DONOTFLUSH))
        {
        }
        m_needFullTextureCopy = false;
    }
}
