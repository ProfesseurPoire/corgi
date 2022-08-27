#pragma once

#include "UniformBufferObject.h"

#include <vector>

/**
 *      @brief  A Material is simply the class that contains the data that must be sent
 *              to the gpu
 */
struct Material
{
    std::vector<UniformBufferObject> ubos;
    std::vector<ImageView> image_view
};