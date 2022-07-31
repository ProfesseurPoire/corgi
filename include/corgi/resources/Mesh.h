#pragma once

#include <corgi/math/Vec2.h>
#include <corgi/math/Vec3.h>
#include <corgi/resources/Resource.h>

#include <string>
#include <vector>
#include <memory>

namespace corgi
{
    enum class PrimitiveType : char
    {
        Triangles,
        Quads,
        Lines
    };

    struct VertexAttribute
    {
        int location {0};
        int offset {0};
        int size {0};
    };

    class Vertex
    {
    public:
        friend class Mesh;

        float* _data;

        // Just adding some default attributes
        Vertex& position(float x, float y, float z);
        Vertex& position(const Vec3& pos);

        // Uv = texture coordinate
        Vertex& uv(float u, float v);
        Vertex& uv(const Vec2& uv);

        Vertex& normal(float x, float y, float z);
        Vertex& normal(const Vec3& n);

        // An attribute range is between 1 and 4
        Vertex& set_attribute(int location, float x);
        Vertex& set_attribute(int location, float x, float y);
        Vertex& set_attribute(int location, float x, float y, float z);
        Vertex& set_attribute(int location, float x, float y, float z, float w);

        [[nodiscard]] bool                   has_attribute(int location) const;
        [[nodiscard]] const VertexAttribute* attribute(int location) const;

        Vertex(float* data);
        Vertex(float* data, const std::vector<VertexAttribute>& attributes);
        std::vector<VertexAttribute> _attributes;
    };

    // Stores Vertex and Primitives that can be displayed by the
    // renderer
    class Mesh : public Resource
    {
        friend class Renderer;

    public:
        /*!
		 * @brief	Every mesh will be registered here when constructed and destroyed
		 *			This gives me a way to track every mesh in existence
		 */
        static inline std::vector<Mesh*> meshes;

        // Static functions

        // Standard mesh has position, texture_coordinate, normals attributes
        static std::shared_ptr<Mesh> new_standard_mesh();

        static std::shared_ptr<Mesh>
        new_standard_mesh(std::vector<float>&&        vertices,
                          std::vector<unsigned int>&& indices);

        /*!
		 * @brief A standard 2D mesh wil have 3 float for the position (I know, I probably should use 2) and 2 float for uv coordinates
		 */
        static std::shared_ptr<Mesh>
        new_standard_2D_mesh(std::vector<float>&&        vertices,
                             std::vector<unsigned int>&& indices);

        static std::shared_ptr<Mesh> new_standard_2D_mesh();

        // Creates a standard sphere, with a position and normal attributes
        static std::shared_ptr<Mesh> new_standard_sphere(float radius,
                                                         int   discretisation);

        // Constructors

        Mesh(const std::string& file, const std::string& name);

        // You need to define what's inside the mesh before actually creating it
        Mesh(const std::vector<VertexAttribute>& attributes);
        Mesh(std::vector<VertexAttribute>&& attributes);

        Mesh(std::vector<VertexAttribute>&& attributes,
             std::vector<float>&&           vertices,
             std::vector<unsigned int>&&    indices);

        ~Mesh() override;

        // Functions

        void build_bounding_volumes();

        struct BoundingBox
        {
            float bottom_left_x {std::numeric_limits<float>::infinity()};
            float bottom_left_y {std::numeric_limits<float>::infinity()};
            float top_right_x {-std::numeric_limits<float>::infinity()};
            float top_right_y {-std::numeric_limits<float>::infinity()};
        };
        BoundingBox bounding_box;

        float bounding_circle_radius {0.0f};
        float bounding_circle_offset_x {0.0f};
        float bounding_circle_offset_y {0.0f};

        void addTriangle(unsigned int index1, unsigned int index2, unsigned int index3);

        void addLine(unsigned int index1, unsigned int index2);

        // Remove all the vertices
        void clear();

        // Checks if the Mesh has an attribute called "name"
        [[nodiscard]] bool has_attribute(int location) const;

        // Returns a read only pointer to an attribute
        // Returns nullptr if no attribute with the given name exist inside the mesh
        [[nodiscard]] const VertexAttribute* attribute(int location) const;

        // TODO : Add the ability to directly set the values of the vertex
        // like
        // add_vertex( 0.0F, 0.0f, 0.0f, 0.1f, 0.1f, 0.1f) etc, as if I had
        // knowledge of how things are
        // Add a new empty Vertex according to the current attributes
        Vertex add_vertex();

        // Manually set the mesh's vertices
        void set_vertices(float* v, int count);

        void set_indices(unsigned* i, int count);

        // Add n vertex to the mesh and return the vertices
        std::vector<Vertex> add_vertices(int count);

        [[nodiscard]] std::vector<float>&        vertices();
        [[nodiscard]] std::vector<unsigned int>& indexes();

        // Actually updates the VBO. The idea is that you first
        // creates all your vertex on the mesh side, then, once
        // you're done update the vertex and their attributes,
        // you push all at once to the GPU
        void update_vertices();

        void update_vertices_really();

        // Returns the mesh's vertices
        [[nodiscard]] const std::vector<float>& vertices() const;

        // Returns the mesh's indexes
        [[nodiscard]] const std::vector<unsigned int>& indexes() const;

        // Returns the size in bytes of one vertex
        [[nodiscard]] int vertex_size() const;

        // Returns how many vertex there are
        [[nodiscard]] int vertex_count() const;

        // Returns the primitive used to connect the vertices
        // Almost always a triangle
        [[nodiscard]] PrimitiveType primitive_type() const;

        // Sets the primitive type usd to connect the vertices
        [[deprecated("use setPrimiteType instead")]] void primitive_type(PrimitiveType typ);

        void setPrimitiveType(PrimitiveType pt);

    private:
        // Store the vertices in a raw format so it's easier to send it to OpenGL
        std::vector<float>        vertices_;    // 16 bytes
        std::vector<unsigned int> indexes_;     // 32 bytes

        // Stores information for the shaders on how to read the vertex data
        // For instance, if I had a "vec3 position" attribute in the shader
        // I would have a "position" attribute the size of 3 floats in a vertex
        std::vector<VertexAttribute> _attributes;    // 48 bytes

        // Vertex Buffer Object storing the vertices and indexes inside the GPU
        unsigned int _vbo_index = 0;    // 52 bytes
        unsigned int _ibo_index = 0;    // 56 bytes
        unsigned int vao_id_    = 0;

        // Primitive used for rendering. Can be TRIANGLES, QUADS or other things
        PrimitiveType _primitive_type;    // 57 bytes (so 60 total)
    };
}    // namespace corgi
