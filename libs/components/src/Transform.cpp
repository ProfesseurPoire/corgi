#include <corgi/components/Transform.h>

using namespace corgi;

Transform::Transform(float x, float y, float z)
{
	translate(x, y, z);
}


Transform::Transform(Vec3 v)
{
	translate(v.x, v.y, v.z);
}

Transform::Transform(const Transform& other): _position(other._position),
	_euler_angles(other._euler_angles),
	_scale(other._scale),
	_world_matrix(other._world_matrix),
	_dirty(other._dirty),
	_inverse_dirty(other._inverse_dirty),
	is_world_(other.is_world_)
{}

Transform::Transform(Transform&& other) noexcept:
	_depth(other._depth),
	_position(other._position),
	_euler_angles(other._euler_angles),
	_scale(other._scale),
	_world_matrix(other._world_matrix),
	_dirty(other._dirty),
	_inverse_dirty(other._inverse_dirty),
	is_world_(other.is_world_)
{}

Transform& Transform::operator=(const Transform& other)
{
	_position      = other._position;
	_euler_angles  = other._euler_angles;
	_scale         = other._scale;
	_world_matrix  = other._world_matrix;
	_dirty         = other._dirty;
	_inverse_dirty = other._inverse_dirty;
	is_world_      = other.is_world_;
	_depth		   = other._depth;
	return *this;
}

Transform& Transform::operator=(Transform&& other) noexcept
{
	_position			= other._position;
	_euler_angles		= other._euler_angles;
	_scale				= other._scale;
	_world_matrix		= other._world_matrix;
	_dirty				= other._dirty;
	_inverse_dirty		= other._inverse_dirty;
	is_world_			= other.is_world_;
	_depth				= other._depth;
	return *this;
}

const Matrix& Transform::matrix() const noexcept
{
	return _world_matrix;
}

const Matrix& Transform::world_matrix() const noexcept
{
	return _world_matrix;
}

bool Transform::is_world() const noexcept
{
	return is_world_;
}

void Transform::is_world(bool v) noexcept
{
	is_world_ = v;
}

void Transform::scale(const Vec3& s) noexcept
{
	_scale = s;
}

Vec3 Transform::world_position() noexcept
{
	//TransformSystem::update_component(*this);
	return _world_matrix * Vec3(0.0f, 0.0f, 0.0f);
}

Vec3 Transform::world_position() const noexcept
{
	return _world_matrix * Vec3(0.0f, 0.0f, 0.0f);
}

void Transform::scale(const float x, const float y, const float z) noexcept
{
	_scale = Vec3(x, y, z);
}

void Transform::position(const float x, const float y) noexcept
{
	set_dirty(); // Transformations need to be computed
	_position = Vec3(x, y, _position.z);
}

void Transform::position(const float x, float y, float z) noexcept
{
	set_dirty(); // Transformation needs to be computed
	_position = Vec3(x, y, z);
}

void Transform::translate(const Vec2& translation) noexcept
{
	translate(translation.x, translation.y);
}

void Transform::position(const Vec3& pos) noexcept
{
	position(pos.x, pos.y, pos.z);
}

void Transform::position(const Vec2& pos) noexcept
{
	position(pos.x, pos.y);
}

const Vec3& Transform::scale() const noexcept
{
	return _scale;
}

const Vec3& Transform::euler_angles() const noexcept
{
	return _euler_angles;
}

const Vec3& Transform::position() const noexcept
{
	return _position;
}

// TODO : I probably don't handle switching parents

//void Transform::parent(Transform* new_parent)
//{
//	// special case where we switch both
//	if (new_parent != nullptr && new_parent->_parent == this)
//	{
//		new_parent->parent(this->_parent);
//		parent(new_parent);
//	}
//	else
//	{
//		if (_parent != nullptr)
//			_parent->remove_child(this);
//
//		if (new_parent != nullptr)
//		{
//			new_parent->_childs.add(this);
//			new_parent->_dirty = true;
//		}
//		_parent = new_parent;
//	}
//
//	set_dirty();
//}

void Transform::set_dirty() noexcept
{
	_dirty         = true;
	_inverse_dirty = true;
}

void Transform::translate(const float x, const float y) noexcept
{
	translate(Vec3(x, y, 0.0f));
}

void Transform::translate(const float x, const float y, const float z) noexcept
{
	translate(Vec3(x, y, z));
}

void Transform::translate(const Vec3& translation) noexcept
{
	position(_position + translation);
}

void Transform::euler_angles(const Vec3& angle) noexcept
{
	set_dirty();
	_euler_angles = angle;
}

void Transform::euler_angles(
	const float x,
	const float y,
	const float z) noexcept
{
	set_dirty();
	_euler_angles = Vec3(x, y, z);
}
