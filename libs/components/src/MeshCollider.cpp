#include <corgi/components/MeshCollider.h>
#include <corgi/components/MeshRenderer.h>

#include <corgi/resources/Mesh.h>
#include <corgi/math/Collisions.h>

#include <assert.h>

namespace corgi
{
	void MeshCollider::mesh(std::shared_ptr<Mesh> m)
	{
		// Assuming there is at least a position attribute inside the mesh
		assert(m->has_attribute(0));
		assert(m->attribute(0)->size == 3);

		_mesh = m;

		// Looping to compute every triangle's normal

		for (size_t i = 0; i < m->indexes().size(); i += 3)
		{
			Vec3 posA (&m->vertices()[m->indexes()[i]	  * m->vertex_size() + 0]);
			Vec3 posB (&m->vertices()[m->indexes()[i + 1] * m->vertex_size() + 0]);
			Vec3 posC (&m->vertices()[m->indexes()[i + 2] * m->vertex_size() + 0]);

			normals.push_back(math::get_triangle_normal(posA, posB, posC));
		}
	}
}