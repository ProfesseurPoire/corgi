#pragma once

#include <corgi/resources/Mesh.h>

namespace corgi
{
	namespace UiUtils
	{
		/*!
		 * @brief	Returns the nine_slice_quad used by most ui elements
		 */
		Mesh& nine_slice_quad();

		/*!
		 * @brief	We need to call that in the game object to
		 *			clean up the default mesh
		 */
		void release_nineslice_mesh();
	}
}