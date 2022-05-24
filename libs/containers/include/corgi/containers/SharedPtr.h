#pragma once

namespace corgi
{
	template<class T>
	class SharedPtr
	{
	public:

		SharedPtr() = default;

		SharedPtr(T* t)
		{
			_count = new int;
			*_count = 1;
			_pointer = t;
		}

		SharedPtr(const SharedPtr& other)
		{
			_count = other._count;
			(*_count) = (*_count) + 1;
			_pointer = other._pointer;
		}

		SharedPtr& operator=(const SharedPtr& other)
		{
			//Just handling self assignment here 
			if(other._pointer==_pointer)
				return *this;
			
			destroy();

			_count		= other._count;
			(*_count)	= (*_count) + 1;
			_pointer	= other._pointer;
			return *this;
		}

		SharedPtr(SharedPtr&& other)noexcept
		{
			_count = other._count;
			_pointer = other._pointer;

			other._count = nullptr;
			other._pointer = nullptr;
		}

		SharedPtr& operator=(SharedPtr&& other) noexcept
		{
			destroy();

			_count		= other._count;
			_pointer	= other._pointer;

			other._count = nullptr;
			other._pointer = nullptr;

			return *this;
		}

		T& operator*()
		{
			return *_pointer;
		}

		operator bool()const noexcept
		{
			return _pointer != nullptr;
		}

		void reset()
		{
			destroy();
		}

		~SharedPtr()
		{
			destroy();
		}

		[[nodiscard]] T* operator->()const noexcept
		{
			return _pointer;
		}

		[[nodiscard]] T* get()const noexcept
		{
			return _pointer;
		}

	private:

		void destroy()
		{
			if (_count != nullptr)
			{
				*_count = *_count -1;

					if (*_count == 0)
					{
						delete _pointer;
						delete _count;
					}
			}
		}

		int* _count = nullptr;
		T* _pointer = nullptr;
	};

	template<class T, class ... Args>
	static SharedPtr<T> make_shared(Args&& ... args)
	{
		return SharedPtr(new T(std::forward<Args>(args)...));
	}
}
