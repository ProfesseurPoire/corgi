#pragma once

#include <corgi/ecs/System.h>
#include <corgi/ecs/ComponentPool.h>

#include <corgi/components/SpriteRenderer.h>
#include <corgi/components/Animator.h>

namespace corgi
{
	class AnimationSystem : public AbstractSystem
	{
	public:

		AnimationSystem(ComponentPool<Animator>& animators, ComponentPool<SpriteRenderer>& sprite_renderers)
			:animators_(animators),_sprite_renderers(sprite_renderers)
		{
			
		}

		void update_scaling_animation(Animator& animator, Entity& entity)
		{
			if (animator.scaling_animation_.is_playing)
			{
				if (animator.scaling_animation_.current_segment_timed_out())
				{
					if (!animator.scaling_animation_.next_segment())
					{
						animator.scaling_animation_.stop();
						return;
					}
				}

				entity.get_component<Transform>()->scale(animator.scaling_animation_.value());
				animator.scaling_animation_.current_tick++;
			}
		}

		void before_update(float elapsed_time) override
		{
			if(pause_)
				return;

			auto* animator_array = animators_.components().data();
			int size = animators_.components().size();
			
			for(int i=0; i< size; i++)
			{
				auto& animator = animator_array[i];
				
				if (!animator.enabled())
					continue;
				
				const auto e	= animators_.entity_id(i);
				auto& entity	= Game::instance().scene()._entities_contiguous[e.id_];
				
				update_scaling_animation(animator, entity);

				// If there's no animation
				if (animator._animations.empty())
					continue;

				// If the animator isn't running
				if (!animator.running)
					continue;

				// Converting the elapsed time in seconds into the animator.current_time ms
				animator.current_time += static_cast<unsigned>( elapsed_time*1000.0f);

				while(animator.current_time > animator.current_frame().time_)
				{
					animator.current_time -= animator.current_frame().time_;
					animator.next_frame();
				}
			}
		}
		
	    void update(float )override{}

		void pause()
		{
			pause_=true;
		}

		void unpause()
		{
			pause_=false;
		}

		void after_update(float ) override
		{
			if(pause_)
				return;

			auto animator_array		= animators_.components().data();
			int array_size			= animators_.components().size();
			
			for(int i=0; i< array_size; ++i)
			{
				auto& animator = animator_array[i];
				
				if (!animator.enabled())
					continue;
				
				auto& sprite	= _sprite_renderers.get(animators_.entity_id(i));
				
				sprite.flip_horizontal(animator.current_animation().flipped_x ^ animator.is_flipped_);
				sprite.sprite(animator.current_frame().sprite_);
			}
		}

	private:

		ComponentPool<Animator>& animators_;
		ComponentPool<SpriteRenderer>& _sprite_renderers;

		bool pause_{false};
		//Scene& _scene;
	};
}