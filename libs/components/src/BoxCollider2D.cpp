#include <corgi/components/BoxCollider2D.h>

namespace corgi
{
    BoxCollider2D::BoxCollider2D()
    {
        _mesh       = corgi::make_shared<Mesh>(std::initializer_list<VertexAttribute>({ {0, 0, 3} }));
    	_edges_mesh = corgi::make_shared<Mesh>(std::initializer_list<VertexAttribute>({ {0, 0, 3} }));
    	
        build_box();
    }

	BoxCollider2D::BoxCollider2D(int layer, float width, float height):
			ColliderComponent(layer),
			_width(width),
			_height(height)
    {
	    _mesh       = corgi::make_shared<Mesh>(std::initializer_list<VertexAttribute>({ {0, 0, 3} }));
    	_edges_mesh = corgi::make_shared<Mesh>(std::initializer_list<VertexAttribute>({ {0, 0, 3} }));
        build_box();
    }

	BoxCollider2D::BoxCollider2D(int layer, int width, int height):
		ColliderComponent(layer),
		_width(static_cast<float>(width)),
		_height(static_cast<float>(height))
    {
        _mesh = corgi::make_shared<Mesh>(std::initializer_list<VertexAttribute>({ {0, 0, 3} }));
        _edges_mesh = corgi::make_shared<Mesh>(std::initializer_list<VertexAttribute>({ {0, 0, 3} }));
        build_box();
    }

	void BoxCollider2D::dimensions(float width, float height)
    {
        _width  = width;
        _height = height;
        build_box();
    }

	float BoxCollider2D::width() const noexcept
    {
        return _width;
    }

	float BoxCollider2D::height() const noexcept
    {
        return _height;
    }

	void BoxCollider2D::width(float value)
    {
        _width = value;
        build_box();
    }

	void BoxCollider2D::height(float value)
    {
        _height = value;
        build_box();
    }

	const std::vector<Vec2>& BoxCollider2D::edges() const noexcept
    {
        return _edges;
    }

	const std::vector<Vec2>& BoxCollider2D::positions() const noexcept
    {
        return _positions;
    }

	const std::vector<Vec2>& BoxCollider2D::axes() const noexcept
    {
        return _axes;
    }

	void BoxCollider2D::build_box()
    {
        _mesh->clear();
        _edges_mesh->clear();

        const float mwidth  = width() / 2.0f;
        const float mheight = height() / 2.0f;

        float* vertices = new float[4 * 3];
        unsigned* indexes = new unsigned[6];
    	
        vertices[0] = -mwidth;
        vertices[1] = -mheight;
        vertices[2] = 0.0f;

        vertices[3] = +mwidth;
        vertices[4] = -mheight;
        vertices[5] = 0.0f;

        vertices[6] = -mwidth;
        vertices[7] = +mheight;
        vertices[8] = 0.0f;

        vertices[9] = +mwidth;
        vertices[10] = +mheight;
        vertices[11] = 0.0f;

        indexes[0] = 0;
        indexes[1] = 1;
        indexes[2] = 3;

        indexes[3] = 0;
        indexes[4] = 3;
        indexes[5] = 2;

        _mesh->set_vertices(vertices, 12);
        _mesh->set_indices(indexes, 6);
    	
        normals.push_back(Vec3::forward());

        _mesh->update_vertices();

        auto* vert = new float[16];
        auto* ind = new unsigned[3 * 4];

        vert[0] = -mwidth;
        vert[1] = -mheight;
        vert[2] = 0.0f;

        vert[3] = +mwidth;
        vert[4] = -mheight;
        vert[5] = 0.0f;

        vert[6] = -mwidth;
        vert[7] = +mheight;
        vert[8] = 0.0f;

        vert[9] = +mwidth;
        vert[10] = +mheight;
        vert[11] = 0.0f;

        ind[0] = 0;
        ind[1] = 1;
        ind[2] = 1;

        ind[3] = 0;
        ind[4] = 2;
        ind[5] = 2;

        ind[6] = 1;
        ind[7] = 3;
        ind[8] = 3;

        ind[9] = 2;
        ind[10] = 3;
        ind[11] = 3;
    	
        _edges_mesh->set_vertices(vert, 16);
        _edges_mesh->set_indices(ind, 12);
        _edges_mesh->update_vertices();

        _edges.clear();
        _positions.clear();
        _axes.clear();

		// Don't think I need more than 2 for the 	
        _axes.emplace_back(1.0f, 0.0f);
        _axes.emplace_back(0.0f, 1.0f);

        _positions.emplace_back(-mwidth, -mheight);
        _positions.emplace_back(+mwidth, -mheight);
        _positions.emplace_back(-mwidth, +mheight);
        _positions.emplace_back(+mwidth, +mheight);

        _edges.emplace_back(-mwidth, -mheight);
        _edges.emplace_back(+mwidth, -mheight);

        _edges.emplace_back(+mwidth, -mheight);
        _edges.emplace_back(+mwidth, +mheight);

        _edges.emplace_back(+mwidth, +mheight);
        _edges.emplace_back(-mwidth, +mheight);

        _edges.emplace_back(-mwidth, +mheight);
        _edges.emplace_back(-mwidth, -mheight);
    }
}