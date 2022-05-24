#pragma once

#include <corgi/SimpleString.h>
#include <corgi/containers/SharedPtr.h>

#include <corgi/ecs/Component.h>
#include <corgi/ecs/EntityId.h>
#include <corgi/math/Vec3.h>
#include <corgi/components/MeshRenderer.h>
#include <corgi/ecs/ComponentPool.h>

namespace corgi
{
class Font;

class TextRenderer : public Component
{
public:

	enum class HorizontalAlignment
	{
		Left,
		Right,
		Centered
	};

	// Lifecycle

	TextRenderer(RefEntity entity);

	TextRenderer(const TextRenderer& other);
	TextRenderer(TextRenderer&& other) noexcept;

	TextRenderer& operator=(const TextRenderer& other);
	TextRenderer& operator=(TextRenderer&& other) noexcept;

	// Functions

	void update_mesh();

	Material& material();
	corgi::SharedPtr<Mesh> mesh();

	// Variables

	float               scaling = 1.0f;
	SimpleString         text;
	Font*               font;
	Vec3                offset;
	HorizontalAlignment alignment = HorizontalAlignment::Left;

private:
	
	Ref<MeshRenderer> _mesh_renderer;
};
}
