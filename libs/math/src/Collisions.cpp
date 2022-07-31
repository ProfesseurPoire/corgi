#include <corgi/math/Collisions.h>

#include <algorithm>
#include <limits>
#include <vector>

namespace corgi::math
{
static void extract_position_attribute(std::vector<Vec3>& positions,
                                       Matrix             model_matrix,
                                       const float*       vertices,
                                       int                vertex_size,
                                       int                position_offset,
                                       int                vertex_count)
{
    positions.reserve(static_cast<size_t>(vertex_count));

    for(int i = 0; i < vertex_count; ++i)
        positions.push_back(model_matrix *
                            Vec3(&vertices[i * vertex_size + position_offset]));
}

/*!
		 *
		 * @brief	Returns the normals of every triangle inside the given mesh
		 *
		 *			This function assume that the every 3 vertex we have a triangle
		 *
		 * @param model_matrix		Model matrix
		 * @param vertices			Array containing the vertex information
		 * @param vertex_size		How many float per vertex
		 * @param position_offset	Where the position information/attribute is store in a vertex
		 * @param vertex_count		How many vertex there is inside the array
		 */
void get_axes(const unsigned int* indexes,
              int                 indexes_size,
              std::vector<Vec3>&  normals,
              const Matrix&       model_matrix,
              const float*        vertices,
              int                 vertex_size,
              int                 position_offset)
{
    // That's the maximal number of normals that could be used
    // at least I avoid memory allocations, and push back would explode the
    // vector size anyways since it's *2 each time I think

    bool found = false;

    for(int i = 0; i < indexes_size; i += 3)
    {
        Vec3 normal = get_triangle_normal(
            model_matrix * Vec3(&vertices[indexes[i] * vertex_size + position_offset]),
            model_matrix *
                Vec3(&vertices[indexes[i + 1] * vertex_size + position_offset]),
            model_matrix *
                Vec3(&vertices[indexes[i + 2] * vertex_size + position_offset]));

        // We only add the normal if it doesn't already exist
        found = false;
        for(auto& norm : normals)
        {
            if(norm.equal(normal, 0.0001f) || norm.equal(-normal, 0.0001f))
                found = true;
        }

        if(!found)
            normals.push_back(normal);
    }
}

bool intersect_3D(Matrix          model_matrix_a,
                  const unsigned* index1,
                  int             index_1_size,
                  const float*    vertices_a,
                  int             vertex_size_a,
                  int             position_offset_a,
                  int             vertex_count_a,

                  Matrix          model_matrix_b,
                  const float*    vertices_b,
                  const unsigned* index2,
                  int             index_2_size,
                  int             vertex_size_b,
                  int             position_offset_b,
                  int             vertex_count_b)
{

    std::vector<Vec3> axes;
    axes.reserve(static_cast<size_t>(vertex_count_a + vertex_count_b));

    // I just have to compute the normals here otherwise I can't use the model matrix
    get_axes(index1, index_1_size, axes, model_matrix_a, vertices_a, vertex_size_a,
             position_offset_a);
    get_axes(index2, index_2_size, axes, model_matrix_b, vertices_b, vertex_size_b,
             position_offset_b);

    // TODO :	This is not a really performant operation, but at least it's easier to wrap
    //			my mind around that. Maybe later on, once this works, optimize it
    std::vector<Vec3> v_a;
    std::vector<Vec3> v_b;

    extract_position_attribute(v_a, model_matrix_a, vertices_a, vertex_size_a,
                               position_offset_a, vertex_count_a);
    extract_position_attribute(v_b, model_matrix_b, vertices_b, vertex_size_b,
                               position_offset_b, vertex_count_b);

    // Ces valeurs concernent le MTV, (Minimum translation vector)
    // Elles permettent de potentiellement pousser une forme au mieux pour �viter que
    // les formes ne se rentrent dedans

    Vec3  smallestaxis;
    float smallestmagnitude = 99999999999.0f;

    // Shape contient déjà les axes à tester

    //std::vector<Vec3> axes = new Vector3[shape1.axes_.Length + shape2.axes_.Length + (shape1.axes_.Length * shape2.axes_.Length)];
    //shape1.axes_.CopyTo(axes, 0);
    //shape2.axes_.CopyTo(axes, shape1.axes_.Length);

    std::vector<Vec3> shape1projections;
    shape1projections.reserve(axes.size());
    std::vector<Vec3> shape2projections;
    shape2projections.reserve(axes.size());

    for(auto& axe : axes)
    {
        shape1projections.push_back(project_points(v_a.data(), v_a.size(), axe));
        shape2projections.push_back(project_points(v_b.data(), v_b.size(), axe));

        if(!Overlap(shape1projections.back().x, shape1projections.back().y,
                    shape2projections.back().x, shape2projections.back().y))
            return false;

        const float overlapvalue =
            GetOverlap(shape1projections.back().x, shape1projections.back().y,
                       shape2projections.back().x, shape2projections.back().y);

        if(overlapvalue < smallestmagnitude)
        {
            smallestmagnitude = overlapvalue;
            smallestaxis      = axe;
        }
    }
    return true;
}

bool intersect_2D(const Vec2*   vertices_a,
                  unsigned int  vertices_a_size,
                  const Vec2*   vertices_b,
                  unsigned int  vertices_b_size,
                  const Matrix& m1,
                  const Matrix& m2,
                  const Vec2*   edges1,
                  unsigned int  edges_1_size,
                  const Vec2*   edges2,
                  unsigned int  edges_2_size,
                  const Matrix& mat1,    // without translation
                  const Matrix& mat2)
{
    Vec2 shape1projections;
    Vec2 shape2projections;

    float smallestmagnitude = std::numeric_limits<float>::max();

    for(auto i = 0u; i < edges_1_size; i++)
    {
        const auto& edge = edges1[i];

        // The project point doesn't really returns the projected point, but
        // directly a min/max value from the edge perspective?
        //
        //
        //  Shape 1                Shape 2
        //
        //		*
        //      |   *
        //   *  |   |                *
        //   |  |   |           *          *
        //   |  |   |				^Working on this edge
        //   *--*---*
        //  min    max
        //

        const auto e = (mat1 * edge).normalized();

        shape1projections = (project_points(vertices_a, vertices_a_size, m1, e));
        shape2projections = (project_points(vertices_b, vertices_b_size, m2, e));

        if(!Overlap(shape1projections.x, shape1projections.y, shape2projections.x,
                    shape2projections.y))
            return false;

        const float overlapvalue = GetOverlap(shape1projections.x, shape1projections.y,
                                              shape2projections.x, shape2projections.y);

        if(overlapvalue < smallestmagnitude)
        {
            smallestmagnitude = overlapvalue;
            //smallestaxis		= edge;
        }
    }

    for(auto i = 0u; i < edges_2_size; i++)
    {
        const auto& edge = edges2[i];

        // The project point doesn't really returns the projected point, but
        // directly a min/max value from the edge perspective?
        //
        //
        //  Shape 1                Shape 2
        //
        //		*
        //      |   *
        //   *  |   |                *
        //   |  |   |           *          *
        //   |  |   |				^Working on this edge
        //   *--*---*
        //  min    max
        //

        const auto e = (mat2 * edge).normalized();

        shape1projections = (project_points(vertices_a, vertices_a_size, m1, e));
        shape2projections = (project_points(vertices_b, vertices_b_size, m2, e));

        if(!Overlap(shape1projections.x, shape1projections.y, shape2projections.x,
                    shape2projections.y))
            return false;

        const float overlapvalue = GetOverlap(shape1projections.x, shape1projections.y,
                                              shape2projections.x, shape2projections.y);

        if(overlapvalue < smallestmagnitude)
        {
            smallestmagnitude = overlapvalue;
            //smallestaxis		= edge;
        }
    }
    return true;
}

float GetOverlap(float valmin1, float valmax1, float valmin2, float valmax2)
{
    if(valmin2 >= valmin1 && valmin2 <= valmax1)
    {
        float max = valmax2;
        if(valmax1 > valmax2)
            max = valmax1;
        return max - valmin2;
    }

    if(valmax2 >= valmin1 && valmax2 <= valmax1)
    {
        float min = valmin1;
        if(valmin2 > valmin1)
            min = valmin2;
        return valmax2 - min;
    }
    return 0.0f;
}

bool Overlap(float valmin1, float valmax1, float valmin2, float valmax2)
{
    if(valmin2 >= valmin1 && valmin2 <= valmax1)
        return true;

    if(valmax2 >= valmin1 && valmax2 <= valmax1)
        return true;

    if(valmin1 >= valmin2 && valmin1 <= valmax2)
        return true;

    if(valmax1 >= valmin2 && valmax1 <= valmax2)
        return true;

    return false;
}

bool intersect_with_mesh(
    const unsigned int* indexes,
    int                 indexes_size,
    const float*        vertices,    // Mesh's vertices
    int    offset,         // offset for the position attribute inside the vertice list
    int    vertex_size,    // Size of 1 vertex, not in bytes but in float
    Matrix model_matrix,
    Ray    ray,
    Vec3&  intersectionPoint,
    Vec3&  intersectionNormal)
{
    bool doesIntersect     = false;
    bool isClosestPointSet = false;

    for(int i = 0; i < indexes_size; i += 3)
    {
        Vec3 posA = model_matrix * Vec3(&vertices[indexes[i] * vertex_size + offset]);
        Vec3 posB = model_matrix * Vec3(&vertices[indexes[i + 1] * vertex_size + offset]);
        Vec3 posC = model_matrix * Vec3(&vertices[indexes[i + 2] * vertex_size + offset]);

        // On a besoin de la normale pour trouver l'équation du plan du triangle
        // Rappel, équation du plan : ax+by+cz+d=0
        Vec3 normal = get_triangle_normal(posA, posB, posC);

        float A = normal.x;
        float B = normal.y;
        float C = normal.z;

        // A partir de la normale, on connait les composantes a,b et c de l'équation ax+by+cz+d=0 du plan
        // On doit désormais trouver la composant D; Pour se faire, on s�lectionne un point de notre plan pour
        // remplir x,y et c et trouver d

        float D = -(posA.x * A + posA.y * B + posA.z * C);

        // On calcule maintenant les composants de l'�quation param�trique de la droite
        // à partir de son vecteur directeur

        float denom = ray.direction.x * A + ray.direction.y * B + ray.direction.z * C;

        // non  Colinarité

        if(denom != 0)
        {
            float t = -ray.start.x * A - ray.start.y * B - ray.start.z * C - D;
            t       = t / denom;

            Vec3 P(ray.start.x + t * ray.direction.x, ray.start.y + t * ray.direction.y,
                   ray.start.z + t * ray.direction.z);

            // We skip if the point is not inside the ray
            if(t <= 0)
                continue;

            if(t >= ray.length)
                continue;

            if(Vec3::point_in_triangle(posA, posB, posC, P))
            {
                if(!isClosestPointSet)
                {
                    isClosestPointSet  = true;
                    intersectionPoint  = P;
                    intersectionNormal = normal;
                }
                else
                {
                    if((P - ray.start).sqrt_length() <
                       (intersectionPoint - ray.start).sqrt_length())
                    {
                        intersectionPoint  = P;
                        intersectionNormal = normal;
                    }
                }
                doesIntersect = true;
            }
        }
    }
    return doesIntersect;
}

Vec3 project_points(const Vec3* points, const unsigned int points_size, const Vec3& axis)
{
    float mi  = points[0].dot(axis);
    float ma  = mi;
    float val = 0.0f;

    for(unsigned int i = 1u; i < points_size; i++)
    {
        val = points[i].dot(axis);
        if(val < mi)
            mi = val;

        if(val > ma)
            ma = val;
    }
    return Vec3(mi, ma, 0.0f);
}

Vec2 project_points(const Vec2*        points,
                    const unsigned int points_size,
                    const Matrix&      mat,
                    const Vec2&        axis)
{
    float mi  = (mat * points[0]).dot(axis);
    float ma  = mi;
    float val = 0.0f;

    for(auto i = 1u; i < points_size; i++)
    {
        val = (mat * points[i]).dot(axis);
        if(val < mi)
            mi = val;

        if(val > ma)
            ma = val;
    }
    return Vec2(mi, ma);
}

Vec3 get_triangle_normal(const Vec3& a, const Vec3& b, const Vec3& c)
{
    float b_ax = b.x - a.x;
    float b_ay = b.y - a.y;
    float b_az = b.z - a.z;

    float c_ax = c.x - a.x;
    float c_ay = c.y - a.y;
    float c_az = c.z - a.z;

    return Vec3((b_ay * c_az) - (b_az * c_ay), (b_az * c_ax) - (b_ax * c_az),
                (b_ax * c_ay) - (b_ay * c_ax))
        .normalized();

    //return (b - a).cross(c - a).normalized();
}

//TODO : I don't think I can significantly speed that up anymore
//		Maybe one way to make the raycast faster would be
//		to use threads where multiple raycast are computed at the same time?
bool intersect_with_collider(int                 offset,
                             int                 vertex_size,
                             const Matrix&       model_matrix,
                             const Matrix&       inverse,
                             const unsigned int* indexes,
                             int                 indexes_size,
                             const float*        vertices,
                             const Vec3*         normals,
                             const Ray&          ray,
                             Vec3&               intersection_point,
                             Vec3&               intersection_normal)
{
    bool does_intersect              = false;
    bool is_first_intersection_point = false;

    // We convert the ray into the model's space by using the inverse matrix
    // Why? Because the ray is only 2 Vec3.The mesh could have
    // hundreds of normals and vertices to transform in world space
    const Vec3 start     = inverse * ray.start;
    const Vec3 direction = inverse * ray.direction - inverse * Vec3::zero();

    // We loop through every triangle
    for(int i = 0; i < indexes_size; i += 3)
    {
        // We first check if the plane and the ray are collinear by
        // comparing the plane's normal with the ray's direction
        const Vec3& normal = normals[i / 3];

        // I don't remember why collinearity is checked that way here
        float denom =
            direction.x * normal.x + direction.y * normal.y + direction.z * normal.z;

        // If denom equals 0, both are collinear and so we can skip
        // the current triangle
        if(denom == 0)
            continue;

        // we use the ax+by+cz+d = 0 plane equation. We use 1 point
        // of the plane to compute the equation

        Vec3  posA(&vertices[indexes[i] * vertex_size + offset]);
        float D = -(posA.x * normal.x + posA.y * normal.y + posA.z * normal.z);

        // Reusing denom variable, was called t before
        denom =
            (-start.x * normal.x - start.y * normal.y - start.z * normal.z - D) / denom;

        // TODO : This is a big hack I'm really not sure it actually works lol
        // Well apparently it does work, this has been added to allow me to send negative
        // length value, although I could just flip the direction when I notice that length is negative too
        if(ray.length > 0.0f)
        {
            // We skip if the point is not inside the ray
            if(denom <= 0)
                continue;

            if(denom >= ray.length)
                continue;
        }
        else
        {
            if(denom >= 0)
                continue;

            if(denom <= ray.length)
                continue;
        }

        // P being the point that intersect between the plane
        // and the ray
        Vec3 P(start + (direction * denom));

        // We now check if the intersection point is inside the triangle

        Vec3 posB(&vertices[indexes[i + 1] * vertex_size + offset]);
        Vec3 posC(&vertices[indexes[i + 2] * vertex_size + offset]);

        if(Vec3::point_in_triangle(posA, posB, posC, P))
        {
            if(!is_first_intersection_point)
            {
                is_first_intersection_point = true;
                intersection_point          = P;
                intersection_normal         = normal;
            }
            else
            {
                if(denom < (intersection_point - start).length())
                {
                    intersection_point  = P;
                    intersection_normal = normal;
                }
            }
            does_intersect = true;
        }
    }

    if(does_intersect)
    {
        intersection_normal =
            model_matrix * intersection_normal - model_matrix * Vec3::zero();
        intersection_point = model_matrix * intersection_point;
    }
    return does_intersect;
}
}    // namespace corgi::math