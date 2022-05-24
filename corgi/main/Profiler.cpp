#include <sstream>

#include <corgi/main/Profiler.h>

using namespace corgi;

//static std::string to_string(float value, int precision)
//{
//	std::ostringstream out;
//	out.precision(precision);
//	out << std::fixed << value;
//	return out.str();
//}

void Profiler::update()
{
	// We only refresh the Profiler information every x seconds
	if (loop_counter_.elapsedTime() > refresh_rate_)
	{
		// We're using a custom to_string function because of precisions issues
		update_time_	    = update_counter_.elapsedTime() * 1000.0f / renderer_counter_.ticks() ;
		render_time_		= renderer_counter_.elapsedTime() * 1000.0f / renderer_counter_.ticks();
		frame_per_seconds_  = loop_counter_.tickPerSecond();
		reset();
	}
}

float Profiler::frames_per_seconds() const noexcept
{
	return frame_per_seconds_;
}

float Profiler::render_time() const noexcept
{
	return render_time_;
}

float Profiler::update_time() const noexcept
{
	return update_time_;
}

void Profiler::reset()
{
	loop_counter_.reset();
	renderer_counter_.reset();
	update_counter_.reset();
}