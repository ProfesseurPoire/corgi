#include <corgi/components/StateMachine.h>

#include <corgi/components/Animator.h>
#include <corgi/ecs/Entity.h>

#include <corgi/logger/log.h>

namespace corgi
{
	void BlendTree::set_input(const Vec2& input)
	{
		_input = &input;
	}

	void BlendTree::add_point(const Vec2& point, const SimpleString& animation)
	{
		_points.emplace_back(point, animation);
	}

	void BlendTree::add_point(float x, float y, const SimpleString& animation)
	{
		_points.emplace_back(Vec2(x, y), animation);
	}

    BlendTree::operator bool() { return enabled_;}

    State::State(unsigned id) :  id_(id){}

    Transition& State::new_transition(unsigned int name)
	{
		transitions_.reserve(20);
		return transitions_.emplace_back(name);
	}

    Event<>& State::on_exit_event()  	{ return on_exit_; }
    Event<>& State::on_enter_event() 	{ return on_enter_;}
    Event<>& State::on_update_event()	{ return on_update_;}

	SimpleString find_closest_point(std::vector<std::pair<Vec2, SimpleString>>& points, Vec2 input)
	{
		float sum = std::numeric_limits<float>::max();

		SimpleString selected;

		for (auto [point, animation] : points)
		{
			Vec2 v = (point - input);
			if (v.length() < sum)
			{
				sum = v.length();
				selected = animation;
			}
		}
		return selected;
	}

    void BlendTree::update()
    {
		if(!entity_)
		{
			log_warning("No Entity attached to the BlendTree");
			return;
		}

		if(no_repeat_mode)
		{
			log_warning("BlendTree in no_repeat_mode");
			return;
		}

		if(!_input)
		{
			log_warning("No input attached to the state machine");
			return;
		}
		
		// No warning are logged here because this is the expected behavior
		if(_input->length()==0 )	
		{
			return;
		}

		// The blendtree needs an animator component associated to it
		if(!entity_->has_component<Animator>())
		{
			log_warning("The Blendtree entity has no Animator component");
			return;
		}

		// get the animation linked to the closest point 
		SimpleString selected = find_closest_point(_points, *_input);

		auto animator =  entity_->get_component<Animator>();

		if (animator->current_animation().name() != selected)
			animator->play(selected, 0, 0);
    }

    Transition::operator bool()const
    {
        bool v = true;

		for (auto& lambda : _lambdas)
		{
			v = v && lambda();
		}

		for (auto& boolean : booleans_)
		{
			v = v && *boolean;
		}

		for(auto* timer : timers_)
		{
			v = v && (*timer).operator bool();
		}

		for(auto& comparison : comparisons_)
		{
			v = v && (*comparison).operator bool();
		}
        return v;
    }

	Transition& Transition::add_condition(std::function<bool()> condition)
	{
		_lambdas.push_back(condition);
		return *this;
	}

	Transition& Transition::add_condition(const Timer& timer)
	{
		timers_.push_back(&timer);
		return *this;
	}

    Transition& Transition::add_condition(
	    const bool& condition)
    {
		booleans_.push_back(&condition);
		return *this;
    }


    State& StateMachine::current_state()
    {
        return states_[current_state_];
    }

    void StateMachine::current_state(int name)
    {
		current_state_ = name;
    }

    void StateMachine::set_animator(Animator* animator)
	{
		default_animator_ = animator;
	}

    int StateMachine::transitioning_to() const noexcept
    {
		return transitioning_to_;
    }

    State& StateMachine::new_state(int id)
    {
		states_.try_emplace(id, id);
		current_state_ = id;

		if(input_)
		{
			//current_state_->blend_tree.set_input(*input_);
		}

		if (default_animator_)
		{
			//current_state_->blend_tree.set_animator(default_animator_);
		}
        return states_[id];
    }

    unsigned int State::id() const
    {
		return id_;
    }

    void StateMachine::set_input(Vec2& vec2)
	{
		input_ = &vec2;
	}

	void StateMachine::transition_to(int state_id)
	{
		transitioning_to_ = state_id;
    	
		current_state().on_exit_();
		current_state_ = state_id;
		current_state().on_enter_();
	}

    void StateMachine::update()
	{
		if (current_state_ != -1)
		{
			current_state().on_update_();

			// First we handle the transitions
			for (auto& transition : current_state().transitions_)
			{
				if (transition) // Checks if the transition's conditions are true
				{
					transition_to(transition._new_state);
					break;
				}
			}

			if(current_state().blend_tree)
			{
				current_state().blend_tree.update();
			}
		}
	}
}