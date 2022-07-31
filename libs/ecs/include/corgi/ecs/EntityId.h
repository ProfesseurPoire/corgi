#pragma once

#include <limits>

namespace corgi
{
/*!
 * @brief 	Identifies an Entity
 *
 *			It's basically just an int, but we did create a user defined
 *			structure for this to differentiate it from a simple index
 */
struct EntityId
{
    /**
	 * @brief Constant value with the greatest possible value for an id
	 */
    static constexpr size_t npos = std::numeric_limits<size_t>::max();

    size_t id_ = npos;

    explicit EntityId(size_t id = npos)
        : id_(id)
    {
    }

    [[nodiscard]] bool operator==(const EntityId& other) const noexcept
    {
        return id_ == other.id_;
    }

    [[nodiscard]] operator bool() const noexcept { return id_ != npos; }

    /*!
	 * @brief We try to make it impossible to convert an EntityId to an unsigned int
	 */
    operator unsigned int() = delete;

    /*!
	 * @brief We try to make it impossible to convert an EntityId to an unsigned int
	 */
    operator int() = delete;
};
}    // namespace corgi
