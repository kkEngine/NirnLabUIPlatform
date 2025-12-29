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
        if (!m_isVisible)
        {
            return;
        }

        if (m_renderData->deviceContext->GetType() == D3D11_DEVICE_CONTEXT_DEFERRED)
        {
            spdlog::error("D3D11_DEVICE_CONTEXT_DEFERRED_____________________");
            return;
        }

        if (!m_acceleratedPaintReady.test(std::memory_order_acquire))
        {
            if (m_cefSRV != nullptr)
            {
                m_renderData->spriteBatch->Draw(
                    m_cefSRV.Get(),
                    _Cef_Menu_Draw_Vector,
                    nullptr,
                    ::DirectX::Colors::White,
                    0.f);
            }

            return;
        }

        Microsoft::WRL::ComPtr<ID3D11Texture2D> sharedTexture = nullptr;
        auto hr = m_device1->OpenSharedResource1(m_acceleratedPaintInfo->shared_texture_handle, IID_PPV_ARGS(sharedTexture.ReleaseAndGetAddressOf()));
        if (FAILED(hr))
        {
            _com_error err(hr);
            LPCTSTR errMsg = err.ErrorMessage();
            spdlog::error("CEFSyncCopyRenderLayer::Draw() - failed to OpenSharedResource1(), unexpected HRESULT {:#X}: {}", static_cast<unsigned long>(hr), errMsg);
            m_acceleratedPaintReady.clear(std::memory_order_release);
            m_acceleratedPaintReady.notify_all();
            return;
        }

        if (m_cefSRV == nullptr)
        {
            D3D11_TEXTURE2D_DESC sharedTextureDesc = {};
            sharedTexture->GetDesc(&sharedTextureDesc);

            auto hResult = m_renderData->device->CreateTexture2D(&sharedTextureDesc, nullptr, m_cefTexture.ReleaseAndGetAddressOf());
            if (FAILED(hResult))
            {
                spdlog::error("CEFSyncCopyRenderLayer::Draw() - failed CreateTexture2D(), code {:X}", hResult);
                return;
            }

            D3D11_SHADER_RESOURCE_VIEW_DESC sharedResourceViewDesc = {};
            sharedResourceViewDesc.Format = sharedTextureDesc.Format;
            sharedResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            sharedResourceViewDesc.Texture2D.MostDetailedMip = 0;
            sharedResourceViewDesc.Texture2D.MipLevels = 1;

            hResult = m_renderData->device->CreateShaderResourceView(m_cefTexture.Get(), &sharedResourceViewDesc, m_cefSRV.ReleaseAndGetAddressOf());
            if (FAILED(hResult))
            {
                spdlog::error("CEFSyncCopyRenderLayer::Draw() - failed CreateShaderResourceView(), code {:X}", hResult);
                return;
            }

            spdlog::info("CEFSyncCopyRenderLayer::Draw() - texture created");
        }

        m_renderData->deviceContext->CopyResource(m_cefTexture.Get(), sharedTexture.Get());
        // The CopyResource call is asynchronous by default
        m_renderData->deviceContext->Flush();

        D3D11_QUERY_DESC QueryDesc = {};
        QueryDesc.Query = D3D11_QUERY_EVENT;
        QueryDesc.MiscFlags = 0;
        Microsoft::WRL::ComPtr<ID3D11Query> Query;
        if (HRESULT hr = m_device1->CreateQuery(&QueryDesc, &Query); FAILED(hr))
        {
            spdlog::error("CEFSyncCopyRenderLayer::Draw() - falied CreateQuery(), code {:X}", hr);
            return;
        }
        m_renderData->deviceContext->End(Query.Get());

        while (S_FALSE == m_renderData->deviceContext->GetData(Query.Get(), NULL, 0, 0))
        {
            // Optional: Sleep briefly to prevent a 100% CPU core usage busy-wait
            // Sleep(1);
        }

        //bool isDone = false;
        //hr = S_OK;
        //for (;;)
        //{
        //    hr = m_renderData->deviceContext->GetData(Query.Get(), &isDone, sizeof(isDone), 0);
        //    if (FAILED(hr))
        //    {
        //        _com_error err(hr);
        //        LPCTSTR errMsg = err.ErrorMessage();
        //        spdlog::error("CEFSyncCopyRenderLayer::Draw() - falied GetData(), code {:#X}: {}", static_cast<unsigned long>(hr), errMsg);
        //        m_acceleratedPaintReady.clear(std::memory_order_release);
        //        m_acceleratedPaintReady.notify_all();
        //        return;
        //    }
        //
        //    // We need to check for S_OK specifically as S_FALSE is also considered a success return code
        //    if (hr == S_OK && isDone)
        //    {
        //        break;
        //    }
        //}

        m_renderData->spriteBatch->Draw(
            m_cefSRV.Get(),
            _Cef_Menu_Draw_Vector,
            nullptr,
            ::DirectX::Colors::White,
            0.f);

        m_acceleratedPaintReady.clear(std::memory_order_release);
        m_acceleratedPaintReady.notify_all();
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
            m_device1 == nullptr ||
            info.shared_texture_handle == nullptr)
        {
            spdlog::error("CEFSyncCopyRenderLayer::OnAcceleratedPaint() - device or renderData is nullptr");
            return;
        }

        m_acceleratedPaintReady.test_and_set(std::memory_order_acquire);

        // while (m_acceleratedPaintReady.test_and_set(std::memory_order_acquire))
        //{
        //     m_acceleratedPaintReady.wait(true, std::memory_order_relaxed);
        // }

        m_acceleratedPaintInfo = &info;

        // m_acceleratedPaintReady.clear(std::memory_order_release);
        m_acceleratedPaintReady.wait(true, std::memory_order_acquire);

        spdlog::info("OnAcceleratedPaint");
    }
}
