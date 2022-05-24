#pragma once

#include <corgi/SimpleString.h>

#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

#include <corgi/ecs/Component.h>
#include <corgi/ecs/RefEntity.h>
#include <corgi/ecs/EntityId.h>

#include <corgi/math/Vec2.h>

#include <corgi/utils/Event.h>
#include <corgi/utils/TimeHelper.h>

namespace corgi
{
	class Animator;

    class BlendTree
    {
    public:

        operator bool();

		void set_input(const Vec2& input);
		void add_point(float x, float y, const SimpleString& animation);
		void add_point(const Vec2& point, const SimpleString& animation);

		// Only chose the animation to play once
        void update();

		bool no_repeat_mode = false;

		RefEntity entity_;
		
		bool enabled_=false;

		const Vec2* _input = nullptr;
		std::vector<std::pair<Vec2, SimpleString>> _points;

    };

	enum class Operator
	{
		Equals,
		NonEquals
	};

	class Transition
	{
		friend class StateMachineSystem;
		friend class StateMachine;

	public:

		Transition(unsigned int state)
			: _new_state(state) {}

		Transition(Transition& tr)
		{
			_lambdas	= tr._lambdas;
			booleans_	= tr.booleans_;
			timers_		= tr.timers_;
			_new_state  = tr._new_state;
			
			comparisons_ = std::move(tr.comparisons_);
		}
		
		Transition& operator=(Transition& tr)
		{
			_lambdas	= tr._lambdas;
			booleans_	= tr.booleans_;
			timers_		= tr.timers_;
			_new_state	= tr._new_state;
			
			comparisons_ = std::move(tr.comparisons_);
			return *this;
		}

		/*!
		 * @brief	This function will make sure all conditions added to the
		 *			transition are true
		 */
		operator bool()const;

		/*!
		 * @brief	It's possible to add a condition as a lambda
		 */
		Transition& add_condition(std::function<bool()> condition);

		/*!
		 * @brief	You can add a condition that is simply a boolean value
		 */
		Transition& add_condition(const bool& condition);

		/*!
		 * @brief	You can use a timer as a condition. Once it returns true it will triggers the transition
		 */
		Transition& add_condition(const Timer& timer);

		class AbstractComparison
		{
		public:

			virtual explicit operator bool() = 0;
			virtual ~AbstractComparison() {}
		};

		template<class T>
		class VariableComparison : public AbstractComparison
		{
		public:

			VariableComparison(const T& value, Operator comparison_operator, const T& compaired_to)
				: value_(value), compaired_to_(compaired_to), comparison_operator_(comparison_operator)
			{

			}

			explicit operator bool()override
			{
				switch (comparison_operator_)
				{
				case Operator::Equals:
					return value_ == compaired_to_;
				case Operator::NonEquals:
					return value_ != compaired_to_;
				}
				return false;
			}

		private:

			const T& value_;
			T compaired_to_;
			Operator comparison_operator_;
		};

		std::vector<std::unique_ptr<AbstractComparison>> comparisons_;

		template<class T>
		Transition& add_condition(const T& value, Operator op, const T& comparison)
		{
			comparisons_.emplace_back(new VariableComparison<T>(value, op, comparison));
			return *this;
		}

	protected:

		std::vector<std::function<bool()>> _lambdas;
		std::vector<const bool*> booleans_;
		std::vector<const Timer*> timers_;

		unsigned int _new_state;
	};
	
	class State
	{
	public:

		friend class StateMachineSystem;
		friend class StateMachine;

		State() = default;
		State(unsigned int id);

		Transition& new_transition(unsigned int v);

		Event<>& on_exit_event();
		Event<>& on_enter_event();
		Event<>& on_update_event();

		[[nodiscard]] unsigned int id()const;

		BlendTree blend_tree;

	protected:

		// could probably just use a SimpleString here
		unsigned int id_;

		Event<> on_exit_;
		Event<> on_enter_;
		Event<> on_update_;

		std::vector<Transition> transitions_;
	};
	
    class StateMachine : public Component
    {
    public:
		
		void update();

        friend class StateMachineSystem;

	    State& new_state(int id);
	    State& current_state();
    	
		void current_state(int name);
		void transition_to(int name);

		void set_input(Vec2& vec2);
		void set_animator(Animator* animator);

		[[nodiscard]] int transitioning_to()const noexcept;

    protected:

		Vec2*		input_				= nullptr;
		Animator*	default_animator_	= nullptr;
		int			current_state_ = -1;
		int transitioning_to_ = -1;

		std::unordered_map<int, State>  states_;
    };
}
