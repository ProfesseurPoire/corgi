//#include "CameraSystem.h"
//
//#include <corgi/components/Transform.h>
//#include <corgi/components/Camera.h>
//#include <corgi/ecs/World.h>
//
//namespace corgi
//{
//    void CameraSystem::update(World& world, float elapsedTime)
//    {
//        auto transforms = world.get_component_pool<Transform>();
//        auto cameras	= world.get_component_pool<Camera>();
//
//        for(int entity_index : cameras->registered_components())
//        {
//            // Honestly I'm not even sure there's a gain
//            // doing what I'm doing since there's so much indirection
//            // First I loop through the registered_components
//            // Use the id through a register to then get the position of
//            // the transform inside the array
//
//            Transform&	transform	= *transforms->get_raw(entity_index);
//            Camera&		camera      = *cameras->get_raw(entity_index);
//
//            if (camera.is_dirty_)
//            {
//                if (camera.isOrthographic())
//                {
//                    camera.ortho
//                    (
//                        camera.orthographic_height_,
//                        camera.ratio_,
//                        camera.znear_,
//                        camera.zfar_
//                    );
//                }
//                camera.is_dirty_ = false;
//            }
//
//            transform.position(
//                transform.position().x,
//                transform.position().y,
//                0.0f
//            );
//        }
//    }
//}