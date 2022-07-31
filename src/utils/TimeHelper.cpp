#include <corgi/utils/TimeHelper.h>

#include <ctime>
#include <sstream>

using namespace corgi;


void Time::start()
{
	_start_clock = std::chrono::steady_clock::now();
	_last_update_counter = 0.0f;
}

void Time::pause()
{
	_elapsed_time = std::chrono::duration<float>(std::chrono::steady_clock::now() - _start_clock).count();
	_last_update_counter+=_elapsed_time ;
}

void Time::resume()
{
	_start_clock = std::chrono::steady_clock::now();
}

void Time::update()
{
	_elapsed_time	= std::chrono::duration<float>(std::chrono::steady_clock::now() - _start_clock).count();
	_start_clock	= std::chrono::steady_clock::now();

	// When the window lose the focus, the game stop being updated and one frame will register 
	// a huge elapsed_time that can lead to unwanted behaviors like ennemies or character going 
	// through wall for instance. So, we make sure the elapsed_time never goes beyond a certain value
	//if (_elapsed_time > 0.4f)
	//	_elapsed_time = timestep();

	_last_update_counter += elapsed_time();
}

void Time::clear()
{
	for (size_t i = 0; i < _callbacks.size(); ++i)
	{
		if (_callbacks[i] != nullptr)
		{
			delete _callbacks[i];
		}
	}
	_callbacks.clear();
}

float Time::timestep() const
{
	return _timestep;
}

void Time::timestep(float seconds)
{
	_timestep = seconds;
}

void Time::detach(Callback* cb)
{
	if (cb)
	{
		//_callbacks.remove(cb);
		delete cb;
	}
}

float Time::elapsed_time()const
{
	return _elapsed_time;
}

bool Time::timestep_overrun()
{
	// We only update the logic if the time elapsed is higher than the 
	// current timestep (fixed timestep)

	//std::cout << "Last update : " << _last_update_counter << std::endl;
	
	if (_last_update_counter > timestep())
	{
		// we make up for the difference 
		_last_update_counter -= timestep();

		for (auto* cb : _callbacks)
			cb->update();
		
		return true;
	}
	return false;
}

std::string Time::get_time()
{
	auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::stringstream ss;
	std::string time_string = std::ctime(&time);
	time_string.erase(time_string.size() - 1);

	return "";
}