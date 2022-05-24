#pragma once

#include <corgi/math/Vec3.h>
#include <corgi/resources/Animation.h>

#include <map>
#include <vector>

namespace corgi
{
    /*!
	 * @brief	The goal of this class is simply to add a key to
	 *			fuse a map with a vector. Datas are still stored
	 *			inside a contiguous vector, and we can still iterate
	 *			directly other the vector, but we can also access things
	 *			with the key
	 *
	 */
    template<class Key, class Value>
    class VectorMap
    {
    public:
        // Lifecycle

        // Functions

        // Modifiers

        void clear() noexcept
        {
            _values.clear();
            _key_to_indexes.clear();
        }

        void emplace(Key key, Value value)
        {
            if(_key_to_indexes.contains(key))
            {
                return;
            }

            _key_to_indexes.emplace(key, static_cast<int>(_values.size()));
            _values.emplace_back(value);
        }

        // Capacity

        [[nodiscard]] int size() const noexcept { return _values.size(); }

        [[nodiscard]] bool empty() const noexcept { return _values.empty(); }

        // Element Access

        [[nodiscard]] int index(Key key) { return _key_to_indexes.at(key); }

        [[nodiscard]] Value& at(Key key) { return _values.at(_key_to_indexes.at(key)); }

        [[nodiscard]] const Value& at(Key key) const
        {
            return _values.at(_key_to_indexes.at(key));
        }

        [[nodiscard]] Value& at(int index) { return _values.at(index); }

        [[nodiscard]] const Value& at(int index) const { return _values.at(index); }

        [[nodiscard]] Value& operator[](int index) { return _values[index]; }

        [[nodiscard]] const Value& operator[](int index) const { return _values[index]; }

    private:
        std::map<Key, int> _key_to_indexes;
        std::vector<Value> _values;
    };

    class Texture;

    struct ScalingAnimation
    {
        enum class Interpolation : char
        {
            Linear,
            Quadratic,
            Cubic
        };

        struct KeyFrame
        {
            int           time;
            Vec3          value;
            Interpolation interpolation;
        };

        struct Segment
        {
            KeyFrame      first;
            KeyFrame      last;
            Interpolation interpolation;
        };

        void stop();

        corgi::Vec3 value();
        bool        has_next_segment() const;

        /*!
		 * @brief	This function will try to change the current segment to the next one
		 *			If there's no segment after, returns false, true otherwise
		 *
		 * @returns		Returns false if no next segment exist, true otherwise
		 */
        bool next_segment();

        std::vector<KeyFrame> keyframes_;

        [[nodiscard]] bool current_segment_timed_out() const noexcept;

        [[nodiscard]] Segment current_segment() const;

        int  current_segment_ = 0;    //4
        int  current_tick     = 0;
        bool is_playing       = false;
    };

    class Animator
    {
    public:
        Animator() = default;

        Animator(const Animator& a)     = default;
        Animator(Animator&& a) noexcept = default;

        Animator& operator=(const Animator& other) = default;
        Animator& operator=(Animator&& other) noexcept = default;

        // Functions

        void add_animation(Animation animation);
        void add_animations(const std::vector<Animation>& animations);
        void add_animations(std::map<SimpleString, Animation> animations);

        Animation& new_animation(const SimpleString& name);
        Animation& new_animation(const SimpleString& name, const Animation& reference);
        Animation& new_animation(const SimpleString& name, const SimpleString& reference);

        [[nodiscard]] Animation&       current_animation();
        [[nodiscard]] const Animation& current_animation() const;

        void set_animations(std::map<SimpleString, Animation> animations);

        /// <summary>
        /// Play the animation identified by name
        /// </summary>
        /// <param name="name"></param>
        /// <param name="frame">Second parameter starts the animation at a specific time in ms</param>
        /// <param name="time"></param>
        void play(const SimpleString& name, int frame = 0, unsigned time = 0u);

        void play_scaling_animation(const ScalingAnimation& anim);

        void stop();

        /*
         * @brief   Checks if the given animation is currently playing
         * @param   name    Name of the animation we want to check
         * @return  Returns true if the animation is playing, false otherwise
         */
        bool is_playing(const SimpleString& name);

        // Returns the current frame of the current animation
        const Animation::Frame& current_frame() const;

        // TODO  : put everything private, add some getter/setters

        bool is_playing() const;

        void enable() { _enabled = true; }

        void disable() { _enabled = false; }

        bool enabled() const { return _enabled; }

        void enabled(bool val) { _enabled = val; }

        void next_frame();
        void update_scaling_animation();

        VectorMap<SimpleString, Animation> _animations;
        ScalingAnimation                   scaling_animation_;
        SimpleString                       current_animation_;         // 4
        int                                current_frame_index {0};    // 4

        // In Milliseconds
        unsigned current_time {0u};    // 4
        // Checks if an animation is still playing
        int  _current_animation_index = -1;    // 4
        bool is_flipped_ {false};
        bool _enabled = true;
        bool running {false};    // 1
    };
}    // namespace corgi
