#include "Mesh.h"

#include <corgi/math/MathUtils.h>
#include <corgi/rendering/RenderCommand.h>
#include <corgi/rendering/renderer.h>
#include <glad/glad.h>

#include <algorithm>
#include <cassert>

namespace corgi
{
    Vertex::Vertex(float* data)
        : _data(data)
    {
    }

    Vertex::Vertex(float* data, const std::vector<VertexAttribute>& attributes)
        : _data(data)
        , _attributes(attributes)
    {
    }

    // TODO : Maybe using a map for the vertex part makes sense
    // Update if I notice some slowdown on mesh construction
    Vertex& Vertex::set_attribute(int location, float x)
    {
        _data[attribute(location)->offset] = x;
        return *this;
    }

    Vertex& Vertex::set_attribute(int location, float x, float y)
    {
        _data[attribute(location)->offset + 0] = x;
        _data[attribute(location)->offset + 1] = y;
        return *this;
    }

    Vertex& Vertex::set_attribute(int location, float x, float y, float z)
    {
        _data[attribute(location)->offset + 0] = x;
        _data[attribute(location)->offset + 1] = y;
        _data[attribute(location)->offset + 2] = z;
        return *this;
    }

    Vertex& Vertex::set_attribute(int location, float x, float y, float z, float w)
    {
        _data[attribute(location)->offset + 0] = x;
        _data[attribute(location)->offset + 1] = y;
        _data[attribute(location)->offset + 2] = z;
        _data[attribute(location)->offset + 3] = w;
        return *this;
    }

    const VertexAttribute* Vertex::attribute(int location) const
    {
        for(auto& attribute : _attributes)
        {
            if(attribute.location == location)
                return &attribute;
        }
        return nullptr;
    }

    Vertex& Vertex::position(float x, float y, float z)
    {
        // 0 is by default location for position in glsl
        return set_attribute(0, x, y, z);
    }

    Vertex& Vertex::position(const Vec3& pos) { return position(pos.x, pos.y, pos.z); }

    Vertex& Vertex::uv(float u, float v)
    {
        // 1 is default location for TexCoord0
        return set_attribute(1, u, v);
    }

    Vertex& Vertex::uv(const Vec2& uv) { return this->uv(uv.x, uv.y); }

    Vertex& Vertex::normal(const Vec3& n) { return normal(n.x, n.y, n.z); }

    Vertex& Vertex::normal(float x, float y, float z) { return set_attribute(2, x, y, z); }

    int Mesh::vertex_count() const { return static_cast<int>(vertices_.size()) / vertex_size(); }

    // Only works in 2D for now
    // I'm also kinda assuming that the position data is the first
    // attribute in the thing
    void Mesh::build_bounding_volumes()
    {
        const auto v_count = vertex_count();
        const auto v_size  = vertex_size();
        const auto v       = vertices().data();

        for(int i = 0; i < v_count; ++i)
        {
            float x = v[i * v_size];
            float y = v[i * v_size + 1];

            if(x < bounding_box.bottom_left_x)
                bounding_box.bottom_left_x = x;

            if(x > bounding_box.top_right_x)
                bounding_box.top_right_x = x;

            if(y < bounding_box.bottom_left_y)
                bounding_box.bottom_left_y = y;

            if(y > bounding_box.top_right_y)
                bounding_box.top_right_y = y;
        }

        // So now I have the bounding box
        // time to find the bounding circle

        float w = (bounding_box.top_right_x - bounding_box.bottom_left_x) / 2.0f;
        float h = (bounding_box.top_right_y - bounding_box.bottom_left_y) / 2.0f;

        bounding_circle_radius = math::sqrtf(w * w + h * h);

        bounding_circle_offset_x = bounding_box.bottom_left_x + w;
        bounding_circle_offset_y = bounding_box.bottom_left_y + h;
    }

    int Mesh::vertex_size() const
    {
        int count = 0;
        for(auto& attribute : _attributes)
            count += attribute.size;
        return count;
    }

    SharedPtr<Mesh> Mesh::new_standard_mesh() { return SharedPtr<Mesh>(new Mesh({{0, 0, 3}, {1, 3, 2}, {2, 5, 3}})); }

    SharedPtr<Mesh> Mesh::new_standard_mesh(std::vector<float>&& vertices, std::vector<unsigned int>&& indices)
    {
        return SharedPtr<Mesh>(new Mesh({{0, 0, 3}, {1, 3, 2}, {2, 5, 3}}, std::move(vertices), std::move(indices)));
    }

    SharedPtr<Mesh> Mesh::new_standard_2D_mesh(std::vector<float>&& vertices, std::vector<unsigned>&& indices)
    {
        return SharedPtr<Mesh>(new Mesh({{0, 0, 3}, {1, 3, 2}}, std::move(vertices), std::move(indices)));
    }

    SharedPtr<Mesh> Mesh::new_standard_2D_mesh() { return SharedPtr<Mesh>(new Mesh({{0, 0, 3}, {1, 3, 2}})); }

    SharedPtr<Mesh> Mesh::new_standard_sphere(const float radius, const int discretisation)
    {
        // We need at least more than 1 slice
        assert(discretisation > 1);

        auto s = SharedPtr<Mesh>(new Mesh({VertexAttribute {0, 0, 3}, VertexAttribute {2, 3, 3}}));

        // We build the vertices first

        float step_height = (radius * 2.0f) / (discretisation + 1);
        float height      = -radius;

        for(int i = 0; i < discretisation; ++i)
        {
            // We try to find the radius of the current sphere slice, which
            // is a circle.
            float slice_angle  = math::asinf(math::absf(height) / (radius));
            float local_radius = math::cos(slice_angle) * radius;

            for(int j = 0; j < discretisation; ++j)
            {
                float angle = 2.0f * math::pi / (discretisation);
                angle       = angle * j;

                float x = math::cos(angle) * local_radius;
                float z = math::sinf(angle) * local_radius;

                Vec3 position(x, height, z);
                Vec3 normal(position.normalized());

                s->add_vertex().position(position).normal(normal);
            }
            height += step_height;
        }

        // We build the triangles

        // Skipping the first slice
        for(int i = 1; i < discretisation; ++i)
        {
            for(int j = 0; j < discretisation; j++)
            {
                int C = i * discretisation + j;
                int D = C + 1;
                int A = C - discretisation;
                int B = A + 1;

                //
                //  C----- D
                //
                //  A ---  B

                s->addTriangle(A, B, D);
                s->addTriangle(A, D, C);
            }

            //
            //  C----- D
            //
            //  A ---  B

            int C = (i + 1) * discretisation - 1;
            int A = C - discretisation;
            int B = (A + 1) - discretisation;
            int D = C + 1 - discretisation;

            s->addTriangle(A, B, D);
            s->addTriangle(A, D, C);
        }

        s->update_vertices();
        return s;
    }

    Mesh::Mesh(const std::string& file, const std::string& name) {}

    Mesh::Mesh(const std::vector<VertexAttribute>& attributes)
        : _attributes(attributes)
        , _vbo_index(RenderCommand::generate_buffer_object())
        , _ibo_index(RenderCommand::generate_buffer_object())
        , vao_id_(RenderCommand::generate_vao_buffer())
        , _primitive_type(PrimitiveType::Triangles)
    {

        meshes.push_back(this);
    }

    Mesh::Mesh(std::vector<VertexAttribute>&& attributes)
        : _attributes(std::move(attributes))
        , _vbo_index(RenderCommand::generate_buffer_object())
        , _ibo_index(RenderCommand::generate_buffer_object())
        , vao_id_(RenderCommand::generate_vao_buffer())
        , _primitive_type(PrimitiveType::Triangles)
    {
        meshes.push_back(this);
    }

    Mesh::Mesh(std::vector<VertexAttribute>&& attributes,
               std::vector<float>&&           vertices,
               std::vector<unsigned>&&        indices)
        : _attributes(std::move(attributes))
        , _vbo_index(RenderCommand::generate_buffer_object())
        , _ibo_index(RenderCommand::generate_buffer_object())
        , vao_id_(RenderCommand::generate_vao_buffer())
        , _primitive_type(PrimitiveType::Triangles)
        , vertices_(std::move(vertices))
        , indexes_(std::move(indices))
    {
        meshes.push_back(this);
        RenderCommand::bind_vertex_array(vao_id_);

        RenderCommand::buffer_vertex_data(_vbo_index, vertices_.data(),
                                          static_cast<int>(sizeof(float) * vertices_.size()));

        RenderCommand::buffer_index_data(_ibo_index, indexes_.data(),
                                         static_cast<int>(indexes_.size() * sizeof(unsigned int)));

        for(auto& attribute : _attributes)
        {
            RenderCommand::enable_vertex_attribute(attribute.location);
            RenderCommand::vertex_attribute_pointer(attribute.location, vertex_size() * sizeof(float), attribute.offset,
                                                    attribute.size);
        }

        RenderCommand::bind_vertex_array(0);
        for(auto& attribute : _attributes)
            RenderCommand::disable_vertex_attribute(attribute.location);
    }

    Vertex Mesh::add_vertex()
    {
        vertices_.insert(vertices_.end(), vertex_size(), 0.0f);
        return Vertex(&vertices_[vertices_.size() - vertex_size()], _attributes);
    }

    std::vector<Vertex> Mesh::add_vertices(int count)
    {
        std::vector<Vertex> vertices;

        vertices_.insert(vertices_.end(), vertex_size() * count, 0.0f);

        for(int i = 0; i < count; ++i)
            vertices.emplace_back(&vertices_[vertices_.size() - (count - i) * vertex_size()], _attributes);
        return vertices;
    }

    void Mesh::update_vertices()
    {
        RenderCommand::bind_vertex_array(vao_id_);

        RenderCommand::buffer_vertex_data(_vbo_index, vertices_.data(),
                                          static_cast<int>(sizeof(float) * vertices_.size()));

        RenderCommand::buffer_index_data(_ibo_index, indexes_.data(),
                                         static_cast<int>(indexes_.size() * sizeof(unsigned int)));

        for(auto& attribute : _attributes)
        {
            RenderCommand::enable_vertex_attribute(attribute.location);
            RenderCommand::vertex_attribute_pointer(attribute.location, vertex_size() * sizeof(float), attribute.offset,
                                                    attribute.size);
        }

        RenderCommand::bind_vertex_array(0);
        for(auto& attribute : _attributes)
            RenderCommand::disable_vertex_attribute(attribute.location);
    }

    void Mesh::clear()
    {
        vertices_.clear();
        indexes_.clear();

        // TODO : I don't think I need to delete and remake the VBO, I could probably use glBufferSubData or something no?
        RenderCommand::delete_vertex_buffer_object(_vbo_index);
        RenderCommand::delete_vertex_buffer_object(_ibo_index);
        RenderCommand::delete_vertex_array_object(vao_id_);

        _vbo_index = RenderCommand::generate_buffer_object();
        _ibo_index = RenderCommand::generate_buffer_object();
        vao_id_    = RenderCommand::generate_vao_buffer();

        RenderCommand::bind_vertex_array(vao_id_);
        RenderCommand::bind_vertex_buffer_object(_vbo_index);
        RenderCommand::bind_index_buffer_object(_ibo_index);
        RenderCommand::bind_vertex_array(0);
        RenderCommand::bind_vertex_buffer_object(0);
        RenderCommand::bind_index_buffer_object(0);
    }

    void Mesh::update_vertices_really()
    {
        RenderCommand::buffer_vertex_subdata(_vbo_index, vertices_.data(), vertices_.size() * sizeof(float));
    }

    std::vector<float>& Mesh::vertices() { return vertices_; }

    Mesh::~Mesh()
    {
        RenderCommand::delete_vertex_buffer_object(_vbo_index);
        RenderCommand::delete_vertex_buffer_object(_ibo_index);
        RenderCommand::delete_vertex_array_object(vao_id_);

        meshes.erase(std::find(meshes.begin(), meshes.end(), this));
    }

    const VertexAttribute* Mesh::attribute(int location) const
    {
        for(auto& attribute : _attributes)
            if(attribute.location == location)
                return &attribute;
        return nullptr;
    }

    void Mesh::addTriangle(unsigned index1, unsigned index2, unsigned index3)
    {
        // TODO : It would be nice to check the actual performance on this, because
        // I'm afraid it's probably really slow
        indexes_.insert(indexes_.end(), {index1, index2, index3});
    }

    void Mesh::addLine(unsigned index1, unsigned index2) { indexes_.insert(indexes_.end(), {index1, index2}); }

    const std::vector<float>& Mesh::vertices() const { return vertices_; }

    const std::vector<unsigned int>& Mesh::indexes() const { return indexes_; }

    PrimitiveType Mesh::primitive_type() const { return _primitive_type; }

    void Mesh::primitive_type(PrimitiveType typ) { _primitive_type = typ; }

    void Mesh::setPrimitiveType(PrimitiveType pt) { _primitive_type = pt; }

    bool Mesh::has_attribute(int location) const
    {
        // TODO : As always, not too sure about the performances of this,
        // especially since the vector is really small
        return std::find_if(_attributes.begin(), _attributes.end(),
                            [&](const VertexAttribute& v) { return v.location == location; }) != _attributes.end();
    }

    bool Vertex::has_attribute(int location) const
    {
        return std::find_if(_attributes.begin(), _attributes.end(),
                            [&](const VertexAttribute& v) { return v.location == location; }) != _attributes.end();
    }

    void Mesh::set_vertices(float* vertices, int count) { vertices_ = std::vector<float>(vertices, vertices + count); }

    void Mesh::set_indices(unsigned* i, int count) { indexes_ = std::vector<unsigned>(i, i + count); }

    std::vector<unsigned int>& Mesh::indexes() { return indexes_; }
}    // namespace corgi
