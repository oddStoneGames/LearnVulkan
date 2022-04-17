#pragma region Includes

#include <iostream>
#include "GLFW/glfw3.h"

#include "VulkanRenderer.h"

#pragma endregion

#pragma region Globals

/// <summary>
/// The Current GLFW window we are Rendering in.
/// </summary>
GLFWwindow* window;

/// <summary>
/// The Current Vulkan Renderer we will be using.
/// </summary>
VulkanRenderer vulkanRenderer;

#pragma endregion

#pragma region Functions

#pragma region Initialize Window

/// <summary>
/// Initializes GLFW And Creates A GLFW Window.
/// </summary>
/// <param name="windowName">Window Name</param>
/// <param name="width">Window Width</param>
/// <param name="height">Window Height</param>
/// <returns>True if GLFW Initialized Successfully</returns>
bool InitWindow(std::string windowName = "Learn Vulkan", int width = 800, int height = 600)
{
	// Initialize GLFW!
	if (!glfwInit())
	{
		std::cout << "Failed To Initialize GLFW! Terminating...\n";
		return false;
	}

	// Make Sure we are not using any kind of OPENGL API
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// Disable Resizable Windows For Now Because it Needs Reinitializing a lot of Vulkan Stuff.
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Create Window
	window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

	return true;
}

#pragma endregion

#pragma region Main

int main()
{
	// Window Creation
	// ------------------------
	if (!InitWindow())
		return -1;

	//Create Vulkan Renderer Instance.
	if (vulkanRenderer.Initialize(window) == EXIT_FAILURE)
		return EXIT_FAILURE;

	// Render Loop
	// ------------------------
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	// Termination & Cleanup
	// ------------------------
	vulkanRenderer.Destroy();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;

}

#pragma endregion

#pragma endregion