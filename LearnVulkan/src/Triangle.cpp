#include "GLFW/glfw3.h"

#include "glm.hpp"
#include "GLM/mat4x4.hpp"

int main()
{
	glfwInit();
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(800, 600, "Learn Vulkan - Triangle", nullptr, nullptr);

	uint32_t vulkanExtensionsCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionsCount, nullptr);
	printf("Vulkan Extensions Count %i\n", vulkanExtensionsCount);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}