#pragma once

#include "PCH.h"
#include "IRenderer.h"

namespace NL::Render
{
    class MultiLayerRenderer final : public IRenderer
    {
      private:
        std::mutex layersVectorMutex;
        std::vector<std::shared_ptr<IRenderLayer>> layers;

      public:
        bool AddLayer(std::shared_ptr<IRenderLayer> a_layer);

        // IRenderer
        void Draw() override;
    };
}
