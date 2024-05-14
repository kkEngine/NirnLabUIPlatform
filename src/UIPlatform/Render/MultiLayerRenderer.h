#pragma once

#include "PCH.h"
#include "IRenderer.h"

namespace NL::Render
{
    class MultiLayerRenderer final : public IRenderer
    {
      private:
        std::mutex m_layersVectorMutex;
        std::vector<std::shared_ptr<IRenderLayer>> m_layers;

      public:
        bool AddLayer(std::shared_ptr<IRenderLayer> a_layer);
        void ClearLayers();

        // IRenderer
        void Draw() override;
    };
}
