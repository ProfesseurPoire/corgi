#pragma once

namespace corgi
{
	/*!
	 * @brief	Count frames. use this if you need something to trigger
	 *			after x frames have passed
	 */
	class FrameCounter
	{
	public:

		/*!
		 * @brief Creates a counter with a random value between min and max
		 */
		FrameCounter(int min, int max) :
			_count_limit (min + rand() % (max - min))
		{
			
		}

		FrameCounter(int limit) :
		_count_limit(limit)
		{
			
		}

		operator bool()const;

		void tick()
		{
			_current++;
		}

		void reset()
		{
			_current = 0;
		}
	
	private:

		int _count_limit;
		int _current{0};
	};

	inline FrameCounter::operator bool() const
	{
		return _current >= _count_limit;
	}
}
