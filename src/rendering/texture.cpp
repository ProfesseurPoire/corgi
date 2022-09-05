#include <corgi/filesystem/FileSystem.h>
#include <corgi/logger/log.h>
#include <corgi/rendering/RenderCommand.h>
#include <corgi/rendering/texture.h>
#include <corgi/resources/image.h>
#include <corgi/utils/Utils.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/rapidjson.h>

#include <map>

using namespace corgi;

// static Texture::MagFilter parse_mag_filter(const std::string& str)
// {
//     static std::map<std::string, Texture::MagFilter> mag_filters = {
//         {"nearest", Texture::MagFilter::Nearest}, {"linear", Texture::MagFilter::Linear}};
//     return mag_filters.at(str);
// }

// static Texture::MinFilter parse_min_filter(const std::string& str)
// {
//     static std::map<std::string, Texture::MinFilter> min_filters_ = {
//         {"nearest", Texture::MinFilter::Nearest},
//         {"linear", Texture::MinFilter::Linear},
//         {"nearest_mipmap_nearest", Texture::MinFilter::NearestMipmapNearest},
//         {"nearest_mipmap_linear", Texture::MinFilter::NearestMipmapLinear},
//         {"linear_mipmap_linear", Texture::MinFilter::LinearMipmapLinear},
//         {"linear_mipmap_nearest", Texture::MinFilter::LinearMipmapNearest}};
//     return min_filters_.at(str);
// }

// static Texture::Wrap load_wrap(const std::string& str)
// {
//     static std::map<std::string, Texture::Wrap> wraps = {
//         {"repeat", Texture::Wrap::Repeat},
//         {"clamp_to_border", Texture::Wrap::ClampToBorder},
//         {"clamp_to_edge", Texture::Wrap::ClampToEdge},
//         {"mirrored_repeat", Texture::Wrap::MirroredRepeat},
//         {"mirror_clamp_to_edge", Texture::Wrap::MirrorClampToEdge}};
//     return wraps.at(str);
// }

Texture::Texture()
{
    log_info("Creating new empty texture");
}

Texture::Texture(const std::string& path, const std::string& relative_path)
    : name_(relative_path.c_str())
{
    auto size = filesystem::size(path.c_str());

#ifdef __linux__
    FILE* fp = fopen(path.c_str(), "r");    // non-Windows use "r"
#else
    FILE* fp = fopen(path.c_str(), "rb");    // non-Windows use "r"
#endif

    std::vector<char> readBuffer(static_cast<unsigned int>(size));

    rapidjson::FileReadStream is(fp, readBuffer.data(), static_cast<unsigned int>(size));

    rapidjson::Document document;
    document.ParseStream(is);

    assert(document.HasMember("wrap_s"));
    assert(document.HasMember("wrap_t"));
    assert(document.HasMember("min_filter"));
    assert(document.HasMember("mag_filter"));

    // help
    std::string pp       = path.c_str();
    auto        img_path = pp.substr(0, pp.size() - 4) + ".img";

    std::ifstream corgi_image(img_path, std::ifstream::binary | std::ifstream::in);

    int w;
    int h;
    int channels;

    corgi_image.read(reinterpret_cast<char*>(&w), sizeof w);
    corgi_image.read(reinterpret_cast<char*>(&h), sizeof h);
    corgi_image.read(reinterpret_cast<char*>(&channels), sizeof channels);

    unsigned char* pixels = new unsigned char[w * h * 4];

    corgi_image.read(reinterpret_cast<char*>(pixels), w * h * 4);

    _width  = w;
    _height = h;

    // min_filter_ = parse_min_filter(document["min_filter"].GetString());
    // mag_filter_ = parse_mag_filter(document["mag_filter"].GetString());
    // wrap_s_     = load_wrap(document["wrap_s"].GetString());
    // wrap_t_     = load_wrap(document["wrap_t"].GetString());

    //log_info("Texture Constructor for "+path);

    id_ = RenderCommand::generate_texture_object();

    RenderCommand::bind_texture_object(id_);

    // RenderCommand::texture_parameter(min_filter_);
    // RenderCommand::texture_parameter(mag_filter_);
    // RenderCommand::texture_wrap_s(wrap_s_);
    // RenderCommand::texture_wrap_t(wrap_t_);

    // if (channels == 3)
    // {
    // 	RenderCommand::initialize_texture_object
    // 	(
    // 		Format::RGB, InternalFormat::RGB, _width, _height,
    // 		DataType::UnsignedByte, pixels
    // 	);
    // }

    // if (channels == 4)
    // {
    RenderCommand::initialize_texture_object(Format::RGBA, InternalFormat::RGBA, _width,
                                             _height, DataType::UnsignedByte, pixels);
    //}

    RenderCommand::end_texture();
    fclose(fp);
    delete[] pixels;
}

Texture::Texture(Texture&& texture) noexcept
    : name_(std::move(texture.name_))
    , id_(texture.id_)
    // , min_filter_(texture.min_filter_)
    // , mag_filter_(texture.mag_filter_)
    // , wrap_s_(texture.wrap_s_)
    // , wrap_t_(texture.wrap_t_)
    , _width(texture._width)
    , _height(texture._height)
{
    //log_info("Texture Move Constructor for "+ name_);

    texture.id_     = 0u;
    texture._width  = static_cast<unsigned short>(0);
    texture._height = static_cast<unsigned short>(0);
    // texture.min_filter_ = MinFilter::Nearest;
    // texture.mag_filter_ = MagFilter::Nearest;
    // texture.wrap_s_     = Wrap::Repeat;
    // texture.wrap_t_     = Wrap::Repeat;
}

Texture& Texture::operator=(Texture&& texture) noexcept
{
    //log_info("Move Affectation texture for "+ name_);

    if(id_ != 0)
        RenderCommand::delete_texture_object(id_);

    name_ = std::move(texture.name_);
    id_   = texture.id_;
    // min_filter_ = texture.min_filter_;
    // mag_filter_ = texture.mag_filter_;
    // wrap_s_     = texture.wrap_s_;
    // wrap_t_     = texture.wrap_t_;
    _width  = texture._width;
    _height = texture._height;

    texture.id_     = 0u;
    texture._width  = static_cast<unsigned short>(0);
    texture._height = static_cast<unsigned short>(0);
    // texture.min_filter_ = MinFilter::Nearest;
    // texture.mag_filter_ = MagFilter::Nearest;
    // texture.wrap_s_     = Wrap::Repeat;
    // texture.wrap_t_     = Wrap::Repeat;

    return *this;
}

const char* Texture::name() const
{
    return name_.c_str();
}

Texture::Texture(const std::string& name,
                 unsigned           width,
                 unsigned           height,
                 //  MinFilter          min_f,
                 //  MagFilter          mag_f,
                 //  Wrap               wrap_s,
                 //  Wrap               wrap_t,
                 Format         format,
                 InternalFormat internal_format,
                 DataType       dt,
                 unsigned char* data)
    : name_(name)
    , id_(RenderCommand::generate_texture_object())
    , _width(static_cast<unsigned short>(width))
    , _height(static_cast<unsigned short>(height))
{
    //log_info("Texture Constructor for "+name);

    RenderCommand::bind_texture_object(id_);
    RenderCommand::initialize_texture_object(format, internal_format, width, height, dt,
                                             data);

    RenderCommand::end_texture();
}

Texture::~Texture()
{
    //log_info("Texture Destructor for "+name_);
    RenderCommand::delete_texture_object(id_);
}

bool Texture::operator==(const Texture& other) const noexcept
{
    if(name_ != other.name())
        return false;

    // I wonder if the id is enough here?
    if(id_ != other.id_)
        return false;

    if(_width != other.width())
        return false;

    if(_height != other.height())
        return false;

    return true;
}

bool Texture::operator<(const Texture& other) const noexcept
{
    // Maybe the id is enough to compare textures
    if(id_ != other.id_)
        return id_ < other.id_;

    if(name_ != other.name_)
        return name_ < other.name_;

    if(_width != other._width)
        return _width < other._width;

    if(_height != other._height)
        return _height < other._height;

    return true;
}

long long Texture::memory_usage() const
{
    return sizeof(Texture) + _width * _height * 4;
}

void Texture::apply_changes()
{
    if(id_ == 0)
        log_warning("Could not apply texture's changes");

    RenderCommand::bind_texture_object(id_);
    RenderCommand::end_texture();
}

unsigned Texture::id() const noexcept
{
    return id_;
}

unsigned Texture::width() const noexcept
{
    return _width;
}

unsigned Texture::height() const noexcept
{
    return _height;
}

void Texture::width(unsigned width) noexcept
{
    _width = width;
}

void Texture::height(unsigned height) noexcept
{
    _height = height;
}

//
//
//static corgi::containers::std::vector<std::string> wraps
//{
//    "clamp_to_border",
//    "clamp_to_edge",
//    "mirrored_repeat",
//    "mirror_clamp_to_edge",
//    "repeat"
//};
//
//static Vector<std::string> min_filters
//{
//    "nearest",
//    "linear",
//    "nearest_mipmap_nearest",
//    "nearest_mipmap_linear",
//    "linear_mipmap_linear",
//    "linear_mipmap_nearest"
//};
//
//static Vector<std::string> mag_filters
//{
//    "nearest",
//    "linear"
//};
//
//// If str exist inside strings, returns its index
//int get_index(Vector<std::string>& strings, const std::string& str)
//{
//    const auto iterator = std::find(strings.begin(), strings.end(), str);
//
//    if (iterator == strings.end())
//    {
//       // log_warning("Could not find" + str + " index inside the Vector");
//        return 0;
//    }
//    return std::distance(strings.begin(), iterator);
//}
//
//static std::string min_filter_to_string(Texture::MinFilter filter)
//{
//    return min_filters[static_cast<int>(filter)];
//}
//
//static Texture::MinFilter string_to_min_filter(const std::string& str)
//{
//    return static_cast<Texture::MinFilter>(get_index(min_filters, str));
//}
//
//static std::string mag_filter_to_string(Texture::MagFilter filter)
//{
//    return mag_filters[static_cast<int>(filter)];
//}
//
//static Texture::MagFilter string_to_mag_filter(const std::string& str)
//{
//    return static_cast<Texture::MagFilter>(get_index(mag_filters, str));
//}
//
//static std::string wrap_to_string(Texture::Wrap wrp)
//{
//    return wraps[static_cast<int>(wrp)];
//}
//
//static Texture::Wrap string_to_wrap(const std::string& str)
//{
//    return static_cast<Texture::Wrap>(get_index(wraps, str));
//}
//
//void Texture::load( const std::string& file)
//{
//    //_name = file;
//
//    //std::string path = ResourceFolder::instance()->folder() + "/images/" + file;
//
//    //// We also try to load the .asset file. If it doesn't exist, it gets
//    //// created
//
//    //filesystem::ini::Document doc;
//    //ConstString asset_path = path + ".asset";
//
//    //if (filesystem::file_exist(asset_path))
//    //{
//    //    doc.read(asset_path);
//
//    //    _wrap_s = string_to_wrap(doc.value("wrap_s", "wrap").as_string());
//    //    _wrap_t = string_to_wrap(doc.value("wrap_t", "wrap").as_string());
//    //    _min_filter = string_to_min_filter(doc.value("min_filter", "filters").as_string());
//    //    _mag_filter = string_to_mag_filter(doc.value("mag_filter", "filters").as_string());
//
//    //}
//    //else  // No asset file as been founded, default one created
//    //{
//    //    doc.set_value(wrap_to_string(_wrap_s), "wrap_s", "wrap");
//    //    doc.set_value(wrap_to_string(_wrap_t), "wrap_t", "wrap");
//    //    doc.set_value(min_filter_to_string(_min_filter), "min_filter", "filters");
//    //    doc.set_value(mag_filter_to_string(_mag_filter), "mag_filter", "filters");
//
//    //    doc.write(asset_path);
//    //}
//
//    //Image img;
//    //img.load_from_png_file(path.c_str());
//    //image(img);
//}

float Texture::ratio() const
{
    return static_cast<float>(_width) / static_cast<float>(_height);
}

void Texture::unpack_pixels()
{
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

// void Texture::build_texture(int width, int height)
// {
//    /* _width = width;
// 	_height = height;

// 	glTexImage2D
// 	(
// 		GL_TEXTURE_2D,
// 		0,
// 		GL_RGBA,
// 		_width,
// 		_height,
// 		0,
// 		GL_RGBA,
// 		GL_UNSIGNED_BYTE,
// 		0
// 	);
// 	check_gl_error();*/
// }
