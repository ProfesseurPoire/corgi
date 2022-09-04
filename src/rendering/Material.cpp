#include <corgi/filesystem/FileSystem.h>
#include <corgi/rendering/Material.h>
#include <corgi/rendering/RenderCommand.h>
#include <corgi/resources/Shader.h>
#include <corgi/utils/ResourcesCache.h>
#include <glad/glad.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/rapidjson.h>

#include <fstream>

namespace corgi
{
void Material::set_uniform(const std::string& name, int value)
{
    for(auto& v : _uniforms)
    {
        if(v.name == name)
        {
            v.data.int_value = value;
            return;
        }
    }
    _uniforms.emplace_back(name, glGetUniformLocation(shader_program->id(), name.c_str()),
                           value);
}

void Material::set_uniform(const std::string& name, Matrix value)
{
    for(auto& uniform : _uniforms)
    {
        if(uniform.name == name)
        {
            uniform.data.matrix = value;
            return;
        }
    }
    _uniforms.emplace_back(name, glGetUniformLocation(shader_program->id(), name.c_str()),
                           value);
}

void Material::set_uniform(const char* name, int value)
{
    for(auto& v : _uniforms)
    {
        if(strcmp(v.name, name) == 0)
        {
            v.data.int_value = value;
            return;
        }
    }
    _uniforms.emplace_back(name, glGetUniformLocation(shader_program->id(), name), value);
}

void Material::set_uniform(const char* name, unsigned value)
{
    for(auto& v : _uniforms)
    {
        if(strcmp(v.name, name) == 0)
        {
            v.data._unsigned_value = value;
            return;
        }
    }
    auto a = Uniform(name, glGetUniformLocation(shader_program->id(), name), value, 1);
    _uniforms.emplace_back(a);
}

void Material::set_uniform(int index, const std::string& name, int value)
{
    if(_uniforms.size() < index)
    {
        _uniforms.resize(index);
        _uniforms[index] = Uniform(
            name, glGetUniformLocation(shader_program->id(), name.c_str()), value);
        return;
    }
    _uniforms[index].data.int_value = value;
}

void Material::set_uniform(const std::string& name, float value)
{
    for(auto& v : _uniforms)
    {
        if(v.name == name)
        {
            v.data.value.x = value;
            return;
        }
    }
    _uniforms.emplace_back(name, glGetUniformLocation(shader_program->id(), name.c_str()),
                           value);
}

void Material::set_uniform(const char* name, float value)
{
    for(auto& v : _uniforms)
    {
        if(strcmp(v.name, name) == 0)
        {
            v.data.value.x = value;
            return;
        }
    }
    _uniforms.emplace_back(name, glGetUniformLocation(shader_program->id(), name), value);
}

void Material::set_uniform(int index, const std::string& name, float value)
{
    if(_uniforms.size() < index)
    {
        _uniforms.resize(index);
        _uniforms[index] = Uniform(
            name, glGetUniformLocation(shader_program->id(), name.c_str()), value);
        return;
    }
    _uniforms[index].data.value.x = value;
}

void Material::set_uniform(const std::string& name, Vec2 value)
{
    for(auto& v : _uniforms)
    {
        if(v.name == name)
        {
            v.data.value.x = value.x;
            v.data.value.y = value.y;
            return;
        }
    }
    _uniforms.emplace_back(name, glGetUniformLocation(shader_program->id(), name.c_str()),
                           value);
}

void Material::set_uniform(const std::string& name, float x, float y)
{
    set_uniform(name, Vec2(x, y));
}

void Material::set_uniform(int index, const std::string& name, float x, float y)
{
    if(_uniforms.size() < index)
    {
        _uniforms.resize(index);
        _uniforms[index] = Uniform(
            name, glGetUniformLocation(shader_program->id(), name.c_str()), Vec2(x, y));
        return;
    }
    _uniforms[index].data.value.x = x;
    _uniforms[index].data.value.y = y;
}

void Material::set_uniform(const std::string& name, Vec3 value)
{
    for(auto& v : _uniforms)
    {
        if(v.name == name)
        {
            v.data.value.x = value.x;
            v.data.value.y = value.y;
            v.data.value.z = value.z;
            return;
        }
    }
    _uniforms.emplace_back(name, glGetUniformLocation(shader_program->id(), name.c_str()),
                           value);
}

void Material::set_uniform(const std::string& name, float x, float y, float z)
{
    set_uniform(name, Vec3(x, y, z));
}

void Material::set_uniform(const std::string& name, const Color& color)
{
    for(auto& v : _uniforms)
    {
        if(strcmp(v.name, name.c_str()) == 0)
        {
            v.data.value.x = color.getRed();
            v.data.value.y = color.getGreen();
            v.data.value.z = color.getBlue();
            v.data.value.w = color.getAlpha();
            return;
        }
    }
    Vec4 v;
    v.x = color.getRed();
    v.y = color.getGreen();
    v.z = color.getBlue();
    v.w = color.getAlpha();

    _uniforms.emplace_back(name, glGetUniformLocation(shader_program->id(), name.c_str()),
                           v);
}

void Material::set_uniform(const char* name, const Color& color)
{
    for(auto& v : _uniforms)
    {
        if(strcmp(v.name, name) == 0)
        {
            v.data.value.x = color.getRed();
            v.data.value.y = color.getGreen();
            v.data.value.z = color.getBlue();
            v.data.value.w = color.getAlpha();
            return;
        }
    }
    Vec4 v;
    v.x = color.getRed();
    v.y = color.getGreen();
    v.z = color.getBlue();
    v.w = color.getAlpha();
    _uniforms.emplace_back(name, glGetUniformLocation(shader_program->id(), name), v);
}

void Material::set_uniform(const std::string& name, Vec4 value)
{
    for(auto& v : _uniforms)
    {
        if(strcmp(v.name, name.c_str()) == 0)
        {
            v.data.value.x = value.x;
            v.data.value.y = value.y;
            v.data.value.z = value.z;
            v.data.value.w = value.w;
            return;
        }
    }
    _uniforms.emplace_back(name, glGetUniformLocation(shader_program->id(), name.c_str()),
                           value);
}

void Material::set_uniform(const std::string& name, float x, float y, float z, float w)
{
    set_uniform(name, Vec4(x, y, z, w));
}

void Material::set_uniform(const char* name, Vec4 value)
{
    for(auto& v : _uniforms)
    {
        if(strcmp(v.name, name) == 0)
        {
            v.data.value.x = value.x;
            v.data.value.y = value.y;
            v.data.value.z = value.z;
            v.data.value.w = value.w;
            return;
        }
    }
    _uniforms.emplace_back(name, glGetUniformLocation(shader_program->id(), name), value);
}

void Material::set_uniform(const char* name, float x, float y, float z, float w)
{
    for(auto& v : _uniforms)
    {
        if(strcmp(v.name, name) == 0)
        {
            v.data.value.x = x;
            v.data.value.y = y;
            v.data.value.z = z;
            v.data.value.w = w;
            return;
        }
    }
    _uniforms.emplace_back(name, glGetUniformLocation(shader_program->id(), name),
                           Vec4(x, y, z, w));
}

void Material::set_uniform(
    int index, const std::string& name, float x, float y, float z, float w)
{
    if(_uniforms.size() < index)
    {
        _uniforms.resize(index);
        _uniforms[index] =
            Uniform(name, glGetUniformLocation(shader_program->id(), name.c_str()),
                    Vec4(x, y, z, w));
        return;
    }
    _uniforms[index].data.value.x = x;
    _uniforms[index].data.value.y = y;
    _uniforms[index].data.value.z = z;
    _uniforms[index].data.value.w = w;
}

void Material::add_texture(const Texture& texture, const char* name)
{
    _texture_uniforms.emplace_back(name, glGetUniformLocation(shader_program->id(), name),
                                   &texture);
}

void Material::set_texture(int index, const Texture& texture, const std::string& name)
{
    if(_texture_uniforms.size() <= index)
    {
        _texture_uniforms.resize(index + 1);
    }

    _texture_uniforms[index].texture = &texture;

    if(strcmp(name.c_str(), _texture_uniforms[index].name) != 0)
    {
        strcpy(_texture_uniforms[index].name, name.c_str());
        _texture_uniforms[index].location =
            glGetUniformLocation(shader_program->id(), name.c_str());
    }
}

void Material::set_texture(int index, const Texture& texture)
{
    _texture_uniforms[index].texture = &texture;
}

bool Material::operator==(const Material& other) const
{
    if(render_queue != other.render_queue)
        return false;

    if(shader_program.get() != nullptr && other.shader_program.get() == nullptr)
        return false;

    if(shader_program.get() == nullptr && other.shader_program.get() != nullptr)
        return false;

    if(shader_program.get() != nullptr && other.shader_program.get() != nullptr)
    {
        if(shader_program->id_ != other.shader_program->id_)
            return false;
    }

    if(_texture_uniforms != other._texture_uniforms)
        return false;

    if(_uniforms != other._uniforms)
        return false;

    if(_flags != other._flags)
        return false;

    if(stencils_ != other.stencils_)
        return false;

    if(stencil_test != other.stencil_test)
        return false;

    if(depth_test != other.depth_test)
        return false;

    return true;
}

static bool are_vectors_equals(const std::vector<Material::TextureUniform>& u1,
                               const std::vector<Material::TextureUniform>& u2)
{
    const auto* ua1   = u1.data();
    const auto* ua2   = u2.data();
    const auto  size1 = u1.size();

    if(size1 != u2.size())
        return false;

    for(unsigned i = 0; i < size1; i++)
    {
        if(!(ua1[i] == ua2[i]))
            return false;
    }
    return true;
}

static bool are_vectors_min(const std::vector<Material::TextureUniform>& u1,
                            const std::vector<Material::TextureUniform>& u2)
{
    const auto* ua1   = u1.data();
    const auto* ua2   = u2.data();
    const auto  size1 = u1.size();
    const auto  size2 = u2.size();

    if(size1 != size2)
        return size1 < size2;

    for(unsigned i = 0; i < size1; i++)
    {
        if(!(ua1[i] == ua2[i]))
            return ua1[i] < ua2[i];
    }
    return true;
}

bool Material::operator<(const Material& other) const
{
    if(render_queue < other.render_queue)
        return true;

    if(render_queue > other.render_queue)
        return false;

    if(shader_program->id_ < other.shader_program->id_)
        return true;

    if(shader_program->id_ > other.shader_program->id_)
        return false;

    // a <=> b is called "three way comparison, where <0 means that a is less than b, >0 means that a is more than b and ==0 means that
    // they are equals. Quite useful to avoid making extra comparisons
    auto r = _texture_uniforms <=> other._texture_uniforms;

    if(r < 0)
        return true;

    if(r > 0)
        return false;

    r = _uniforms <=> other._uniforms;

    if(r < 0)
        return true;

    if(r > 0)
        return false;

    if(_flags != other._flags)
        return _flags < other._flags;

    if(stencils_ != other.stencils_)
        return stencils_ < other.stencils_;

    if(stencil_value != other.stencil_value)
        return stencil_value < other.stencil_value;

    return false;
}

Material::Material(const std::string& name)
    : name_(name)
{
}

static std::shared_ptr<ShaderProgram> generate_program(const char*   name,
                                                       const Shader* vertex_shader,
                                                       const Shader* fragment_shader)
{
    auto* program =
        new ShaderProgram(name, vertex_shader, fragment_shader, glCreateProgram());

    glAttachShader(program->id_, vertex_shader->id());
    glAttachShader(program->id_, fragment_shader->id());

    glLinkProgram(program->id_);
    return std::shared_ptr<ShaderProgram>(program);
}

void Material::render_color(bool value)
{
    _flags.set_bit(5, value);
}

[[nodiscard]] bool Material::render_color() const
{
    return _flags.check_bit(5);
}

void Material::clear_stencil_buffer(bool value)
{
    _flags.set_bit(6, value);
}

void Material::is_transparent(bool val)
{
    _flags.set_bit(1, val);
}

bool Material::is_transparent() const
{
    return _flags.check_bit(1);
}

void Material::enable_depth_test(bool value)
{
    _flags.set_bit(2, value);
}

bool Material::enable_depth_test() const
{
    return _flags.check_bit(2);
}

void Material::enable_blend(bool value)
{
    _flags.set_bit(3, value);
}

bool Material::enable_blend() const
{
    return _flags.check_bit(3);
}

void Material::depth_mask(bool value)
{
    _flags.set_bit(4, value);
}

bool Material::depth_mask() const
{
    return _flags.check_bit(4);
}

bool Material::clear_stencil_buffer() const
{
    return _flags.check_bit(6);
}

void Material::stencil_mask(bool value)
{
    _flags.set_bit(7, value);
}

bool Material::stencil_mask() const
{
    return _flags.check_bit(7);
}

void Material::stencil_fail(StencilOp value)
{
    stencils_.set_bit(0, (static_cast<char>(value) >> 0) & 1);
}

StencilOp Material::stencil_fail() const
{
    return static_cast<StencilOp>(stencils_.get_bit(0));
}

void Material::stencil_success_depth_fail(StencilOp value)
{
    stencils_.set_bit(1, static_cast<char>(value));
}

void Material::is_lit(bool val)
{
    _flags.set_bit(0, val);
}

bool Material::is_lit() const
{
    return _flags.check_bit(0);
}

StencilOp Material::stencil_success_depth_fail() const
{
    return static_cast<StencilOp>(stencils_.get_bit(1) >> 1);
}

void Material::stencil_success_depth_success(StencilOp value)
{
    stencils_.set_bit(2, static_cast<char>(value) >> 0);
}

StencilOp Material::stencil_success_depth_success() const
{
    return static_cast<StencilOp>(stencils_.get_bit(2) >> 2);
}

StencilOp Material::stencil_op() const
{
    return static_cast<StencilOp>(stencils_.get_bit(3) >> 3);
}

void Material::write_color(bool value)
{
    stencils_.set_bit(4, value);
}

bool Material::write_color() const noexcept
{
    return stencils_.get_bit(4);
}

void Material::enable_stencil_test(bool value)
{
    stencils_.set_bit(5, value);
}

bool Material::enable_stencil_test() const noexcept
{
    return stencils_.get_bit(5);
}

void Material::polygon_mode(PolygonMode value)
{
    stencils_.set_bits(6, 2, static_cast<char>(value));
}

Material::PolygonMode Material::polygon_mode() const noexcept
{
    return static_cast<PolygonMode>(stencils_.get_bits(6, 2));
}

void Material::face(FaceMode face_mode)
{
    _face = face_mode;
}

FaceMode Material::face() const noexcept
{
    return _face;
}

void Material::stencil_op(StencilOp value)
{
    stencils_.set_bit(3, static_cast<char>(value));
}

Material::Material(const std::string& path, const std::string& relative_name)
{
    std::ifstream file(path.c_str(), std::ifstream::in | std::ifstream::binary);

    int fileSize = 0;

    if(file.is_open())
    {
        file.seekg(0, std::ios::end);
        fileSize = int(file.tellg());
        file.close();
    }

    FILE* fp = fopen(path.c_str(), "rb");    // non-Windows use "r"

    auto* readBuffer = new char[fileSize];

    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document       document;
    document.ParseStream(is);

    // Not optional
    assert(document.HasMember("vertex_shader"));
    assert(document.HasMember("fragment_shader"));

    std::string vertex_shader_path   = document["vertex_shader"].GetString();
    std::string fragment_shader_path = document["fragment_shader"].GetString();

    auto vertex_shader   = ResourcesCache::get<Shader>(vertex_shader_path);
    auto fragment_shader = ResourcesCache::get<Shader>(fragment_shader_path);

    auto program = generate_program(filesystem::filename(path.c_str()).c_str(),
                                    vertex_shader, fragment_shader);

    location_model_view_projection_matrix =
        glGetUniformLocation(program->id_, "mvp_matrix");

    auto strr = filesystem::filename(path.c_str());

    name_          = strr.c_str();
    shader_program = program;

    if(document.HasMember("is_lit"))
        is_lit(document["is_lit"].GetBool());

    if(document.HasMember("Samplers"))
    {
        for(auto& value : document["Samplers"].GetArray())
        {
            _texture_uniforms.emplace_back(
                std::string(value["name"].GetString()),
                glGetUniformLocation(shader_program->id(), value["name"].GetString()),
                nullptr);
        }
    }

    if(document.HasMember("Uniforms"))
    {
        for(auto& value : document["Uniforms"].GetArray())
        {
            if(value["type"] == "int")
            {
                set_uniform(value["name"].GetString(), value["value"].GetInt());
                continue;
            }
            if(value["type"] == "float")
            {
                set_uniform(value["name"].GetString(), value["value"].GetFloat());
                continue;
            }
            if(value["type"] == "Vec2")
            {
                set_uniform(value["name"].GetString(), Vec2());
                continue;
            }
            if(value["type"] == "Vec3")
            {
                set_uniform(value["name"].GetString(), Vec3());
                continue;
            }
            if(value["type"] == "Vec4")
            {
                set_uniform(value["name"].GetString(), Vec4());
            }
            if(value["type"] == "Matrix")
            {
                set_uniform(value["name"].GetString(), Matrix());
            }
        }
    }
    delete[] readBuffer;
}

long long Material::memory_usage() const
{
    return sizeof(Material);
}

void Material::remove_uniform(const char* name)
{
    //uniforms_.erase(name);
}
}    // namespace corgi
