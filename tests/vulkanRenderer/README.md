## Things to know


To build the Vulkan test, we need to set the VULKAN_SDK_PATH to the location of the Vulkan SDK.

For vs code with the cmake extension installed, we do it by editing the .vscode/settings.json file 

```json
{
    "cmake.configureArgs": [
        "-DVULKAN_SDK_PATH=C:/VulkanSDK/1.3.216.0"
    ],
}
```



## How it actually works?

* First we need to find the vulkan extensions needed by SDL
* Then we need to find the validation layers (for debugging purposes)
* Some magic need to happens to set up the validation layers and have debug informations
* Need to create a vkInstance
* Need to get the GPU we're going to use through vkPhysicalDevice (usually the "best one")
* Find existing queues. Every operation in vulkan must be submitted to a queue
* We need to find the graphic queue. If there's no graphic queue we exit the application
* We then need to create a logical device VkDevice
* After that we need to create the surface to which we're going to draw
* We need to create the present queue
* SwapChain



## How to compile the shaders 

C:/VulkanSDK/x.x.x.x/Bin32/glslc.exe shader.vert -o vert.spv
C:/VulkanSDK/x.x.x.x/Bin32/glslc.exe shader.frag -o frag.spv
pause