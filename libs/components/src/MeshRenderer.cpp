#include <corgi/components/MeshRenderer.h>
#include <corgi/utils/ResourcesCache.h>

namespace corgi
{
MeshRenderer::MeshRenderer(RefEntity id)
{
    _entity_id = id;
    material   = *ResourcesCache::get<Material>(
        "corgi/materials/unlit/unlit_color.mat");
    material.is_transparent(false);

    layers.emplace_back(1);
}

MeshRenderer::MeshRenderer(RefEntity id, Material material)
{
    _entity_id     = id;
    this->material = material;
    layers.emplace_back(1);
}

MeshRenderer::MeshRenderer(const MeshRenderer& other)
{
    material   = other.material;
    _mesh      = other._mesh;
    _entity_id = other._entity_id;
    layer      = other.layer;
    layers.emplace_back(1);
}

MeshRenderer::MeshRenderer(MeshRenderer&& other) noexcept
{
    material   = other.material;
    _mesh      = (other._mesh);
    _entity_id = other._entity_id;
    layer      = other.layer;
}

MeshRenderer& MeshRenderer::operator=(const MeshRenderer& other)
{
    material   = other.material;
    _mesh      = other._mesh;
    _entity_id = other._entity_id;
    layer      = other.layer;
    return *this;
}

MeshRenderer& MeshRenderer::operator=(MeshRenderer&& other) noexcept
{
    material   = other.material;
    _mesh      = other._mesh;
    _entity_id = other._entity_id;
    layer      = other.layer;
    return *this;
}
}    // namespace corgi
