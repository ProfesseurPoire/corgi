#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

class PhysicalDevice
{
    friend class VulkanRenderer;

public:
    /*!
     *  Returns a vector with the available physical devices detected by Vulkan
     */
    static std::vector<PhysicalDevice> get_physical_devices(VkInstance instance);

    /**
     * @brief   Picks the best PhysicalDevice from a list of PhysicalDevice
     * @param physical_devices 
     * @return VkPhysicalDevice 
     */
    static PhysicalDevice
    get_best_physical_device(const std::vector<PhysicalDevice>& physical_devices);

public:
    [[nodiscard]] VkPhysicalDevice vulkan_device() const { return vulkan_device_; }

    void print() const;

private:
    PhysicalDevice(VkPhysicalDevice device);
    PhysicalDevice() = default;

    VkPhysicalDevice vulkan_device_;
};
