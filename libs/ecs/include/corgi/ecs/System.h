#pragma once

namespace corgi
{
	/*!
	 * @brief	New Systems will have to inherit from this class and implement its
	 *			virtual functions
	 */
	class AbstractSystem
	{
	public:

		friend class Scene;
		
		virtual ~AbstractSystem()=default;

	protected :

		virtual void before_update(float /*elapsed_time*/) {}
		virtual void update(float /*elapsed_time*/) {}
		virtual void after_update(float /*elapsed_time*/) {}
	};
}
