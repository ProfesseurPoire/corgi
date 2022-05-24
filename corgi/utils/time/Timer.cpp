#include <corgi/utils/time/Timer.h>
#include <iostream>
#include <chrono>

namespace corgi
{
	namespace time
	{
	    void Timer::start()
	    {
	        auto now  = std::chrono::system_clock::now();
	        _nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
	    }

	    float Timer::elapsed_time()const noexcept
	    {
	        auto now  = std::chrono::system_clock::now();
	        auto ns  = duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();

	        auto difference = ns - _nanoseconds;
	        return static_cast<float>(difference) / 1000000000.0f;
	    }
	}
}