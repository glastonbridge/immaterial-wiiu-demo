#pragma once

#include <memory>
struct RenderMaterial;

using RenderMaterial_p = std::unique_ptr<RenderMaterial>;
RenderMaterial_p createProjectedMaterial();
RenderMaterial_p createComposeMaterial();
RenderMaterial_p createBlurMaterial();
RenderMaterial_p createTextureMaterial(const char *path);
RenderMaterial_p createBoneMaterial(const char *path);
