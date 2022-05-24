#include <corgi/components/Animator.h>
#include <corgi/components/SpriteRenderer.h>
#include <corgi/components/Transform.h>
#include <corgi/ecs/Entity.h>
#include <corgi/logger/log.h>
#include <corgi/math/easing.h>

namespace corgi
{
    ScalingAnimation::Segment ScalingAnimation::current_segment() const
    {
        return {keyframes_[current_segment_], keyframes_[current_segment_ + 1],
                keyframes_[current_segment_ + 1].interpolation};
    }

    Animation& Animator::current_animation() { return _animations[_current_animation_index]; }

    const Animation& Animator::current_animation() const { return _animations[_current_animation_index]; }

    void set_key_frame(Animator& animator, SpriteRenderer& spriter, Sprite sprite)
    {
        spriter.flip_horizontal(animator.current_animation().flipped_x ^ animator.is_flipped_);
        spriter.sprite(sprite);
    }

    void Animator::add_animation(Animation animation) { _animations.emplace(animation.name(), animation); }

    void Animator::add_animations(const std::vector<Animation>& animations)
    {
        for(auto& anim : animations)
            _animations.emplace(anim.name(), anim);
    }

    void Animator::add_animations(std::map<SimpleString, Animation> animations)
    {
        for(auto& [key, animation] : animations)
            _animations.emplace(key, animation);
    }

    void Animator::play(const SimpleString& name, const int frame, const unsigned time)
    {
        current_animation_       = name;
        _current_animation_index = _animations.index(name);
        current_frame_index      = frame;
        current_time             = time;
        running                  = true;

        if(static_cast<int>(current_animation().frames.size()) <= frame)
        {
            log_error(("Current animation frame out of bound " + name + " frame : " + to_string(frame)).c_str());
        }
    }

    void Animator::play_scaling_animation(const ScalingAnimation& animation)
    {
        scaling_animation_            = animation;
        scaling_animation_.is_playing = true;
    }

    void Animator::stop() { running = false; }

    bool Animator::is_playing(const SimpleString& name)
    {
        if(current_animation_ != "")
        {
            return current_animation().name() == name;
        }
        return false;
    }

    void Animator::set_animations(std::map<SimpleString, Animation> animations)
    {
        for(auto& [key, animation] : animations)
            _animations.emplace(key, animation);
    }

    Animation& Animator::new_animation(const SimpleString& name)
    {
        _animations.emplace(name, Animation());
        return _animations.at(name);
    }

    Animation& Animator::new_animation(const SimpleString& name, const SimpleString& reference)
    {
        _animations.emplace(name, _animations.at(reference));
        return _animations.at(name);
    }

    Animation& Animator::new_animation(const SimpleString& name, const Animation& reference)
    {
        _animations.emplace(name, reference);
        return _animations.at(name);
    }

    const Animation::Frame& Animator::current_frame() const
    {
        return current_animation().frames.at(current_frame_index);
    }

    bool Animator::is_playing() const { return running; }

    void Animator::next_frame()
    {
        current_frame_index++;

        if(current_frame_index >= current_animation().frames.size())
        {
            if(current_animation().looping)
                current_frame_index = 0;
            else
            {
                running = false;
                current_frame_index--;
            }
        }
    }

    void ScalingAnimation::stop() { is_playing = false; }

    corgi::Vec3 ScalingAnimation::value()
    {
        auto segment = current_segment();
        Vec3 v;

        float t = float(current_tick - segment.first.time) / (segment.last.time - segment.first.time);

        switch(segment.interpolation)
        {
            case Interpolation::Linear:
                return easing::linear_easing(t, segment.first.value, segment.last.value);
            case Interpolation::Quadratic:
                return easing::quadratic_easing_in_out(t, segment.first.value, segment.last.value);
            case Interpolation::Cubic:
                break;
        }
        return v;
    }

    bool ScalingAnimation::current_segment_timed_out() const noexcept
    {
        return current_segment().last.time < current_tick;
    }

    bool ScalingAnimation::has_next_segment() const
    {
        return ((current_segment_ + 1) < (static_cast<int>(keyframes_.size()) - 1));
    }

    bool ScalingAnimation::next_segment()
    {
        if(!has_next_segment())
        {
            return false;
        }
        current_segment_++;
        return true;
    }

}    // namespace corgi