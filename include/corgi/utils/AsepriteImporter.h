#pragma once

#include <corgi/resources/Animation.h>
#include <corgi/SimpleString.h>
#include <corgi/containers/Vector.h>

namespace corgi
{
	struct CorgiAnimationFrame
	{
        using Millisecond   = unsigned;
        using Pixel         = unsigned;

        Millisecond duration =0u;

        Pixel x = 0u;
        Pixel y = 0u;

        Pixel w = 0u;
        Pixel h = 0u;
	};

	enum class AnimationDirection : char
	{
		Forward, Reverse, PingPong
	};

    static const inline int max_animation_character=50;

	struct CorgiActualAnimation
	{
        int from;
        int to;
        AnimationDirection direction;
        char name[max_animation_character];
	};

    class Texture;
    class Animator;
    /*
     *  @brief  Loads the data contained inside the json files aseprite
     *          generates after exporting the sprite sheet
     */
    class AsepriteImporter
    {
    public:

        struct AnimationPair
        {
            AnimationPair(SimpleString p_name, Animation p_animation):
                name(p_name), animation(p_animation){}

            SimpleString name;
            Animation animation;
        };

        // Just so the operation isn't done in the constructor
        void load(const SimpleString& path);

		Vector<AnimationPair> animations(Texture* texture);

        struct SourceSize
        {
            int w, h;
        };

        struct SpriteSourceSize
        {
            int x, y, w, h;
        };

        struct Frame
        {
            unsigned x, y, w, h;
        };

        struct AsepriteFrame
        {
            using Millisecond = unsigned;

            Frame frame;
            SpriteSourceSize    sprite_source_size;
            SourceSize          source_size;
            bool rotated;
            bool trimmed;
            Millisecond duration = 0u;
        };

        struct FrameTag
        {
            SimpleString name;
            int from;
            int to;
            SimpleString direction;
        };

        struct Layer
        {
            SimpleString name;
            SimpleString blend_mode;
            int opacity;
        };

        struct Meta
        {
            SimpleString app;
            SimpleString version;
            SimpleString image;
            SimpleString format;
            SimpleString scale;
            Vector<Layer> layers;
            Vector<FrameTag> frame_tags;
            int w{-1};
            int h{-1};
        };

        Vector<AsepriteFrame> frames;
        Meta meta;
    };
}
