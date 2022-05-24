#pragma once

#include <corgi/rendering/Sprite.h>
#include <corgi/containers/Vector.h>
#include <corgi/SimpleString.h>

namespace corgi
{
	struct Animation 
	{
	    struct Frame
	    {
			using Millisecond = unsigned;

			Frame(Sprite sprite, unsigned time)
				:sprite_(sprite),time_(time)
			{}

			Sprite sprite_;
			Millisecond time_ { 0 };
	    };

		[[nodiscard]] const char* name()const
		{
			return name_.c_str();
		}

		void name(const char* name)
		{
			name_ = name;
		}

		// I added this constructor because otherwise I get a warning for padding initialization
		Animation() : frames(), speed(1.0f), name_("animation"), looping(false), flipped_x(false), padding { false,false }{}

	    Vector<Frame>		frames;							// 12 
		float               speed       {1.0f};				// 4
		SimpleString		name_;							// 4
	    bool                looping     {false};			// 1 
	    bool                flipped_x   {false};			// 1

		bool padding[2]={};	// Just to remove the C4820 warning and also reminds me that there's a padding problem

		// Total : 26
		// True total : 28
	};
}
