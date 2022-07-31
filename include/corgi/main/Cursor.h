#pragma once

namespace corgi
{
    class Window;

    // The only reason this is a class instead of a namespace
    // is so I can use the friend mechanism to hide
    // init and finalize function while still being able
    // to call them in the correct object
    class Cursor
    {
    public:
        enum class DefaultCursor : int
        {
            Arrow     = 0,
            Wait      = 1,
            Crosshair = 2,
            WaitArrow = 3,
            SizeNWSE  = 4,
            SizeNESW  = 5,
            SizeWE    = 6,
            SizeNS    = 7,
            SizeAll   = 8,
            No        = 9,
            Hand      = 10
        };

        // Functions

        /**
		 * @brief 	Sets the current cursor, using one of the system default ones
		 * 
		 * @param cursor 
		 */
        static void set(DefaultCursor cursor);

        static DefaultCursor get();

        // TODO : I feel like the cursor should be tied to the game/application and not only to 1 window
        friend class corgi::Window;

        /**
		 * @brief Needs to be called to init every cursor
		 */
        static void init();

        /**
		 * @brief Needs to be called when exiting the application
		 */
        static void finalize();
    };
}    // namespace corgi
