#pragma once

namespace corgi
{
    /**
     * @brief   Represents a RGBA color
     * 
     *          A color is divided in 4 components : 
     *              * red
     *              * green
     *              * blue  
     *              * alpha
     *
     *          The class is made in a way that you can either set the intensity
     *          as an int or a float.
     * 
     *          As an int, values must range between 0 and 255
     *          As a float, values must range between 0.0f and 1.0f
     *
     *          Values will be clamped to the ranges. So if you try to
     *          sets the red component to 300, it will be clamped to 255.
     *
     *          The component member variable are stored as int
     */
    class Color
    {
    public:
        /**
         * @brief   Uses fuchsia color by default
         * 
         *          The idea is to make it obvious that a color has not been set
         */
        Color() noexcept = default;

        /**
         * @brief   Constructs a new color object
         *
         *          Values for each color component are clamped on the 0-255 range 
         */
        explicit Color(int r, int g, int b, int a = 255) noexcept;

        /**
         * @brief   Constructs a new color object
         *          
         *          Values for each color component are clamped between 0.0f and 1.0f
         *          Actually, the value is then converted to the 0-255 integer range 
         */
        explicit Color(float r, float g, float b, float a = 1.0f) noexcept;

        /**
         * @brief   Sets the color's red component intensity
         * 
         * @param red : Value clamped on the [0,255] range
         */
        void setRed(int red) noexcept;

        /**
         * @brief   Sets the color's red component intensity
         * 
         * @param red : Value clamped on the [0.0f, 1.0f] range
         */
        void setRed(float red) noexcept;

        /**
         * @brief   Sets the color's green component intensity
         * 
         * @param green Value clamped on the [0,255] range
         */
        void setGreen(int green) noexcept;

        /**
         * @brief   Sets the color's green component intensity
         * 
         * @param green Value clamped on the [0.0f,1.0f] range
         */
        void setGreen(float green) noexcept;

        /**
         * @brief   Sets the color's blue component intensity
         * 
         * @param blue Value clamped on the [0,255] range
         */
        void setBlue(int blue) noexcept;

        /**
         * @brief Sets the color's blue component intensity
         * 
         * @param blue Value clamped on the [0.0f,1.0f] range
         */
        void setBlue(float blue) noexcept;

        /**
         * @brief Sets the color's alpha component intensity
         * 
         * @param alpha Value clamped on the [0,255] range
         */
        void setAlpha(int alpha) noexcept;

        /**
         * @brief Sets the the color's alpha component intensity
         * 
         * @param alpha Value clamped on the [0.0f,1.0f] range
         */
        void setAlpha(float alpha) noexcept;

        /**
         * @brief   Gets the red component's intensity
         * @return  Return value will range between 0.0f and 1.0f
         */
        [[nodiscard]] float getRed() const noexcept;

        /**
         * @brief   Gets the red component's intensity
         * @return  Return value will range between 0 and 255
         */
        [[nodiscard]] int getRedInt() const noexcept;

        /**
         * @brief   Gets the green component's intensity
         * @return  Return value will range between 0.0f and 1.0f
         */
        [[nodiscard]] float getGreen() const noexcept;

        /**
         * @brief   Gets the green component's intensity
         * @return  Return value will range between 0 and 255
         */
        [[nodiscard]] int getGreenInt() const noexcept;

        /**
         * @brief   Gets the blue component's intensity
         * @return  Return value will range between 0.0f and 1.0f
         */
        [[nodiscard]] float getBlue() const noexcept;

        /**
         * @brief   Gets the blue component's intensity
         * @return  Return value will range between 0 and 255
         */
        [[nodiscard]] int getBlueInt() const noexcept;

        /**
         * @brief   Gets the alpha component's intensity
         * @return  Return value will range between 0.0f and 1.0f
         */
        [[nodiscard]] float getAlpha() const noexcept;

        /**
         * @brief   Gets the alpha component's intensity
         * @return  Return value will range between 0 and 255
         */
        [[nodiscard]] int getAlphaInt() const noexcept;

        /**
         * @brief   Comparison operator
         *  
         *          Returns true if both colors are equals
         * 
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool operator==(const Color& rhs) const noexcept;

        /**
         * @brief Substract 2 colors
         * 
         * @param rhs 
         * @return Color 
         */
        [[nodiscard]] Color operator-(const Color& rhs) const noexcept;

        /**
         * @brief   Add up 2 colors
         * 
         * @param rhs 
         * @return Color 
         */
        [[nodiscard]] Color operator+(const Color& rhs) const noexcept;

    private:
        // Using ints because it makes it easier to compare colors
        // and most color are encoded on 32 bits anyways (8 bits for each
        // color component)
        int red_   = 255;
        int green_ = 0;
        int blue_  = 255;
        int alpha_ = 255;
    };
}    // namespace corgi
