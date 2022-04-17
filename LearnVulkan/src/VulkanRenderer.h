#pragma once

#include "GLFW/glfw3.h"
#include "Utilities.h"
#include <stdexcept>
#include <vector>

class VulkanRenderer
{
public:
	VulkanRenderer();
	~VulkanRenderer();

	// Intializes Vulkan Renderer Context.
	int Initialize(GLFWwindow* window);

	// Destroys The Vulkan Created Memory.
	void Destroy();
private:

	// Associated GLFWwindow.
	GLFWwindow* m_Window;

	// Vulkan Components
	// ------------------------------------------------
	VkInstance m_Instance;
	struct 
	{
		VkPhysicalDevice physicalDevice;
		VkDevice device;
	} m_MainDevice;
	VkQueue m_GraphicsQueue;


	// Vulkan Functions
	// ------------------------------------------------

	// - Create Functions
	// ------------------
	bool m_CreateInstance();
	void m_CreateLogicalDevice();

	// - Get Functions
	// ------------------
	void m_GetPhysicalDevice();

	// - Support Functions
	// ------------------
	bool m_CheckInstanceExtensionSupport(std::vector<const char*>* checkExtensions);
	bool m_CheckPhysicalDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices m_GetQueueFamilies(VkPhysicalDevice device);
};