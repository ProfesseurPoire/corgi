#pragma once

#include <vector>

// I would like to hide chrono here ...
#include <chrono>

namespace corgi
{
    class Callback
    {
    public:
        virtual ~Callback() = default;
        virtual void start() { _is_active = true; }
        virtual void reset() {}

        virtual void update() {}

        bool is_active() { return _is_active; }

    protected:
        bool _is_active = false;
    };

    // // Simply call the affected object method after x update tick occurred
    // template<class T>
    // class Simple_callback : public Callback
    // {
    // 	friend class Time;

    // public:

    // 	Simple_callback(T& obj, void(T::*ptr)(void), int ticks);
    // 	Simple_callback(T* obj, void(T::*ptr)(void), int ticks);

    // 	void reset()override
    // 	{
    // 		_tick_count = 0;
    // 		_is_active = true;
    // 	}

    // private:

    // 	void update()override;

    // 	void(T::*_method)(void);
    // 	T*      _object;
    // 	int     _tick_count = 0;
    // 	int     ticks;
    // };

    // template <class T>
    // Simple_callback<T>::Simple_callback(T& obj, void(T::*ptr)(void), int ticks)
    // 	: _method(&ptr), _object(obj) {}

    // template<class T>
    // Simple_callback<T>::Simple_callback(T* obj, void(T::*ptr)(void), int ticks)
    // 	: _method(ptr), _object(obj) {}

    // template<class T>
    // void Simple_callback<T>::update()
    // {
    // 	if (!_is_active)
    // 		return;

    // 	// if (_tick_count++ == tick)
    // 	// {
    // 	// 	(_object->*_method)();
    // 	// 	_is_active = false;
    // 	// }
    // }

    // Call the affected member method repeatedly
    template<class T>
    class Repeat_callback : public Callback
    {
        friend class Time;

    public:
        Repeat_callback(T* obj, void (T::*ptr)(void), int interval, int max_repeat = -1);
        Repeat_callback(T& obj, void (T::*ptr)(void), int interval, int max_repeat = -1);

        void reset() override
        {
            _tick_count = 0;
            _sum_repeat = 0;
            _is_active  = true;
        }

    private:
        void update() override;

        void (T::*_method)(void);
        T* _object;

        int _interval;
        int _max_repeat;
        int _tick_count = 0;
        int _sum_repeat = 0;
    };

    template<class T>
    Repeat_callback<T>::Repeat_callback(T* obj,
                                        void (T::*ptr)(void),
                                        int interval,
                                        int max_repeat)
        : _object(obj)
        , _method(ptr)
        , _interval(interval)
        , _max_repeat(max_repeat)
    {
    }

    template<class T>
    Repeat_callback<T>::Repeat_callback(T& obj,
                                        void (T::*ptr)(void),
                                        int interval,
                                        int max_repeat)
        : _object(obj)
        , _method(ptr)
        , _interval(interval)
        , _max_repeat(max_repeat)
    {
    }

    template<class T>
    void Repeat_callback<T>::update()
    {
        if(!_is_active)
            return;

        if(_tick_count++ == _interval)
        {
            (_object->*_method)();
            _tick_count = 0;

            if(_sum_repeat++ == _max_repeat)
                _is_active = false;
        }
    }

    // Use the time class to attach methods that need to be call at certain points in time
    // or that need to be call repeatidly, or to know the current timestep
    class Time
    {
        friend class Game;
        //friend class Editor;

        std::chrono::steady_clock::time_point _start_clock;

    public:
        static std::string get_time();

        void pause();
        void resume();

        void clear();
        void start();

        bool timestep_overrun();

        // template<class T>
        // Simple_callback<T>* attach_simple_callback(T& obj, void(T::*ptr)(void), int ticks);

        // template<class T>
        // Simple_callback<T>* attach_simple_callback(T* obj, void(T::*ptr)(void), int ticks);

        template<class T>
        Repeat_callback<T>* attach_repeat_callback(T* obj,
                                                   void (T::*ptr)(void),
                                                   int interval,
                                                   int max_repeat = -1);

        template<class T>
        Repeat_callback<T>* attach_repeat_callback(T& obj,
                                                   void (T::*ptr)(void),
                                                   int interval,
                                                   int max_repeat = -1);

        void detach(Callback* callback);

        void update();

        /*!
		 * @brief	Returns the time it took for the last frame refresh to execute in seconds
		 */
        float elapsed_time() const;

        float timestep() const;
        void  timestep(float ts);

    private:
        std::vector<Callback*> _callbacks;

        float _timestep            = 1.0f / 100.0f;
        float _elapsed_time        = 0.0f;
        float _last_update_counter = 0.0f;
    };

    /*!
	 * @brief	bool operator will return true when x frame have passed 
	 */
    class Timer
    {
    public:
        explicit Timer(const int time)
            : time_(0)
            , end_(time)
        {
        }

        explicit operator bool() const { return time_ >= end_; }

        void start()
        {
            time_    = 0;
            enabled_ = true;
        }

        int time() const { return time_; }

        void stop() { enabled_ = false; }

        void restart() { time_ = 0; }

        void update()
        {
            if(enabled_)
            {
                time_++;
            }
        }

    private:
        int  time_;
        int  end_;
        bool enabled_ = false;
    };

    /*!
	 * @brief	As long as the counter is not reset, it will continue to register the
	 *			elapsed time since the last start() call.
	 *			
	 *			tick() will also count how many time the tick() function has been called
	 */
    class Counter
    {
        Time  _time;
        int   _ticks {0};
        float _elapsedTime {0.0f};

    public:
        void start() { _time.start(); }

        [[nodiscard]] float ticks() const noexcept { return static_cast<float>(_ticks); }

        void tick()
        {
            _ticks++;
            _time.update();
            _elapsedTime += _time.elapsed_time();
        }

        float elapsedTime() const { return _elapsedTime; }

        void reset()
        {
            _ticks       = 0;
            _elapsedTime = 0.0f;
            _time.clear();
        }

        float tickPerSecond() const { return static_cast<float>(_ticks) / _elapsedTime; }
    };

    // template<class T>
    // Simple_callback<T>* Time::attach_simple_callback(T& obj, void(T::*ptr)(void), int ticks)
    // {
    // 	Simple_callback<T>* sc = new Simple_callback(obj, ptr, ticks);
    // //	_callbacks.add(sc);
    // 	return sc;
    // }

    // template<class T>
    // Simple_callback<T>* Time::attach_simple_callback(T* obj, void(T::*ptr)(void), int ticks)
    // {
    // 	Simple_callback<T>* sc = new Simple_callback<T>(obj, ptr, ticks);
    // 	//_callbacks.add(sc);
    // 	return sc;
    // }

    template<class T>
    Repeat_callback<T>* Time::attach_repeat_callback(T* obj,
                                                     void (T::*ptr)(void),
                                                     int interval,
                                                     int max_repeat)
    {
        Repeat_callback<T>* sc = new Repeat_callback<T>(obj, ptr, interval, max_repeat);
        //_callbacks.emplace(sc);
        return sc;
    }

    template<class T>
    Repeat_callback<T>* Time::attach_repeat_callback(T& obj,
                                                     void (T::*ptr)(void),
                                                     int interval,
                                                     int max_repeat)
    {
        Repeat_callback<T>* sc = new Repeat_callback<T>(obj, ptr, interval, max_repeat);
        //_callbacks.add(sc);
        return sc;
    }
}    // namespace corgi
