#pragma once

namespace corgi::random
{
	/*!
	 * @brief	Initialize the Random Generator with a specific seed
	 */
	void initialize(int seed);

	/*!
	 * @brief	Returns a random integer number between min and max
	 */
	int range(int min, int max);

	/*!
	 *@brief	Returns a random unsigned integer number 
	 */
	unsigned uinteger();

	/*!
	 * @brief	Returns a random integer number between 0 and max
	 */
	int range(int max);

	/*!
	 * @brief	Returns a random float number between min and max
	 */
	float range(float min, float max);

	/*!
	 * @brief	Returns a random float number between 0.0f and max
	 */
	float range(float max);

	/*!
	 *	@brief	Returns a random float number between 0.0f and 1.0f
	 */
	float real_value();


	// TODO : Move this in vec3 one day, but not now

	///*!
	// * @brief	Returns a random vector 
	// */
	//Vec3 random_vec3(float length = 1.0f);
}
