//#include "StateMachineSystem.h"
//
//#include <corgi/ecs/World.h>
//#include <corgi/components/StateMachine.h>
//
//namespace corgi
//{
//    void StateMachineSystem::update(World& g, float elapsed_time)
//    {
//        auto pool = g.get_component_pool<StateMachine>();
//
//        for (auto& entity_index : pool->registered_components())
//        {
//            auto stateMachine = pool->get(entity_index);
//            
//            if (stateMachine->currentState != nullptr)
//            {
//                if (stateMachine->currentState->blend_tree)
//                    stateMachine->currentState->blend_tree.update();
//
//                if (stateMachine->currentState->on_update_)
//                    stateMachine->currentState->on_update_(elapsed_time);
//
//                auto* cs = stateMachine->currentState;
//
//                for (auto& transition : stateMachine->currentState->transitions_)
//                {
//                    if (*transition) // Checks if the transition's conditions are true
//                    {
//                        if (stateMachine->currentState->on_exit_)
//                            stateMachine->currentState->on_exit_(elapsed_time);
//
//                        stateMachine->currentState = &transition->newState;
//
//                        if (stateMachine->currentState->on_enter_)
//                            stateMachine->currentState->on_enter_(elapsed_time);
//                    }
//                }
//            }
//        }
//    }
//}