target_sources(Pixelizer-Lib PRIVATE
        VulkanSystem.cpp
        Pipeline.cpp
        SwapChain.cpp
        Device.cpp
        Instance.cpp
        PhysicalDevice.cpp
        ShaderModule.cpp
        Surface.cpp
        VulkanUtils.cpp)

add_subdirectory(CommandBuffer)
add_subdirectory(Pipeline)
add_subdirectory(Descriptors)
add_subdirectory(Memory)
add_subdirectory(Queue)
add_subdirectory(Synchronization)