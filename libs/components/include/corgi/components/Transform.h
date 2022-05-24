#pragma once

#include <corgi/math/Vec2.h>
#include <corgi/math/Vec3.h>
#include <corgi/math/Matrix.h>

namespace corgi
{
class Scene;

/*!
* @brief	Stores a transformation in world space
*			The transformation is then applied to the child of the entity owning this
*			transform and the vertices of a mesh
*/
class Transform 
{
	friend class TransformSystem;
	friend class Entity;

public:

	// Lifecycle

	Transform() = default;

	~Transform()
	{
		//std::cout << "checking out" << std::endl;
	}

	/*!
	* @brief	Sets the initial position of the transform object
	* 
	* 			More often than not, I end up setting up the position of 
	* 			of a transform object when initializing it
	*/
	Transform(float x, float y, float z = 0.0f);
	Transform(Vec3 v);

	Transform(const Transform& other);
	Transform(Transform&& other)noexcept;

	Transform& operator=(const Transform& other);
	Transform& operator=(Transform&& other)noexcept;

	// Functions

	/*!
	* @brief	Translate the current transform component
	*/
	void translate(float x, float y) noexcept;
	void translate(float x, float y, float z) noexcept;
	void translate(const Vec3& translation) noexcept;
	void translate(const Vec2& translation) noexcept;

	// Setters

	/*!
	* @brief	Sets the transform rotation value
	*/
	void euler_angles(const Vec3& angle) noexcept;
	void euler_angles(float x, float y, float z) noexcept;

	/*!
	* @brief	Sets the transform scale value 
	*/
	void scale(const Vec3& s) noexcept;
	void scale(float x, float y, float z) noexcept;

	/*!
	* @brief	Sets the transform position 
	*/
	void position(float x, float y) noexcept;
	void position(float x, float y, float z) noexcept;
	void position(const Vec3& pos) noexcept;
	void position(const Vec2& pos) noexcept;

	void is_world(bool v) noexcept;

	// Getters

	/*!
	* @brief Returns a matrix that combines translation, rotation, scale
	*/
	[[nodiscard]] const Matrix& matrix() const noexcept;

	/*!
	* @brief Returns the local matrix multiplied with its parents
	*/
	[[nodiscard]] const Matrix& world_matrix() const noexcept;

	/*!
	* @brief	Returns the world position of the current transform
	*			Warning : Actually compute the matrix and its parents if
	*			needed
	*/
	[[nodiscard]] Vec3 world_position() noexcept;

	/*
	* @brief	Returns the world position of the current transform
	*			Warning : Doesn't recompute the matrix. Mainly used for the sort function inside
	*			the renderer
	*/
	[[nodiscard]] Vec3 world_position() const noexcept;

	/*!
	* @brief Returns the scale value of the current transform
	*/
	[[nodiscard]] const Vec3& scale() const noexcept;

	/*!
	* @brief Returns the euler angles of the transform's rotation
	*/
	[[nodiscard]] const Vec3& euler_angles() const noexcept;

	/*!
	* @brief Returns the "local" position of the transform
	*/
	[[nodiscard]] const Vec3& position() const noexcept;

	/*!
	* @brief	Returns true if the current transformation should ignore its parent's transformation
	*/
	[[nodiscard]] bool is_world() const noexcept;

	int _depth=0;

	void enable()	{ _enabled = true; }
	void disable()	{ _enabled = false; }
	[[nodiscard]] bool enabled() const { return _enabled; }

private:

	void copy(const Transform& t);
	void set_dirty() noexcept;

	// There 4 bytes from a pointer to entity

	Vec3 _position;                       // 24 bytes
	Vec3 _euler_angles;                   // 36 bytes
	Vec3 _scale = Vec3(1.0f, 1.0f, 1.0f); // 48 bytes

	// TODO : Add code to sue custom matrix

	Matrix _world_matrix; // Matrix are 64 bytes	128

	bool _dirty         = true; // 129
	bool _inverse_dirty = true; // 130
	// If true, the transform doesn't apply the parent's transformation
	bool is_world_ = false; // +// 131 + some empty stuff
	bool _enabled = true;
};
}
