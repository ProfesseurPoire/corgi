
# How to install

mkdir conan
conan install ../ --build=missing

Don't forget to set the VULKAN_SDK_PATH to where you installed your vulkan version.

add the following line in the CMakeSettings.json if you're using visual studio

```cpp
"cmakeCommandArgs": "-DVULKAN_SDK_PATH=C:\\VulkanSDK\\1.3.216.0", 
```