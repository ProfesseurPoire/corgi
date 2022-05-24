#pragma once

#include <corgi/components/ColliderComponent.h>
#include <corgi/ecs/EntityId.h>

namespace corgi
{
	class Entity;

	// TODO : At one point I'll probably end up using SAT for everything
	//			so this will be renamed BoxCollider 

	// A collider needs transform component
	// AABB stands for axis aligned bounding box
	// Attach functions/lambdas to the exit, enter and collision callbacks

	// The BoxCollider is actually building a new Mesh under the hood representing
	// the actual Box. When the BoxCollider is destroyed, the associated mesh is 
	// also automatically deleted
	class BoxCollider : public ColliderComponent
	{
		friend class CollisionSystem;
		friend class Renderer;
		friend class Physic;

	public:

	// Constructors

		BoxCollider();
		BoxCollider(int layer, float width, float height, float depth=1.0f);

	//  Functions

		void dimensions(float width, float height, float depth);

		float width()const;
		float height()const;
		float depth()const;

		void width(float v);
		void height(float v);
		void depth(float v);

	// Variables

		// TODO : Maybe remove the offset thing. If you really need to offset
		// a box collider, maybe just attach it to a sub node or something
		float offsetX	{0.0f};
		float offsetY	{0.0f};
		float offsetZ	{0.0f};
		
		// Contains the triangles that makes the collider

		void build_box();

	protected:

		float _width	{ 1.0f };
		float _height	{ 1.0f };
		float _depth	{ 1.0f };
	};
}
