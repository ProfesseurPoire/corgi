#pragma once

namespace corgi
{
class AbstractTexture
{
public:
    enum class Format : char
    {
        RED,
        RG,
        RGB,
        BGR,
        RGBA,
        BGRA,
        RED_INTEGER,
        RG_INTEGER,
        RGB_INTEGER,
        BGR_INTEGER,
        RGBA_INTEGER,
        BGRA_INTEGER,
        STENCIL_INDEX,
        DEPTH_COMPONENT,
        DEPTH_STENCIL
    };

    // TODO : More exist
    enum class InternalFormat : char
    {
        DEPTH_COMPONENT,
        DEPTH_STENCIL,
        RED,
        RG,
        RGB,
        RGBA,
        R8,
        R16,
        RG8,
        RG16,
        RG32F,
        RG32I,
        RG32UI,
        DEPTH24_STENCIL8
    };

    enum class DataType : char
    {
        UnsignedByte,
        Byte,
        UnsignedShort,
        Short,
        UnsignedInt,
        Int,
        HalfFloat,
        Float,
        UnsignedInt24_8
    };

    struct CreateInfo
    {
        int             width;
        int             height;
        Format          format;
        InternalFormat  internal_format;
        DataType        data_type;
        void*           data;
    };

private:
};
}    // namespace corgi