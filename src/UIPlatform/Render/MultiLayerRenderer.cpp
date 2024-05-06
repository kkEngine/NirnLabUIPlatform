#include "MultiLayerRenderer.h"

namespace NL::Render
{
    bool MultiLayerRenderer::AddLayer(std::shared_ptr<IRenderLayer> a_layer)
    {
        std::lock_guard<std::mutex> lock(layersVectorMutex);
        a_layer->Init(renderData);
        layers.push_back(a_layer);

        return true;
    }

    void MultiLayerRenderer::Draw()
    {
        std::lock_guard<std::mutex> lock(layersVectorMutex);
        if (layers.empty())
        {
            return;
        }

        renderData->spriteBatch->Begin(::DirectX::SpriteSortMode_Deferred, renderData->commonStates->NonPremultiplied());
        for (const auto layer : layers)
        {
            layer->Draw();
        }
        renderData->spriteBatch->End();
    }
}
