#pragma once

namespace corgi
{
	namespace time
	{


		/*!
		 * @brief	As long as the counter is not reset, it will continue to register the
		 *			elapsed time since the last start() call.
		 *
		 *			tick() will also count how many time the tick() function has been called
		 */
		class Timer
		{
			long long   _nanoseconds{ 0 };

		public:

			void start();

			/*!
			 *  @brief  Returns the time in seconds
			 */
			[[nodiscard]] float elapsed_time()const noexcept;
		};
	}
}