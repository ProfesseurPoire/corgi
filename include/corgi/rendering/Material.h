#pragma once

#include <corgi/containers/Vector.h>
#include <corgi/math/Matrix.h>
#include <corgi/math/Vec2.h>
#include <corgi/math/Vec3.h>
#include <corgi/math/Vec4.h>
#include <corgi/rendering/AbstractMaterial.h>
#include <corgi/rendering/Sampler.h>
#include <corgi/rendering/ShaderProgram.h>
#include <corgi/rendering/UniformBufferObject.h>
#include <corgi/resources/Resource.h>
#include <corgi/utils/Color.h>
#include <corgi/utils/Flags.h>

#include <cstring>
#include <memory>
#include <string>
#include <vector>

struct UniformFloat;

namespace corgi
{
class Texture;

/*!
 * @brief	Goes with PolygonMode. Tells which face of a triangle should be rendered
 *			and how
 */
enum class FaceMode : char
{
    Front,
    Back,
    FrontAndBack
};

enum class DepthTest : char
{
    Always,
    Never,
    Less,
    Equal,
    LEqual,
    Greater,
    NotEqual,
    GEqual
};

enum class StencilTest : char
{
    Always,
    NotEqual,
    Equal
};

enum class StencilOp : char
{
    Replace = 0b00000000,
    Keep    = 0b00000001
};

// Material is a class that contains additional information on how a
// mesh should be rendered. Also, the renderer will sort the renderer
// component based on their material to reduce draw calls when the
// material's parameters are the same
class Material : public AbstractMaterial, public Resource
{
public:
    std::vector<Sampler*> samplers;

    enum class PolygonMode : char
    {
        Fill  = 0,
        Line  = 1,
        Point = 2
    };

    // Lifecycle

    Material(const std::string& name = "empty");
    Material(const std::string& path, const std::string& relative_name);
    Material(Descriptor descriptor);

    ~Material() override = default;

    // Functions

    [[nodiscard]] long long memory_usage() const override;

    // Look for an uniform called "name" and remove it
    void remove_uniform(const char* name);

    // Renderer Components are sorted by material to reduce draw calls
    bool operator<(const Material& m) const;
    bool operator==(const Material& m) const;

    //I use main_texture as a default name for texture in shader
    void add_texture(const Texture& texture, const char* name = "main_texture");

    void set_texture(int index, const Texture& texture, const std::string& name);
    void set_texture(int index, const Texture& texture);

    UniformBufferObject* ubo;

    // TODO : Set all variables privates

    struct TextureUniform
    {
        int            location;
        const Texture* texture;
        char           name[16];

        TextureUniform() = default;

        TextureUniform(const std::string& n, int p_location, const Texture* t)
        {
            std::strcpy(name, n.c_str());
            location = p_location;
            texture  = t;
        }

        bool operator==(const TextureUniform& other) const
        {
            if(location != other.location)
                return false;

            if(texture != other.texture)
                return false;

            return true;

            /*if (strcmp(name, other.name)!=0)
				return false;*/
        }

        [[nodiscard]] bool operator<(const TextureUniform& other) const noexcept
        {
            if(location < other.location)
                return true;

            if(other.location < location)
                return false;

            if(texture < other.texture)
                return true;

            return false;
        }
    };

    void set_uniform(const std::string& name, int value);
    void set_uniform(const char* name, int value);

    void set_uniform(const char* name, unsigned value);

    void set_uniform(int index, const std::string& name, int value);

    void set_uniform(const std::string& name, float value);
    void set_uniform(const char* name, float value);

    void set_uniform(int index, const std::string& name, float value);

    void set_uniform(const std::string& name, Vec2 value);
    void set_uniform(const std::string& name, float x, float y);
    void set_uniform(int index, const std::string& name, float x, float y);

    void set_uniform(const std::string& name, Matrix value);

    void set_uniform(const std::string& name, Vec3 value);
    void set_uniform(const std::string& name, float x, float y, float z);

    void set_uniform(const std::string& name, const Color& color);
    void set_uniform(const char* name, const Color& color);

    void set_uniform(const std::string& name, Vec4 value);
    void set_uniform(const std::string& name, float x, float y, float z, float w);
    void set_uniform(const char* name, Vec4 value);
    void set_uniform(const char* name, float x, float y, float z, float w);
    void
    set_uniform(int index, const std::string& name, float x, float y, float z, float w);

    enum class UniformType
    {
        Int,
        Unsigned,
        Float,
        Vec2,
        Vec3,
        Vec4,
        Matrix
    };

    //#pragma warning(disable : 4582)
    // I couldn't find how not to have the warning about
    // implicit value inside the union so I just desactivated for now

    struct Uniform
    {
        // TODO : really not sure about that, a lot of the time I feel that a
        // const char* should be enough, but what if the data gets delete from
        // elsewhere? So I'd rather have this
        char name[16];

        union Data
        {
            int           int_value;
            unsigned      _unsigned_value;
            Vec4          value;
            corgi::Matrix matrix;

            explicit Data()
                : value(Vec4(0.0f, 0.0f, 0.0f, 0.0f))
            {
            }

            explicit Data(int v)
                : int_value(v)
            {
            }

            explicit Data(unsigned v)
                : _unsigned_value(v)
            {
            }

            explicit Data(Matrix m)
                : matrix(m)
            {
            }

            explicit Data(const Vec4& v)
                : value(v)
            {
            }
        } data;

        UniformType type;
        int         location;

        explicit Uniform() {}

        // TODO : Check if I can only store a const char* pointer here
        // I only added a int as a parameter because he had difficulties chosing the right constructor
        explicit Uniform(const char* p_name, int p_location, unsigned value, int)
            : data(value)
            , type(UniformType::Unsigned)
            , location(p_location)
        {
            std::strcpy(name, p_name);
        }

        explicit Uniform(const std::string& p_name, int p_location, int v)
            : type(UniformType::Int)
            , location(p_location)
            , data(v)
        {
            std::strcpy(name, p_name.c_str());
            location = p_location;
        }

        explicit Uniform(const std::string& p_name, int p_location, float v)
            : type(UniformType::Float)
            , location(p_location)
            , data(Vec4(v, 0.0f, 0.0f, 0.0f))
        {
            std::strcpy(name, p_name.c_str());
        }

        explicit Uniform(const std::string& p_name, int p_location, Vec2 v)
            : data(Vec4(v.x, v.y, 0.0f, 0.0f))
        {
            std::strcpy(name, p_name.c_str());
            location = p_location;
            type     = UniformType::Vec2;
        }

        explicit Uniform(const std::string& p_name, int p_location, Vec3 v)
            : data(Vec4(v.x, v.y, v.z, 0.0f))
        {
            std::strcpy(name, p_name.c_str());
            location = p_location;
            type     = UniformType::Vec3;
        }

        explicit Uniform(const std::string& p_name, int p_location, Vec4 v)
            : data(v)
        {
            std::strcpy(name, p_name.c_str());
            location = p_location;
            type     = UniformType::Vec4;
        }

        explicit Uniform(const std::string& p_name, int p_location, Matrix m)
            : data(m)
        {
            std::strcpy(name, p_name.c_str());
            location = p_location;
            type     = UniformType::Matrix;
        }

        [[nodiscard]] bool operator==(const Uniform& other) const noexcept
        {
            if(location != other.location)
                return false;

            /*if (strcmp(name, other.name)!=0)
				return false;

			if (type != other.type)
				return false;
			*/

            // TODO : This probably doesn't work anymore tbh

            if(data.int_value != other.data.int_value)
                return false;

            if(data.value != other.data.value)
                return false;

            if(data.matrix != other.data.matrix)
                return false;

            return true;
        }

        /*!
		 * Used for sorting purposes
		 */
        [[nodiscard]] bool operator<(const Uniform& other) const noexcept
        {

            // TODO : Probably check that out a bit, with addition of the matrix
            // this probably doesn't work anymore

            if(location < other.location)
                return true;

            if(location > other.location)
                return false;

            if(data.int_value < other.data.int_value)
                return true;

            if(data.int_value > other.data.int_value)
                return false;

            if((data.value.lexicographical_comp(other.data.value)) < 0)
                return true;

            return false;
        }
    };

    //#pragma warning(enable : 4582)
    // What's inside flag variable

    void               is_lit(bool val);
    [[nodiscard]] bool is_lit() const;

    void               is_transparent(bool val);
    [[nodiscard]] bool is_transparent() const;

    void               enable_depth_test(bool value);
    [[nodiscard]] bool enable_depth_test() const;

    void               enable_blend(bool value);
    [[nodiscard]] bool enable_blend() const;

    void               depth_mask(bool value);
    [[nodiscard]] bool depth_mask() const;

    // When false, the object won't write in the color buffer
    void               render_color(bool value);
    [[nodiscard]] bool render_color() const;

    void               clear_stencil_buffer(bool value);
    [[nodiscard]] bool clear_stencil_buffer() const;

    void               stencil_mask(bool value);
    [[nodiscard]] bool stencil_mask() const;

    // Stencil Things

    void                    stencil_fail(StencilOp value);
    [[nodiscard]] StencilOp stencil_fail() const;

    void                    stencil_success_depth_fail(StencilOp value);
    [[nodiscard]] StencilOp stencil_success_depth_fail() const;

    void                    stencil_success_depth_success(StencilOp value);
    [[nodiscard]] StencilOp stencil_success_depth_success() const;

    void                    stencil_op(StencilOp value);
    [[nodiscard]] StencilOp stencil_op() const;

    void               write_color(bool value);
    [[nodiscard]] bool write_color() const noexcept;

    void               enable_stencil_test(bool value);
    [[nodiscard]] bool enable_stencil_test() const noexcept;

    void                      polygon_mode(PolygonMode value);
    [[nodiscard]] PolygonMode polygon_mode() const noexcept;

    /*!
	 * @brief	Sets which triangle's face should be rendered (Tied with PolygonMode)
	 *			Values are Front, Back and FrontAndBack
	 */
    void face(FaceMode face);

    /*!
	     * @brief	Gets which triangle's face should be rendered (Tied with PolygonMode)
	     *			Values are Front, Back and FrontAndBack
	     */
    [[nodiscard]] FaceMode face() const noexcept;

    // Variables

    short render_queue {0};

    std::shared_ptr<ShaderProgram> shader_program {nullptr};

    // I could remove another 16 bytes if I fuse
    // Texture/Sampler Uniform vector with the UniformVector
    std::vector<TextureUniform> _texture_uniforms;
    std::vector<Uniform>        _uniforms;

    // Shader Program linked to the current material
    int stencil_value {1};
    int location_model_view_projection_matrix {-1};

    Flags _flags = Flags(0b00011100);

    //
    // 6-7 bits : PolygonMode
    Flags stencils_ = Flags(0b00010000);

    StencilTest stencil_test = StencilTest::Always;    // 1 byte
    // Can use 2 bits

    // When the depth passes. If current fragment z value
    // is "less" than the one stored, then it passes and writes
    // 9 values. 4 bits
    //
    DepthTest depth_test = DepthTest::LEqual;    // 1 byte

    FaceMode _face = FaceMode::FrontAndBack;

    std::string name_;

    void update() override;

private:
    void generate_shaders();
};
}    // namespace corgi
