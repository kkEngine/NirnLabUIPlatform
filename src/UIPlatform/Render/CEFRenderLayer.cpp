#include "CEFRenderLayer.h"

namespace NL::Render
{
    std::shared_ptr<CEFRenderLayer> CEFRenderLayer::make_shared()
    {
        const auto cefRender = new CEFRenderLayer();
        cefRender->AddRef();
        return std::shared_ptr<CEFRenderLayer>(cefRender, CEFRenderLayer::release_shared);
    }

    void CEFRenderLayer::release_shared(CEFRenderLayer* a_render)
    {
        a_render->Release();
    }

    CEFRenderLayer::~CEFRenderLayer()
    {
        if (m_cefSRV != nullptr)
        {
            m_cefSRV->Release();
        }
        if (m_cefTexture != nullptr)
        {
            m_cefTexture->Release();
        }
    }

    void CEFRenderLayer::Init(RenderData* a_renderData)
    {
        IRenderLayer::Init(a_renderData);

        const auto hr = m_renderData->device->QueryInterface(IID_PPV_ARGS(&m_device1));
        if (FAILED(hr))
        {
            spdlog::error("{}: failed QueryInterface(), code {:X}", NameOf(CEFRenderLayer), hr);
        }
    }

    const ::DirectX::SimpleMath::Vector2 _Cef_Menu_Draw_Vector = {0.f, 0.f};
    void CEFRenderLayer::Draw()
    {
        if (m_isVisible && m_cefSRV != nullptr)
        {
            m_renderData->spriteBatch->Draw(
                m_cefSRV,
                _Cef_Menu_Draw_Vector,
                nullptr,
                ::DirectX::Colors::White,
                0.f);
        }
    }

    const char* CEFRenderLayer::GetName()
    {
        return "CEFRenderLayer[DEPRECATED]";
    }

    void CEFRenderLayer::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
    {
        rect = m_renderData ? CefRect(0, 0, m_renderData->width, m_renderData->height) : CefRect(0, 0, 800, 600);
    }

    void CEFRenderLayer::OnPaint(
        CefRefPtr<CefBrowser> browser,
        PaintElementType type,
        const RectList& dirtyRects,
        const void* buffer,
        int width,
        int height)
    {
    }

    void CEFRenderLayer::OnAcceleratedPaint(
        CefRefPtr<CefBrowser> browser,
        PaintElementType type,
        const RectList& dirtyRects,
        const CefAcceleratedPaintInfo& info)
    {
        if (
            type == PET_POPUP ||
            m_renderData == nullptr ||
            m_device1 == nullptr ||
            m_sharedTextureHandle == info.shared_texture_handle)
        {
            return;
        }

        ID3D11Texture2D* tex = nullptr;
        auto hr = m_device1->OpenSharedResource1(info.shared_texture_handle, IID_PPV_ARGS(&tex));
        if (FAILED(hr))
        {
            spdlog::error("{}: failed OpenSharedResource(), code {:X}", NameOf(CEFRenderLayer), hr);
            return;
        }

        D3D11_TEXTURE2D_DESC td;
        tex->GetDesc(&td);

        ID3D11ShaderResourceView* srv = nullptr;
        if (td.BindFlags & D3D11_BIND_SHADER_RESOURCE)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
            srv_desc.Format = td.Format;
            srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srv_desc.Texture2D.MostDetailedMip = 0;
            srv_desc.Texture2D.MipLevels = 1;

            hr = m_renderData->device->CreateShaderResourceView(tex, &srv_desc, &srv);
            if (FAILED(hr))
            {
                tex->Release();
                spdlog::error("{}: failed CreateShaderResourceView(), code {:X}", NameOf(CEFRenderLayer), hr);
                return;
            }
        }

        m_renderData->drawLock.Lock();

        std::swap(m_cefTexture, tex);
        std::swap(m_cefSRV, srv);

        m_renderData->drawLock.Unlock();

        if (srv != nullptr)
        {
            srv->Release();
        }
        if (tex != nullptr)
        {
            tex->Release();
        }
    }
}
