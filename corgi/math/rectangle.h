#pragma once

#include <corgi/math/detail/Vec2.h>

namespace corgi { namespace math { namespace detail {

enum collision_direction
{
    collision_top,
    collision_bottom,
    collision_left,
    collision_right,
    collision_none
};

/*! 
    @brief  Represents a rectangle
    The x and y variables represents the top left corner of the rectangle

    (x,y)   (x+width,y)
    *------->
    |
    |
    v
    (x, y+height)
*/
template<class T>
class Rectangle
{
public:

// Static Methods

    // TODO this needs more explanation, also, where is it used? 
    /*!
        @brief  This fuction will push back r2 from r1 if the 2 overlap
        @detail The idea is that is r1 and r2 overlap, r2 will be pushed to the closest point
        @return How the collision was resolved
    */
    constexpr static collision_direction resolve_collision(const Rectangle& r1, Rectangle& r2)
    {
        // Magic stuff 
        T lx = r2._width + r2._x - r1._x;
        T by = r2._height + r2._y - r1._y;
        T rx = r1._x + r1._width - r2._x;
        T ty = r1._y + r1._height - r2._y;
        
        // now I need to pick the min value from all thoses 4 values
        
        if (lx < rx && lx < ty && lx < by)
        {
            r2._x = r1._x - r2._width;
            return collision_left;
        }
        
        if (rx < lx && rx < ty && rx < by)
        {
            r2._x = r1._x + r1._width;
            return collision_right;
        }
        
        if (ty < lx && ty < rx && ty < by)
        {
            r2._y = r1._y + r1._height;
            return collision_top;
        }
        
        if (by < lx && by < rx && by < ty)
        {
            r2.y = r1.y - r2.height;
            return collision_bottom;
        }
        return collision_none;    
    }

//Constructors 

    constexpr Rectangle(T x = T(), T y = T(), T width = T(), T height = T())
        :_x(x), _y(y), _width(width), _height(height)
    {

    }

    constexpr Rectangle(const  corgi::detail::Vec2<T>& position, const  corgi::detail::Vec2<T>& size)
        :_x(position.get_x()), _y(position.get_y()), _width(size.get_x()), _height(size.get_y())
    {
                
    }

// Methods

    /*!
        @brief  Checks if a point is inside the current rectangle
        @param  point_x x coordinate of the point
        @param  point_y coordinate of the point
        @return Returns true if the point is inside the current rectangle, false otherwise
    */
    constexpr bool contains(T point_x, T point_y)
    {
        if (point_x >= _x && point_x<= (_x + _width))
        {
            return (point_y >= _y && point_y <= (_y + _height));
        }
        return false;
    }

    /*!
        @brief  Checks if @ref point is inside the current rectangle
        @param  point Tested point
        @return Returns true if the point is inside the current rectangle, false otherwise
    */
    constexpr bool contains(const corgi::detail::Vec2<T>& point)
    {
        return contains(point.x(), point.y());
    }

    /*!
    @brief      Check if the [A,B] segment intersect with the rectangle
    @details

    @param[out] intersetion  The intersection point between a1 and a2
    @returns    Returns true if the segment intersect with the rectangle and
    store the intersection point in the intersection parameter
    */
    constexpr bool intersect(const corgi::detail::Vec2<T>& a1, const  corgi::detail::Vec2<T>& a2,  corgi::detail::Vec2<T>& intersection)const
    {
        corgi::detail::Vec2<T> v1;
        corgi::detail::Vec2<T> v2;
        corgi::detail::Vec2<T> v3;
        corgi::detail::Vec2<T> v4;

        int i1 = 0;
        int i2 = 0;
        int i3 = 0;
        int i4 = 0;

        i1 += segment_intersect(corgi::detail::Vec2<T>(_x, _y),            corgi::detail::Vec2<T>(_x + _width, _y), a1, a2, v1);
        i2 += segment_intersect(corgi::detail::Vec2<T>(_x, _y),            corgi::detail::Vec2<T>(_x, _y + _height), a1, a2, v2);
        i3 += segment_intersect(corgi::detail::Vec2<T>(_x, _y + _height),  corgi::detail::Vec2<T>(_x + _width, _y + _height), a1, a2, v3);
        i4 += segment_intersect(corgi::detail::Vec2<T>(_x + _width, _y),   corgi::detail::Vec2<T>(_x + _width, _y + _height), a1, a2, v4);

        int i = i1 + i2 + i3 + i4;

        T distance = std::numeric_limits<T>::max();
        T dlol = 0.0f;

        if (i1)
        {
            distance = (v1 - a1).length();
            intersection = v1;
        }

        if (i2)
        {
            dlol = (v2 - a1).length();
            if (dlol < distance)
            {
                distance = dlol;
                intersection = v2;
            }
        }

        if (i3)
        {
            dlol = (v3 - a1).length();
            if (dlol < distance)
            {
                distance = dlol;
                intersection = v3;
            }
        }

        if (i4)
        {
            dlol = (v4 - a1).length();
            if (dlol < distance)
            {
                distance = dlol;
                intersection = v4;
            }
        }
        return (i >0);
    }

    /*!
    @brief  Returns true if the current rectangle intersect this @ref r
    */
    constexpr bool intersect(const Rectangle& r)
    {
        return !(
            (r._x >= _x + _width) ||
            (r._x + r._width <= _x) ||
            (r._y >= _y + _height) ||
            (r._y + r._height <= _y)
            );
    }


 //Getters

    constexpr T get_x()const
    {
        return _x;
    }

    constexpr T get_y()const
    {
        return _y;
    }

    constexpr T get_width()const
    {
        return _width;
    }

    constexpr T get_height()const
    {
        return _height;
    }

// Setters

    void set_x(T x)
    {
        _x = x;
    }

    void set_y(T y)
    {
        _y = y;
    }

    void set_width(T width)
    {
        _width = width;
    }

    void set_height(T height)
    {
        _height = height;
    }

    void set(T x, T y, T width, T height)
    {
        _x = x;
        _y = y;
        _width = width;
        _height = height;
    }

private:

    T _x;
    T _y;
    T _width;
    T _height;
};
 }}}