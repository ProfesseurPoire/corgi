//#include "AnimatorSystem.h"
//
//#include <corgi/ecs/World.h>
//#include <corgi/components/Sprite.h>
//#include <corgi/components/Animator.h>
//
//namespace corgi
//{
//    void set_key_frame(Animator& animator, Sprite& sprite, int keyframeIndex)
//    {
//        sprite.textureOffsetX		    = animator.current_animation->frames[keyframeIndex].x;
//        sprite.textureOffsetY		    = animator.current_animation->frames[keyframeIndex].y;
//        sprite.textureWidth			    = animator.current_animation->frames[keyframeIndex].width;
//        sprite.textureHeight		    = animator.current_animation->frames[keyframeIndex].height;
//        animator.current_frame_index    = keyframeIndex;
//    }
//
//    void update_component(Animator& animator, Sprite& sprite, float elapsed_time)
//    {
//        if (animator.start)
//        {
//            animator.current_time = 0.0f;
//            animator.running = true;
//            animator.start = false;
//            set_key_frame(animator, sprite, 0);
//            return;
//        }
//
//        if (!animator.running)
//            return;
//        
//        // Just a few shortcuts to make the thing more readable
//        Animation&  animation           = *animator.current_animation;
//        int&        current_key_frame   = animator.current_frame_index;
//        float&      current_time        = animator.current_time;
//        Animation::Frame& key_frame     = animation.frames[current_key_frame];
//
//        sprite.flipX = animation.flipped_x;
//        current_time += elapsed_time;
//
//        if (current_time > key_frame.time * animation.speed)
//        {
//            current_time = current_time - (key_frame.time* animation.speed);
//            current_key_frame++;
//
//            // If we displayed every keyframe
//            if (current_key_frame == animation.frames.size())
//            {
//                // If we're looping, we start back from the first frame
//                if (animation.looping)
//                    set_key_frame(animator, sprite, 0);
//                else
//                    animator.running = false;   // stops the animation otherwise
//            }
//            else
//                set_key_frame(animator, sprite, current_key_frame);
//        }
//    }
//
//    void AnimatorSystem::update(World& game, float elapsedTime)
//    {
//        // I really don't like much having to fetch the pools and stuff D:
//        auto animators	= game.get_component_pool<Animator>();
//        auto sprites	= game.get_component_pool<Sprite>();
//
//        for(int entity_id : animators->registered_components())
//        {
//            Animator& animator	= *animators->get_raw(entity_id);
//            Sprite& sprite		= *sprites->get_raw(entity_id);
//
//            if (animator.animations.empty())
//                return;
//
//            assert(sprites->exist(entity_id)&&
//                "An Animator component needs a sprite component");
//            
//            update_component(animator, sprite, elapsedTime);
//        }
//    }
//}