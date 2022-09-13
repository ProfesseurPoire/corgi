target_sources(${PROJECT_NAME} PRIVATE
	gl/GLUniformBufferObject.cpp
	AbstractRenderer.cpp
	Color.cpp
	DrawList.cpp
	FrameBuffer.cpp
	Material.cpp
	PostProcessing.cpp
	Profiler.cpp
	RenderCommand.cpp
	renderer.cpp
	ShaderProgram.cpp
	Sprite.cpp
	texture.cpp
	VulkanRenderer.cpp
	WindowDrawList.cpp)

	add_subdirectory(vulkan)