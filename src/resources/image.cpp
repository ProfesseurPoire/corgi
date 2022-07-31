#include <corgi/resources/image.h>

#include <corgi/logger/log.h>

#include <fstream>

#include "config.h"

namespace corgi
{
    Image::Image(const char* img)
    {
		std::string f= OUTPUT_DIRECTORY;

		f += "/resources/";
		f +=img;
		f += ".dat";

        std::ifstream file(f, std::ifstream::binary);

        if (!file.is_open())
        {
			//+ std::string(img)
            log_error("Could not load image ");
        }

        char buffer[sizeof(int)];

        file.read(buffer, sizeof(int));
        _width = *(int*) buffer;

        file.read(buffer, sizeof(int));
        _height = *(int*) buffer;

        file.read(buffer, sizeof(int));
        _channel = *(int*) buffer;

        _pixels = new unsigned char[_width*_height*_channel];
		int size = _width * _height * _channel;
        file.read(reinterpret_cast<char*>(_pixels), static_cast<long long>(size));
    }

    Image::~Image()
    {
        if (_pixels != nullptr)
        {
            delete _pixels; 
        }
    }

    Image::Image(int width, int height, int channel, Byte* data)
        :_width(width), _height(height), _channel(channel), _pixels(data){}

    Image::Image(int width, int height, int channel) :
         _width(width), _height(height),_channel(channel)
    {
        _pixels = new Byte[width*height*channel];
    }

    int Image::width()const
    {
        return _width;
    }

    int Image::height()const
    {
        return _height;
    }

    void Image::fill(Byte red, Byte green, Byte blue)
    {
        if (_channel!=3)
        {
            log_error("Wrong Image Format");
            return;
        }

        for (int i = 0; i < _width; ++i)
        {
            for (int j = 0; j < _height; j++)
            {
                _pixels[i*_width * _channel + j * _channel] = red;
                _pixels[i*_width * _channel + j * _channel+1] = green;
                _pixels[i*_width * _channel + j * _channel+2] = blue;
            }
        }
    }

    void Image::fill(Byte red, Byte green, Byte blue, Byte alpha)
    {
        if (_channel != 4)
        {
            log_error("Wrong Image Format");
            return;
        }

        for (int i = 0; i < _width; ++i)
        {
            for (int j = 0; j < _height; j++)
            {
                _pixels[i*_width * _channel + j * _channel] = red;
                _pixels[i*_width * _channel + j * _channel + 1] = green;
                _pixels[i*_width * _channel + j * _channel + 2] = blue;
                _pixels[i*_width * _channel + j * _channel + 3] = alpha;
            }
        }
    }

    void Image::pixel(Byte red, Byte green, Byte blue, Byte alpha, int x, int y)
    {
        #ifdef _DEBUG
        if (_channel != 4)
        {
            log_error("Image channel doesn't correspond pixel function");
            return;
        }
        #endif 

        _pixels[y*(_width * _channel) + x * _channel]       = red;
        _pixels[y*(_width * _channel) + x * _channel + 1]   = green;
        _pixels[y*(_width * _channel) + x * _channel + 2]   = blue;
        _pixels[y*(_width * _channel) + x * _channel + 3]   = alpha;
    }

    void Image::pixel(Byte red, Byte green, Byte blue, int x, int y)
    {
        #ifdef _DEBUG
        if (_channel != 3)
        {
            log_error("Image channel doesn't correspond pixel function");
            return;
        }
        #endif 

        _pixels[y*(_width * _channel) + x * _channel]   = red;
        _pixels[y*(_width * _channel) + x * _channel+1] = green;
        _pixels[y*(_width * _channel) + x * _channel+2] = blue;
    }

    // No boundary check is made
    Image::Pixel Image::pixel(int x, int y) const
    {
        if (_channel == 3)
        {
            return
            {
                _pixels[y*(_width * _channel) + x * _channel],
                _pixels[y*(_width * _channel) + x * _channel+1],
                _pixels[y*(_width * _channel) + x * _channel+2],
                0
            };
        }
        if (_channel == 4)
        {
            return
            {
                _pixels[y*(_width * _channel) + x * _channel],
                _pixels[y*(_width * _channel) + x * _channel+1],
                _pixels[y*(_width * _channel) + x * _channel+2],
                _pixels[y*(_width * _channel) + x * _channel+3]
            };
        }
		return { 255,255,255,255 };
    }

    Byte* Image::pixels()const
    {
        return  _pixels;
    }

    int Image::channel()const
    {
        return _channel;
    }
}