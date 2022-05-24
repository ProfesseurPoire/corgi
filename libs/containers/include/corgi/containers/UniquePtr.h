#pragma once
#include "UniquePtr.h"

namespace corgi
{
	template<class T>
	class UniquePtr 
	{
	public:

		UniquePtr() = default;

		/*!
		 * @brief UniquePtr will take ownership of the given pointer
		 */
		UniquePtr(T* ptr) :_pointer(ptr){}

		UniquePtr(UniquePtr&& other) noexcept
			: _pointer(other._pointer)
		{
			other._pointer = nullptr;
		}

		template<class U>
		UniquePtr(UniquePtr<U>&& other) noexcept
			:_pointer(dynamic_cast<T*>(other._pointer))
		{
			other._pointer = nullptr;
		}

		UniquePtr& operator=(const UniquePtr&) = delete;
		UniquePtr(const UniquePtr&) = delete;


		UniquePtr& operator=(UniquePtr&& other) noexcept
		{
			if(_pointer)
				delete _pointer;

			_pointer = other._pointer;
			other._pointer = nullptr;
			return *this;
		}
		~UniquePtr(){delete _pointer;}

		/*!
		 * @brief	Returns true if the UniquePtr is currently owning something
		 *			Returns false otherwise
		 */
		operator bool() const noexcept
		{
			return _pointer != nullptr;
		}

		T* operator->()
		{
			return _pointer;
		}

		const T* operator->()const
		{
			return _pointer;
		}

		T& operator*()
		{
			return *_pointer;
		}

		const T& operator*()const
		{
			return *_pointer;
		}

		/*!
		 * @brief Resets the pointer managed by the UniquePtr
		 */
		void reset(T* p=nullptr)
		{
			delete _pointer;
			_pointer = p;
		}

		/*!
		 * @brief	Returns the pointer owned by the UniquePtr
		 *
		 *			Be Careful when using this function that you don't
		 *			delete the memory or something like that
		 */
		T* get()const noexcept;

		T* _pointer = nullptr;
	};


	template<class T, class ... Args>
	UniquePtr<T> make_unique(Args&& ... args)
	{
		return UniquePtr(new T(std::forward<Args>(args)...));
	}


	/*!
			 * @brief	Returns the pointer owned by the UniquePtr
			 *
			 *			Be Careful when using this function that you don't
			 *			delete the memory or something like that
			 */
	template<class T>
	T* UniquePtr<T>::get()const noexcept
	{
		return _pointer;
	}
}
