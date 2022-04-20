#pragma once

#include "GLFW/glfw3.h"
#include "Utilities.h"
#include <stdexcept>
#include <vector>

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

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
	VkInstance m_Instance;		// --- > Vulkan Instance
	VkDebugUtilsMessengerEXT m_DebugMessenger; // --- > Vulkan Debug Messenger
	struct 
	{
		VkPhysicalDevice physicalDevice;	// --- > Physical Device (GPU)
		VkDevice device;					// --- > Vulkan Logical Device
	} m_MainDevice;
	VkQueue m_GraphicsQueue;				// --- > Vulkan Graphics Queue

	// Vulkan Standard Validation layer for error debugging only when in DEBUG configuration.
	const std::vector<const char*> m_ValidationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};
	#ifdef RELEASE
		const bool m_EnableValidationLayers = false;
	#else
		const bool m_EnableValidationLayers = true;
	#endif


	// Vulkan Functions
	// ------------------------------------------------

	// - Create Functions
	// ------------------
	bool m_CreateInstance();
	void m_CreateLogicalDevice();

	// - Get Functions
	// ------------------
	void m_GetPhysicalDevice();

	// - Callback Functions
	// ------------------
	static VKAPI_ATTR VkBool32 VKAPI_CALL m_DebugCallback
		(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		) 
	{

		printf("validation layer: %s\n",  pCallbackData->pMessage);

		return VK_FALSE;
	}

	// - Support Functions
	// ------------------
	void m_SetupDebugMessenger();
	void m_PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	bool m_CheckInstanceExtensionSupport(std::vector<const char*>* checkExtensions);
	bool m_CheckPhysicalDeviceSuitable(VkPhysicalDevice device);
	bool m_CheckValidationLayersSupport();
	QueueFamilyIndices m_GetQueueFamilies(VkPhysicalDevice device);
};