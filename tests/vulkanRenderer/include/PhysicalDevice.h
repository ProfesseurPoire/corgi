#pragma once

#include <vulkan/vulkan_core.h>

#include <iostream>
#include <vector>

class PhysicalDevice
{
public:
    /*!
     *  Returns a vector with the available physical devices detected by Vulkan
     */
    static std::vector<PhysicalDevice> get_physical_devices(VkInstance instance);

    /**
     * @brief   For now we simply pick the first discrete GPU
     * 
     *          There's probably way better strategy to find the "best" gpu but that'll do
     *          for now
     * 
     * @param physical_devices 
     * @return VkPhysicalDevice 
     */

    static PhysicalDevice
    get_best_physical_device(const std::vector<PhysicalDevice>& physical_devices);

public:
    [[nodiscard]] VkPhysicalDevice vulkan_device() const { return vulkan_device_; }

    void print() const
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures   deviceFeatures;
        vkGetPhysicalDeviceProperties(vulkan_device_, &deviceProperties);
        vkGetPhysicalDeviceFeatures(vulkan_device_, &deviceFeatures);

        std::cout << "\t" << deviceProperties.deviceName << "\n";
        std::cout << "\t\t" << deviceProperties.apiVersion << "\n";
        std::cout << "\t\t" << deviceProperties.driverVersion << "\n";
        std::cout << "\t\t" << deviceProperties.vendorID << "\n";
        std::cout.flush();
    }
private:
    PhysicalDevice(VkPhysicalDevice device);
    VkPhysicalDevice vulkan_device_;
};
