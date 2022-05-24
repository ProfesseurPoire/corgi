#pragma once

#include <corgi/ecs/EntityId.h>

namespace corgi
{
	class Entity;
	class Scene;

	/*!
	 *  @brief	Stores the ID of an Entity and use it to retrieve the
	 *			correct Entity inside the contiguous array used by the ECS system
	 */
	class RefEntity
	{
	public:

	// Lifecycle
		
		explicit RefEntity() = default;
		explicit RefEntity(Scene& scene, const Entity& entity);
		explicit RefEntity(Entity& entity);
		
		RefEntity(const RefEntity& other);
		RefEntity(RefEntity&& other) noexcept;

		RefEntity& operator=(const RefEntity& other);
		RefEntity& operator=(RefEntity&& other)noexcept;

		~RefEntity() = default;

	// Functions 

		/*!
		 * @brief	Returns false if the object doesn't reference anything
		 */
		[[nodiscard]] explicit operator bool()const noexcept;

		/*!
		 * @brief	Invalidates the current Reference
		 *
		 *			It basically just sets the _id to -1
		 */
		void reset();

	// Accessors

		//[[nodiscard]] EntityId entity_id()const;
		
		[[nodiscard]] Entity* operator->();
		[[nodiscard]] const Entity* operator->()const;

		[[nodiscard]] Entity& operator*();
		[[nodiscard]] const Entity& operator*()const;

		[[nodiscard]] bool operator==(const RefEntity& other)const noexcept;

	// Conversions

		 operator int() const  noexcept = delete;

	private:

		EntityId _id;
		Scene* _scene = nullptr;
	};
}
