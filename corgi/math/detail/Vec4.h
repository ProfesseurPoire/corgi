#pragma once

namespace corgi
{
    namespace detail
    {
        template<class ValueType>
        class Vec4
        {
            //static_assert(std::is_arithmetic<ValueType>(),
            //"Template argument is not an arithmetic type");

        public:
            explicit constexpr Vec4() = default;
            explicit constexpr Vec4(ValueType p_x, ValueType p_y, ValueType p_z, ValueType p_w)
                : x(p_x)
                , y(p_y)
                , z(p_z)
                , w(p_w)
            {
            }

            [[nodiscard]] constexpr ValueType sqrt_length() const noexcept
            {
                return (x * x) + (y * y) + (z * z) + (w * w);
            }

            [[nodiscard]] bool operator==(const Vec4& other) const noexcept
            {
                return (x == other.x && y == other.y && z == other.z && w == other.w);
            }

            [[nodiscard]] bool operator!=(const Vec4& other) const noexcept { return !(*this == other); }

            /*!
			 * @brief lexicographical  comparison for sorting only
			 */
            int lexicographical_comp(const Vec4& other) const noexcept
            {
                if(x < other.x)
                    return -1;

                if(x > other.x)
                    return 1;

                if(y < other.y)
                    return -1;

                if(y > other.y)
                    return 1;

                if(z < other.z)
                    return -1;

                if(z > other.z)
                    return 1;

                if(w < other.w)
                    return -1;

                if(w > other.w)
                    return 1;

                return 0;
            }

            [[nodiscard]] Vec4 operator*(const Vec4& v) const noexcept
            {
                return Vec4(x * v.x, y * v.y, z * v.z, w * v.w);
            }

            bool operator<(const Vec4& other) const noexcept { return sqrt_length() < other.sqrt_length(); }

            [[nodiscard]] Vec4 operator-(const Vec4& v) const noexcept
            {
                return Vec4(x - v.x, y - v.y, z - v.z, w - v.w);
            }

            [[nodiscard]] Vec4 operator+(const Vec4& v) const noexcept
            {
                return Vec4(x + v.x, y + v.y, z + v.z, w + v.w);
            }

            void set(ValueType x, ValueType y, ValueType z, ValueType w)
            {
                this->x = x;
                this->y = y;
                this->z = z;
                this->w = w;
            }

            ValueType x = 0;
            ValueType y = 0;
            ValueType z = 0;
            ValueType w = 0;
        };
    }    // namespace detail
}    // namespace corgi
