#pragma once

#include <map>
#include <typeindex>
#include <memory>
#include <functional>

namespace corgi::containers
{
	/*!
	 * @brief	Use this data structure when you want to store an object using 
	 * 			their type U as a key. U must inherit from T
	 *
	 *			This is really just a wrapper around a map that stores an 
	 * 			std::type_index as a key and objects that inherits from T
	 */
	template<class T>
	class TypeMap
	{
	public:

		template<class U>
		void add(U* u)
		{
			map_.emplace(typeid(U), u);
		}

		void clear()
		{
			map_.clear();
		}

		template<class U, class ...Args >
		void add(Args&&... param)
		{
			auto deleter = [](T* u)
			{
				delete static_cast<U*>(u);
			};

			std::unique_ptr<U, decltype(deleter)> ptr(new U(std::forward<Args>(param)...), deleter);

			map_.try_emplace
			(
				typeid(U),
				std::move(ptr)
			);
		}

		template<class U>
		U* get()
		{
			return static_cast<U*>(map_.at(typeid(U)).get());
		}

		auto begin()
		{
			return map_.begin();
		}

		auto end()
		{
			return map_.end();
		}

		auto begin()const
		{
			return map_.cbegin();
		}

		auto end()const
		{
			return map_.cend();
		}

		/*!
		 *	@brief	Checks if the container stores an element with a key equals to typeid(T)
		 * 	@tparam T convert the template parameter to a key 
		 *  @return 	Returns true if th ekey already exist. False otherwise
		 */
		template<class T>
		[[nodiscard]] bool contains() const 
		{ 
			return exists(typeid(T)); 
		}

		/*!
		 *	@brief	Checks if the container stores an element with a key equals to @p key
		 *	@param	type Key to inspect 
		 *	@return	Returns true if the key already exist. False otherwise
		 */
		[[nodiscard]] bool contains(const std::type_info& key)const
		{
			return map_.contains(key);
		}

	private:

		// I tried to use std::any, but then it falls apart if I'm using polymorphism
		// since afaik, I can't use any_cast with a polymorphic/abstract class
		// I only returns null since the any_cast failed
		std::map<std::type_index, std::unique_ptr<T, std::function<void(T*)>>> map_;
	};
}