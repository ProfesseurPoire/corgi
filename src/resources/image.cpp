#include "config.h"

#include <corgi/logger/log.h>
#include <corgi/resources/image.h>

#include <fstream>

namespace corgi
{
Image::Image(const char* img)
{
    std::ifstream corgi_image(img, std::ifstream::binary | std::ifstream::in);

    corgi_image.read(reinterpret_cast<char*>(&width_), sizeof width_);
    corgi_image.read(reinterpret_cast<char*>(&height_), sizeof height_);
    corgi_image.read(reinterpret_cast<char*>(&channels_), sizeof channels_);

    pixels_ = new unsigned char[width_ * height_ * 4];

    corgi_image.read(reinterpret_cast<char*>(pixels_), width_ * height_ * 4);
}

Image::~Image()
{
    delete[] pixels_;
}

Image::Image(int width, int height, int channel, Byte* data)
    : width_(width)
    , height_(height)
    , channels_(channel)
    , pixels_(data)
{
}

Image::Image(int width, int height, int channel)
    : width_(width)
    , height_(height)
    , channels_(channel)
{
    pixels_ = new Byte[width * height * channel];
}

int Image::width() const
{
    return width_;
}

int Image::height() const
{
    return height_;
}

void Image::fill(Byte red, Byte green, Byte blue)
{
    if(channels_ != 3)
    {
        log_error("Wrong Image Format");
        return;
    }

    for(int i = 0; i < width_; ++i)
    {
        for(int j = 0; j < height_; j++)
        {
            pixels_[i * width_ * channels_ + j * channels_]     = red;
            pixels_[i * width_ * channels_ + j * channels_ + 1] = green;
            pixels_[i * width_ * channels_ + j * channels_ + 2] = blue;
        }
    }
}

void Image::fill(Byte red, Byte green, Byte blue, Byte alpha)
{
    if(channels_ != 4)
    {
        log_error("Wrong Image Format");
        return;
    }

    for(int i = 0; i < width_; ++i)
    {
        for(int j = 0; j < height_; j++)
        {
            pixels_[i * width_ * channels_ + j * channels_]     = red;
            pixels_[i * width_ * channels_ + j * channels_ + 1] = green;
            pixels_[i * width_ * channels_ + j * channels_ + 2] = blue;
            pixels_[i * width_ * channels_ + j * channels_ + 3] = alpha;
        }
    }
}

void Image::pixel(Byte red, Byte green, Byte blue, Byte alpha, int x, int y)
{
#ifdef _DEBUG
    if(channels_ != 4)
    {
        log_error("Image channel doesn't correspond pixel function");
        return;
    }
#endif

    pixels_[y * (width_ * channels_) + x * channels_]     = red;
    pixels_[y * (width_ * channels_) + x * channels_ + 1] = green;
    pixels_[y * (width_ * channels_) + x * channels_ + 2] = blue;
    pixels_[y * (width_ * channels_) + x * channels_ + 3] = alpha;
}

void Image::pixel(Byte red, Byte green, Byte blue, int x, int y)
{
#ifdef _DEBUG
    if(channels_ != 3)
    {
        log_error("Image channel doesn't correspond pixel function");
        return;
    }
#endif

    pixels_[y * (width_ * channels_) + x * channels_]     = red;
    pixels_[y * (width_ * channels_) + x * channels_ + 1] = green;
    pixels_[y * (width_ * channels_) + x * channels_ + 2] = blue;
}

// No boundary check is made
Image::Pixel Image::pixel(int x, int y) const
{
    if(channels_ == 3)
    {
        return {pixels_[y * (width_ * channels_) + x * channels_],
                pixels_[y * (width_ * channels_) + x * channels_ + 1],
                pixels_[y * (width_ * channels_) + x * channels_ + 2], 0};
    }
    if(channels_ == 4)
    {
        return {pixels_[y * (width_ * channels_) + x * channels_],
                pixels_[y * (width_ * channels_) + x * channels_ + 1],
                pixels_[y * (width_ * channels_) + x * channels_ + 2],
                pixels_[y * (width_ * channels_) + x * channels_ + 3]};
    }
    return {255, 255, 255, 255};
}

Byte* Image::pixels() const
{
    return pixels_;
}

int Image::channel() const
{
    return channels_;
}
}    // namespace corgi