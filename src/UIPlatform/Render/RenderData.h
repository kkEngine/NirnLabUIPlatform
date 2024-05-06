#pragma once

#include <directxtk/CommonStates.h>
#include <directxtk/SimpleMath.h>
#include <directxtk/SpriteBatch.h>

namespace NL::Render
{
    struct RenderData
    {
        std::shared_ptr<::DirectX::CommonStates> commonStates = nullptr;
        std::shared_ptr<::DirectX::SpriteBatch> spriteBatch = nullptr;
        ID3D11ShaderResourceView* texture = nullptr;
    };
}
