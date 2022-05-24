#pragma once

namespace corgi
{
	class Window;

	class Cursor
	{
	public:

		enum class DefaultCursor : int
		{
			Arrow		= 0,
			Wait		= 1,
			Crosshair	= 2,
			WaitArrow	= 3,
			SizeNWSE	= 4,
			SizeNESW	= 5,
			SizeWE		= 6,
			SizeNS		= 7,
			SizeAll		= 8,
			No			= 9,
			Hand		= 10
		};

	// Functions

		static void set(DefaultCursor cursor);
		static DefaultCursor get();

	private:

		friend class corgi::Window;

		static void init();
		static void finalize();
	};
}
