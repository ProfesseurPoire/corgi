#include "PhysicalDevice.h"

#include <vulkan/vulkan.h>

#include <algorithm>
#include <iostream>
#include <set>
#include <stdexcept>

static const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

static bool checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                             deviceExtensions.end());

    for(const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

PhysicalDevice PhysicalDevice::get_best_physical_device(
    const std::vector<PhysicalDevice>& physical_devices)
{
    //TODO : There's probably way better strategy to find the "best" gpu but that'll do
    //for now
    for(auto& physical_device : physical_devices)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures   deviceFeatures;
        vkGetPhysicalDeviceProperties(physical_device.vulkan_device_, &deviceProperties);
        vkGetPhysicalDeviceFeatures(physical_device.vulkan_device_, &deviceFeatures);

        if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            if(!checkDeviceExtensionSupport(physical_device.vulkan_device_))
            {
                throw std::runtime_error("Physical device has no swapchain support");
            }
            return physical_device;
        }
    }
    return nullptr;
}

void PhysicalDevice::print() const
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

std::vector<PhysicalDevice> PhysicalDevice::get_physical_devices(VkInstance instance)
{
    // We first ask vulkan how many device there are
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

    // We throw an exception if no Physical device could be found
    if(device_count == 0)
        throw std::runtime_error("No GPU found by Vulkan. Application will close");

    std::vector<VkPhysicalDevice> devices(device_count);
    devices.resize(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    std::vector<PhysicalDevice> physical_devices;

    // std back inserter needed otherwise we would need physical_device to be already set in size
    std::transform(devices.begin(), devices.end(), std::back_inserter(physical_devices),
                   [](const VkPhysicalDevice& vulkan_device)
                   { return PhysicalDevice(vulkan_device); });

    return physical_devices;
}

PhysicalDevice::PhysicalDevice(VkPhysicalDevice device)
{
    vulkan_device_ = device;
}