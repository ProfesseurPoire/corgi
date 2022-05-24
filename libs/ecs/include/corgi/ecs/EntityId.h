#pragma once

namespace corgi
{
	/*!
	 * @brief Identifies an Entity
	 *
	 *			It's basically just an int, but we did create a user defined
	 *			structure for this to differentiate it from a simple index
	 */
	struct EntityId
	{
		int id_ = -1;

		explicit EntityId(int id=-1) : id_(id) {}

		[[nodiscard]] bool operator==(const EntityId& other)const noexcept{ return id_ == other.id_;}
		[[nodiscard]] operator bool()const noexcept { return id_ != -1;}

		/*!
		 * @brief We try to make it impossible to convert an EntityId to an int
		 */
		operator int() = delete;
	};
}
