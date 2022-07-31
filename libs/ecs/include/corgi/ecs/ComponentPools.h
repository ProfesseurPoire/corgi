#pragma once

#include <map>
#include <memory>
#include <typeindex>

#include <corgi/ecs/ComponentPool.h>

namespace corgi
{
class ComponentPools
{
public:

	// Capacity

	/*!
	 * @brief	Returns how many pools are stored in the list
	 */
	[[nodiscard]] int size() const noexcept;

	// Modifiers

	/*!
	 * @brief	Returns a non owning pointer to the component pool of type T.
	 *			Returns nullptr if no pool of type T is stored inside the container
	 */
	template <class T>
	[[nodiscard]] ComponentPool<T>* get()
	{
		if(!pools_.contains(typeid(T)))
			add<T>();
		return dynamic_cast<ComponentPool<T>*>(get(typeid(T)));
	}

	template <class T>
	[[nodiscard]] const ComponentPool<T>* get() const
	{
		return dynamic_cast<const ComponentPool<T>*>(get(typeid(T)));
	}

	[[nodiscard]] const AbstractComponentPool* get(std::type_index t)const;
	[[nodiscard]] AbstractComponentPool* get(std::type_index t);
	

	/*!
	* @brief	Adds a new ComponentPool of the specified type
	* 
	*			If a component pool of the same type already exists, it won't
	*			construct it 
	*/
	template <class T>
	void add(unsigned int size = 0)
	{
		if(contains<T>())
			return;

		pools_.emplace(typeid(T), new ComponentPool<T>(size));
	}

	/*!
	* @brief	Removes a pool of type @a T from the list (if it exists)
	*/
	template <class T>
	void remove()
	{
		remove(typeid(T));
	}

	void remove(const std::type_info& component_type);

	// Lookup

	/*!
	* @brief	Checks if the a pool of type T has already been stored
	*/
	template <class T>
	[[nodiscard]] bool contains() const noexcept
	{
		return contains(typeid(T));
	}

	[[nodiscard]] bool contains(std::type_index t)const noexcept;

	std::map<std::type_index, std::unique_ptr<AbstractComponentPool>>::iterator
	begin();
	std::map<std::type_index, std::unique_ptr<AbstractComponentPool>>::iterator
	end();

private:

	std::map<std::type_index, std::unique_ptr<AbstractComponentPool>> pools_;
};
}
