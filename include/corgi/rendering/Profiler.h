#pragma once

namespace corgi
{
struct ProfilerInfo
{
    ProfilerInfo() {}

    /*!
	 * @brief Memory currently used on the GPU side in Kb
	 */
    unsigned gpu_memory_used = 0u;

    /*!
	 * @brief Total memory available on the GPU in kb
	 */
    unsigned gpu_memory_available = 0u;

    /*!
	 * @brief Draw calls have been done last time
	 */
    unsigned draw_calls = 0u;

    /*!
	 * @brief Vertices stored on the GPU
	 */
    unsigned vertices_count = 0u;

    /*!
	 * @brief Memory used by the GPU for the vertices
	 */
    unsigned vertices_size = 0u;

    /*!
	 * @brief Returns how many triangles are currently drawn
	 */
    unsigned triangle_count = 0u;
};
}    // namespace corgi
