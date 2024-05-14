#include "MultiLayerRenderer.h"

namespace NL::Render
{
    bool MultiLayerRenderer::AddLayer(std::shared_ptr<IRenderLayer> a_layer)
    {
        std::lock_guard<std::mutex> lock(m_layersVectorMutex);
        a_layer->Init(m_renderData);
        m_layers.push_back(a_layer);

        return true;
    }

    void MultiLayerRenderer::ClearLayers()
    {
        std::lock_guard<std::mutex> lock(m_layersVectorMutex);
        m_layers.clear();
    }

    void MultiLayerRenderer::Draw()
    {
        std::lock_guard<std::mutex> lock(m_layersVectorMutex);
        if (m_layers.empty())
        {
            return;
        }

        m_renderData->spriteBatch->Begin(::DirectX::SpriteSortMode_Deferred, m_renderData->commonStates->NonPremultiplied());
        for (const auto layer : m_layers)
        {
            layer->Draw();
        }
        m_renderData->spriteBatch->End();
    }
}
