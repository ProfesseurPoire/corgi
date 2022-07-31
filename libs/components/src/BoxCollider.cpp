#include <corgi/components/BoxCollider.h>
#include <corgi/resources/Mesh.h>

namespace corgi
{
	BoxCollider::BoxCollider()
	{
		// Actually builds a mesh under the hood for the BoxCollider
		_mesh = std::make_shared<Mesh>(std::initializer_list<VertexAttribute>({ {0, 0, 3} }));
		build_box();
	}

	BoxCollider::BoxCollider(int l, float width, float height, float depth)
		: _width(width), _height(height),  _depth(depth)
	{
		layer(l);
		_mesh = std::make_shared<Mesh>(std::initializer_list<VertexAttribute>({ {0, 0, 3} }));
		build_box();
	}

	void BoxCollider::dimensions(float width, float height, float depth)
	{
		_width		= width;
		_height		= height;
		_depth		= depth;
		build_box();
	}

	float BoxCollider::width()const
	{
		return _width;
	}

	float BoxCollider::height()const
	{
		return _height;
	}

	float BoxCollider::depth()const
	{
		return _depth;
	}

	void BoxCollider::width(float v)
	{
		_width = v;
		build_box();
	}

	void BoxCollider::height(float v)
	{
		_height = v;
		build_box();
	}

	void BoxCollider::depth(float v)
	{
		_depth = v;
		build_box();
	}

	void BoxCollider::build_box()
	{
		_mesh->clear();
		
		const float mdepth	= depth()  / 2.0f;
		const float mwidth	= width()  / 2.0f;
		const float mheight	= height() / 2.0f;

		float* vertices = new float[24];
		unsigned* indexes = new unsigned[12 * 3];
		

		vertices[0] = -mwidth;
		vertices[1] = -mheight;
		vertices[2] = +mdepth;

		vertices[3] = +mwidth;
		vertices[4] = -mheight;
		vertices[5] = +mdepth;

		vertices[6] = -mwidth;
		vertices[7] = +mheight;
		vertices[8] = +mdepth;

		vertices[9]  = +mwidth;
		vertices[10] = +mheight;
		vertices[11] = +mdepth;
		
		vertices[12] = -mwidth;
		vertices[13] = -mheight;
		vertices[14] = -mdepth;

		vertices[15] = +mwidth;
		vertices[16] = -mheight;
		vertices[17] = -mdepth;

		vertices[18] = -mwidth;
		vertices[19] = +mheight;
		vertices[20] = -mdepth;

		vertices[21] = +mwidth;
		vertices[22] = +mheight;
		vertices[23] = -mdepth;

		_mesh->set_vertices(vertices, 24);
		
		//Front

		indexes[0] = 0;
		indexes[1] = 1;
		indexes[2] = 3;

		indexes[3] = 0;
		indexes[4] = 3;
		indexes[5] = 2;

		normals.push_back(Vec3::forward());
		normals.push_back(Vec3::forward());

		//// Back

		indexes[6] = 4;
		indexes[7] = 5;
		indexes[8] = 7;

		indexes[9] = 4;
		indexes[10] = 7;
		indexes[11] = 6;

		normals.push_back(Vec3::backward());
		normals.push_back(Vec3::backward());

		// Left

		indexes[12] = 4;
		indexes[13] = 0;
		indexes[14] = 2;

		indexes[15] = 4;
		indexes[16] = 2;
		indexes[17] = 6;

		normals.push_back(Vec3::left());
		normals.push_back(Vec3::left());

		// Right 

		indexes[18] = 1;
		indexes[19] = 5;
		indexes[20] = 7;

		indexes[21] = 1;
		indexes[22] = 7;
		indexes[23] = 3;

		normals.push_back(Vec3::right());
		normals.push_back(Vec3::right());

		// Top 

		indexes[24] = 2;
		indexes[25] = 3;
		indexes[26] = 6;

		indexes[27] = 3;
		indexes[28] = 7;
		indexes[29] = 6;
		

		normals.push_back(Vec3::up());
		normals.push_back(Vec3::up());

		// Down

		indexes[30] = 0;
		indexes[31] = 1;
		indexes[32] = 4;

		indexes[33] = 1;
		indexes[34] = 5;
		indexes[35] = 4;
		

		normals.push_back(Vec3::down());
		normals.push_back(Vec3::down());

		_mesh->set_indices(indexes, 12 * 3);
		_mesh->update_vertices();
	}
}