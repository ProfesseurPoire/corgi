#pragma once

#include <corgi/utils/TimeHelper.h>

namespace corgi
{
	struct Profiler
	{
	// Accessors

		[[nodiscard]] float frames_per_seconds()const noexcept;
		[[nodiscard]] float update_time()const noexcept;
		[[nodiscard]] float render_time()const noexcept;

	// Functions
		
		void reset();
		void update();
		
		Counter update_counter_;
		Counter renderer_counter_;
		Counter loop_counter_;
		
		float refresh_rate_ = 0.85f;

	private:

		float frame_per_seconds_{0.0f};
		float update_time_{ 0.0f };
		float render_time_{ 0.0f };
	};
}
