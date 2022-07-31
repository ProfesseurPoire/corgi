#pragma once

#include <corgi/ecs/Component.h>
#include <corgi/ecs/ComponentPool.h>
#include <memory>
#include <string>

namespace corgi
{
class Font;
class MeshRenderer;
class Material;
class Mesh;

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
	std::shared_ptr<Mesh> mesh();

	// Variables

	float               scaling = 1.0f;
	std::string         text;
	Font*               font;

    struct
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    } offset;

	HorizontalAlignment alignment = HorizontalAlignment::Left;

private:
	
	Ref<MeshRenderer> _mesh_renderer;
};
}
