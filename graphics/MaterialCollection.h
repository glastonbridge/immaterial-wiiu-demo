#pragma once

#include <memory>
struct RenderMaterial;

using RenderMaterial_p = std::unique_ptr<RenderMaterial>;
RenderMaterial_p createComposeMaterial();
RenderMaterial_p createBlurMaterial();
