#include "CEFCopyRenderLayer.h"

namespace NL::Render
{
    std::shared_ptr<CEFCopyRenderLayer> CEFCopyRenderLayer::make_shared()
    {
        const auto cefRender = new CEFCopyRenderLayer();
        cefRender->AddRef();
        return std::shared_ptr<CEFCopyRenderLayer>(cefRender, CEFCopyRenderLayer::release_shared);
    }

    void CEFCopyRenderLayer::release_shared(CEFCopyRenderLayer* a_render)
    {
        a_render->Release();
    }

    void CEFCopyRenderLayer::Init(RenderData* a_renderData)
    {
        IRenderLayer::Init(a_renderData);

        const auto hr = m_renderData->device->QueryInterface(IID_PPV_ARGS(&m_device1));
        if (FAILED(hr))
        {
            spdlog::error("{}: failed QueryInterface(), code {:X}", NameOf(CEFCopyRenderLayer), hr);
        }

        D3D11_TEXTURE2D_DESC textDesc;
        ZeroMemory(&textDesc, sizeof(D3D11_TEXTURE2D_DESC));
        textDesc.Width = m_renderData->width;
        textDesc.Height = m_renderData->height;
        textDesc.MipLevels = 1;
        textDesc.ArraySize = 1;
        textDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        textDesc.SampleDesc.Count = 1;
        textDesc.SampleDesc.Quality = 0;
        textDesc.Usage = D3D11_USAGE_DYNAMIC;
        textDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        textDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        textDesc.MiscFlags = 0;

        auto hResult = m_renderData->device->CreateTexture2D(&textDesc, nullptr, m_cefTexture.ReleaseAndGetAddressOf());
        if (FAILED(hResult))
        {
            spdlog::error("{}: failed CreateTexture2D(), code {:X}", NameOf(CEFCopyRenderLayer), hResult);
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC sharedResourceViewDesc = {};
        sharedResourceViewDesc.Format = textDesc.Format;
        sharedResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        sharedResourceViewDesc.Texture2D.MostDetailedMip = 0;
        sharedResourceViewDesc.Texture2D.MipLevels = 1;

        hResult = m_renderData->device->CreateShaderResourceView(m_cefTexture.Get(), &sharedResourceViewDesc, m_cefSRV.ReleaseAndGetAddressOf());
        if (FAILED(hResult))
        {
            spdlog::error("{}: failed CreateShaderResourceView(), code {:X}", NameOf(CEFCopyRenderLayer), hResult);
        }

        hResult = m_renderData->device->CreateDeferredContext(0, m_deferredContext.ReleaseAndGetAddressOf());
        if (FAILED(hResult))
        {
            spdlog::error("{}: failed CreateDeferredContext(), code {:X}", NameOf(CEFCopyRenderLayer), hResult);
        }
    }

    const inline ::DirectX::SimpleMath::Vector2 _Cef_Menu_Draw_Vector = {0.f, 0.f};
    void CEFCopyRenderLayer::Draw()
    {
        if (m_isVisible && m_cefSRV != nullptr)
        {
            Microsoft::WRL::ComPtr<ID3D11CommandList> commandList;
            const auto result = m_deferredContext->FinishCommandList(FALSE, &commandList);
            if (result == S_OK && commandList)
            {
                m_renderData->deviceContext->ExecuteCommandList(commandList.Get(), TRUE);
            }
            else
            {
                spdlog::error("{}: failed FinishCommandList(), code {:X}", NameOf(CEFCopyRenderLayer), result);
            }

            m_renderData->spriteBatch->Draw(
                m_cefSRV.Get(),
                _Cef_Menu_Draw_Vector,
                nullptr,
                ::DirectX::Colors::White,
                0.f);
        }
    }

    void CEFCopyRenderLayer::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
    {
        rect = m_renderData ? CefRect(0, 0, m_renderData->width, m_renderData->height) : CefRect(0, 0, 800, 600);
    }

    void CEFCopyRenderLayer::OnPaint(
        CefRefPtr<CefBrowser> browser,
        PaintElementType type,
        const RectList& dirtyRects,
        const void* buffer,
        int width,
        int height)
    {
        spdlog::error("CEFCopyRenderLayer::OnPaint called");
    }

    void CEFCopyRenderLayer::OnAcceleratedPaint(
        CefRefPtr<CefBrowser> browser,
        PaintElementType type,
        const RectList& dirtyRects,
        const CefAcceleratedPaintInfo& info)
    {
        if (type == PET_POPUP ||
            m_renderData == nullptr ||
            m_device1 == nullptr)
        {
            return;
        }

        ID3D11Texture2D* tex = nullptr;
        auto hr = m_device1->OpenSharedResource1(info.shared_texture_handle, IID_PPV_ARGS(&tex));
        if (FAILED(hr))
        {
            _com_error err(hr);
            LPCTSTR errMsg = err.ErrorMessage();
            spdlog::error("OpenSharedResource1: unexpected HRESULT {:#X}: {}", static_cast<unsigned long>(hr), errMsg);
            return;
        }

        m_renderData->drawLock.Lock();

        m_deferredContext->FinishCommandList(false, nullptr);
        m_deferredContext->CopyResource(m_cefTexture.Get(), tex);

        m_renderData->drawLock.Unlock();

        tex->Release();
    }
}
