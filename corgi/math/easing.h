#pragma once

#include <corgi/math/MathUtils.h>
#include <corgi/math/Vec3.h>

#include <iostream>
#include <vector>

namespace easing
{
    static float PI = 3.141597f;

    inline float
    quadratic_easing_in(const float t, const float b, const float c, const float d)
    {
        return c * (t / d) * t + b;
    }

    inline float
    quadratic_easing_out(const float t, const float b, const float c, const float d)
    {
        return -c * (t / d) * (t - 2.0f) + b;
    }

    enum class EasingEquation
    {
        Linear,
        QuadraticEasingIn,
        QuadraticEasingOut,
        QuadraticEasingInOut
    };

    /*!
	 * @brief   t equals time, d equals duration, b equals start value, c is the varying value
	 *          over time
	 */
    inline float linear_easing(float t, float b, float c, float d)
    {
        return ((c * t) / d) + b;
    }

    inline corgi::Vec3 linear_easing(float t, corgi::Vec3 start, corgi::Vec3 end)
    {
        return corgi::Vec3(linear_easing(t, start.x, end.x - start.x, 1.0f),
                           linear_easing(t, start.y, end.y - start.y, 1.0f),
                           linear_easing(t, start.z, end.z - start.z, 1.0f));
    }

    /**
     * @brief   Returns the y value from a quadratic curve/function
     * 
     *          The "in" function represent the part of the curve on the
     *          [0.f;1.f] range
     * 
     * @param time Represent the x value given to the function. 
     *             Must be between 0.f and 1.f
     * @return float    Return value will be int [0.f;1.f] range    
     */
    float easeInQuad(float time);

    /**
     * @brief   Returns the y value from a quadratic function
     * 
     *          The "out" in the function name tells us that we're on the 
     *          [-1.f;0.f] range
     * 
     * @param time  Represents the x value given to the function. 
     *              Must be between 0.f and 1.f
     * @return float    The return value will also be in the [0.f; 1.f] range
     */
    float easeOutQuad(const float t);

    inline float getEasing(EasingEquation equation, float time)
    {
        switch(equation)
        {
            case EasingEquation::QuadraticEasingIn:
                return easeInQuad(time);
            case EasingEquation::QuadraticEasingOut:
                return easeOutQuad(time);
            case EasingEquation::Linear:    // Non implemented yet
                return 0.0f;
            case EasingEquation::QuadraticEasingInOut:    // not implemented yet
                return 0.0f;
        }
        return 0.0f;
    }

    inline float quadratic_easing_in_out(float t, float b, float c, float d)
    {
        t /= d / 2;
        if(t < 1)
            return c / 2 * t * t + b;
        t--;
        return -c / 2 * (t * (t - 2) - 1) + b;
    }

    /*!
     * @brief   Interpolate a value between start and end by using the t value that must be inside the
     *          [0.0f, 1.0f] range
     */
    inline corgi::Vec3
    quadratic_easing_in_out(float t, corgi::Vec3 start, corgi::Vec3 end)
    {
        return corgi::Vec3(quadratic_easing_in_out(t, start.x, end.x - start.x, 1.0f),
                           quadratic_easing_in_out(t, start.y, end.y - start.y, 1.0f),
                           quadratic_easing_in_out(t, start.z, end.z - start.z, 1.0f));
    }

    inline float bounce_easing_out(float t, float b, float c, float d);

    inline float bounce_easing_in(float t, float b, float c, float d)
    {
        return c - bounce_easing_out(d - t, 0, c, d) + b;
    }

    inline float bounce_easing_out(float t, float b, float c, float d)
    {
        if((t /= d) < (1 / 2.75f))
        {
            return c * (7.5625f * t * t) + b;
        }
        else if(t < (2 / 2.75f))
        {
            float postFix = t -= (1.5f / 2.75f);
            return c * (7.5625f * (postFix)*t + .75f) + b;
        }
        else if(t < (2.5 / 2.75))
        {
            float postFix = t -= (2.25f / 2.75f);
            return c * (7.5625f * (postFix)*t + .9375f) + b;
        }
        else
        {
            float postFix = t -= (2.625f / 2.75f);
            return c * (7.5625f * (postFix)*t + .984375f) + b;
        }
    }

    inline float bounce_easing_in_out(float t, float b, float c, float d)
    {
        if(t < d / 2)
            return bounce_easing_in(t * 2, 0, c, d) * .5f + b;
        else
            return bounce_easing_out(t * 2 - d, 0, c, d) * .5f + c * .5f + b;
    }

    inline float elastic_easing_in(float t, float b, float c, float d)
    {
        if(t == 0)
            return b;
        if((t /= d) == 1)
            return b + c;
        const float p = d * .3f;
        const float a = c;
        const float s = p / 4;
        const float postFix =
            a *
            corgi::math::pow(
                2,
                10 * (t -= 1));    // this is a fix, again, with post-increment operators
        return -(postFix * corgi::math::sinf((t * d - s) * (2 * PI) / p)) + b;
    }

    inline float elastic_easing_out(float t, float b, float c, float d)
    {
        if(t == 0)
            return b;
        if((t /= d) == 1)
            return b + c;
        float p = d * .3f;
        float a = c;
        float s = p / 4;

        // update the elasticity value to affect how elastic the thing is
        float elasticity = -10;
        return (a * corgi::math::pow(2, elasticity * t) *
                    corgi::math::sinf((t * d - s) * (2 * PI) / p) +
                c + b);
    }

    inline float elastic_easing_in_out(float t, float b, float c, float d)
    {
        if(t == 0)
            return b;
        if((t /= d / 2) == 2)
            return b + c;
        float p = d * (.3f * 1.5f);
        float a = c;
        float s = p / 4;

        if(t < 1)
        {
            float postFix =
                a * corgi::math::pow(2, 10 * (t -= 1));    // postIncrement is evil
            return -.5f * (postFix * corgi::math::sinf((t * d - s) * (2 * PI) / p)) + b;
        }
        float postFix =
            a * corgi::math::pow(2, -10 * (t -= 1));    // postIncrement is evil
        return postFix * corgi::math::sinf((t * d - s) * (2 * PI) / p) * .5f + c + b;
    }

    struct EasingSegment
    {
        int            time;
        int            startTime;
        EasingEquation easing;
        float          value;
    };

    class EasingAnimation
    {
    public:
        void init(float startValue) { startValue_ = startValue; }

        void addSegment(EasingEquation equation, float value, int time)
        {
            int addedTime = 0;
            if(!segments_.empty())
            {
                addedTime += segments_.back().time;
            }
            segments_.emplace_back(time + addedTime, addedTime, equation, value);
        }

        /**
         * @brief   Returns the segment active at the given time
         *
         *          If no segment could be found for this time, returns
         *          -1
         */
        int getSegmentByTime(int time)
        {
            int index = 0;

            for(const auto& segment : segments_)
            {
                if(time < segment.time)
                {
                    return index;
                }
                index++;
            }
            return -1;
        }

        void start()
        {
            currentTime_ = 0;
            running_     = true;
        }

        float update()
        {
            if(getSegmentByTime(currentTime_) == -1)
            {
                running_ = false;
                return startValue_;
            }
            auto v = value(currentTime_);
            std::cout << "Value " << v << std::endl;
            currentTime_++;
            return v;
        }

        bool running() const { return running_; }

        float value(int time)
        {
            const int segmentIndex = getSegmentByTime(time);

            if(segmentIndex == -1)
                return 0.0f;

            const auto segment = segments_[segmentIndex];

            const float finalValue = segment.value;
            float       startValue {0.0f};

            const float duration =
                static_cast<float>(segment.time - segment.startTime - 1);

            const float actualTime = static_cast<float>(time - segment.startTime);

            const float t = actualTime / duration;

            if(segmentIndex == 0)
                startValue = startValue_;
            else
                startValue = segments_[segmentIndex - 1].value;

            const float dif = finalValue - startValue;

            switch(segment.easing)
            {
                case EasingEquation::Linear:
                    return startValue + t * dif;
                case EasingEquation::QuadraticEasingIn:
                    return startValue + easeInQuad(t) * dif;
                case EasingEquation::QuadraticEasingOut:
                    return startValue + easeOutQuad(t) * dif;
            }
        }

    private:
        bool                       running_ = false;
        float                      startValue_;
        int                        currentTime_ = 0;
        std::vector<EasingSegment> segments_;
    };
}    // namespace easing