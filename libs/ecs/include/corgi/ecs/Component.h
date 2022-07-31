#pragma once

#include <string>
#include <map>
#include <typeindex>

namespace corgi
{
	class Entity;
	class Scene;
	class Transform;
	class SpriteRenderer;

	/*!
	 * @brief Components are data object attached to an entity and updated by the systems
	 *
	 *	 We don't use a virtual destructor because components are stored inside array of their type
	 */
	class Component
	{
		friend class Entity;

	public:

	// Lifecycle

		Component() : is_enabled_(true), padding{false,false,false}{}

		// TODO : Would be nice to not have it as a virtual destructor
		virtual ~Component();

	// Functions

		[[nodiscard]] bool is_enabled() const;

		void enable();
		void disable();

		void is_enabled(bool value);

		static inline std::map<std::type_index, std::string> type_name;
		static int register_type_name(std::type_index t, std::string);

	protected:

		bool is_enabled_ = true;

		// To remind me there's 3 unused bytes and to remove c4820 warning
		bool padding[3];
	};
}
