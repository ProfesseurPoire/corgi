#pragma once

namespace corgi
{
    using Byte = unsigned char;

    class Image
    {
    public:
        
        struct Pixel
        {
            Byte r, g, b, a;
        };

        enum class Format
        {
            RGB_888,
            RGBA_8888
        };
        
    public:

        // Creates an empty Image with the parameters given
        Image(int width, int height, int channel);
        Image(int width, int height, int channel, Byte* data);
        Image(const char* img);

        ~Image();
        
        Byte* pixels()const;

        int width()const;
        int height()const;
        int channel()const;

        void fill(Byte red, Byte green, Byte blue);
        void fill(Byte red, Byte green, Byte blue, Byte alpha);

        void pixel(Byte red, Byte green, Byte blue, Byte alpha, int x, int y);
        void pixel(Byte red, Byte green, Byte blue, int x, int y);

        // No boundary check is made
        Pixel pixel(int x, int y) const;

    private:

        int _width, _height, _channel;

        Byte* _pixels = nullptr; // 16 bytes
    };
}