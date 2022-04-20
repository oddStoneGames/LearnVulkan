#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer() {}
VulkanRenderer::~VulkanRenderer() {}

/// <summary>
/// Initializes The Vulkan Context With Instance, Devices and Validation Layers.
/// </summary>
/// <param name="window">GLFWwindow</param>
/// <returns></returns>
int VulkanRenderer::Initialize(GLFWwindow* window)
{
	//Cache GLFWwindow.
	m_Window = window;

	try {
		m_CreateInstance();
		if (!m_EnableValidationLayers)
			m_SetupDebugMessenger();
		m_GetPhysicalDevice();
		m_CreateLogicalDevice();
	}
	catch (const std::runtime_error& e)
	{
		printf("ERORR: %s", e.what());
		return EXIT_FAILURE;
	}
	return 0;
}

/// <summary>
/// Destroy The Vulkan Instance, Devices And Validation Layers.
/// </summary>
void VulkanRenderer::Destroy()
{
	if (m_EnableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
	}
	vkDestroyInstance(m_Instance, nullptr);
	vkDestroyDevice(m_MainDevice.device, nullptr);
}

bool VulkanRenderer::m_CreateInstance()
{
	// The Information about the application itself.
	// Most Stuff here doesn't affect the actual program & is for the developer convenience.
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan App";					// Your Application Name.
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);		// Your Application Version.
	appInfo.pEngineName = "Null Engine";						// Your Engine Name.
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);			// Your Engine Version.
	appInfo.apiVersion = VK_API_VERSION_1_3;					// Vulkan API Version.

	// Creation Info For A Vulkan Instance.
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Create List To Hold Instance Extenstions
	std::vector<const char*> instanceExtensions = std::vector<const char*>();

	// Set up the extensions GLFW will use.
	uint32_t glfwExtensionsCount = 0;			// GLFW May Require Multiple Extensions.
	const char** glfwExtensions;				// Extensions Passed as Array of cstrings, so need pointer(The Array) to pointer (cstring)

	// Get GLFW Extensions.
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

	// Add GLFW Extensions to the list of extensions.
	for (size_t i = 0; i < glfwExtensionsCount; i++)
		instanceExtensions.push_back(glfwExtensions[i]);

	// Check if We Support All The Extensions.
	if (!m_CheckInstanceExtensionSupport(&instanceExtensions))
		throw std::runtime_error("VkInstance Does not support required Extensions.");

	// Add Message Callback Extension if Validation Layer is Enabled.
	if (m_EnableValidationLayers)
		instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();

	// Create Validation Layers.
	if (m_EnableValidationLayers)
	{
		if(!m_CheckValidationLayersSupport())
			throw std::runtime_error("Validation Layers Requested, But Not Available!");

		createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		m_PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
	}

	// Create Instance.
	VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed To Create Vulkan Instance!");

	return false;
}

void VulkanRenderer::m_PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = m_DebugCallback;
}

void VulkanRenderer::m_SetupDebugMessenger()
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	m_PopulateDebugMessengerCreateInfo(createInfo);
	
	if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
		throw std::runtime_error("failed to set up debug messenger!");
	
}

void VulkanRenderer::m_CreateLogicalDevice()
{
	// Queues The Logical Device Needs (only 1 for now)
	QueueFamilyIndices indices = m_GetQueueFamilies(m_MainDevice.physicalDevice);
	VkDeviceQueueCreateInfo queueInfo = {};
	queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueInfo.queueFamilyIndex = indices.graphicsFamily;	// The Index of a family to create Queue From.
	queueInfo.queueCount = 1;
	float priority = 1.0f;
	queueInfo.pQueuePriorities = &priority;					// 1 = Highest Priority

	// Logical Device Creation Info.
	VkDeviceCreateInfo deviceInfo = {};
	deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceInfo.queueCreateInfoCount = 1;
	deviceInfo.pQueueCreateInfos = &queueInfo;
	deviceInfo.enabledExtensionCount = 0;
	deviceInfo.ppEnabledExtensionNames = nullptr;
	deviceInfo.enabledLayerCount = 0;

	// Physical Device Features.
	VkPhysicalDeviceFeatures deviceFeatures = {};
	vkGetPhysicalDeviceFeatures(m_MainDevice.physicalDevice, &deviceFeatures);

	deviceInfo.pEnabledFeatures = &deviceFeatures;

	// Finally Create Vulkan Logical Device.
	VkResult result = vkCreateDevice(m_MainDevice.physicalDevice, &deviceInfo, nullptr, &m_MainDevice.device);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Failed To Create Logical Device");

	// Queues are created as the same time as the device.
	// So we want to store them.
	vkGetDeviceQueue(m_MainDevice.device, indices.graphicsFamily, 0, &m_GraphicsQueue);
}

void VulkanRenderer::m_GetPhysicalDevice()
{
	// Enemurate Physical Devices VkInstance can Access.
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

	// If No Devices Available, No Vulkan Support.
	if (!deviceCount)
		throw std::runtime_error("No Physical Device(GPU) Found That Support Vulkan!");

	// Get List Of Physical Devices.
	std::vector<VkPhysicalDevice> deviceList(deviceCount);
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, deviceList.data());

	// Check if the Physical Device is Suitable.
	for (const auto& device : deviceList)
	{
		if (m_CheckPhysicalDeviceSuitable(device))
		{
			m_MainDevice.physicalDevice = device;
			break;
		}
	}
}

bool VulkanRenderer::m_CheckInstanceExtensionSupport(std::vector<const char*>* checkExtensions)
{
	// Need To Get Number Of Extensions To Create Array of correct size to hold extensions.
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	// Create a list of VkExtensionProperties using count.
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	// Check If Extensions are there.
	for (const auto& checkExtenion : *checkExtensions)
	{
		bool hasExtension = false;
		for (const auto& extension : extensions)
		{
			if (strcmp(checkExtenion, extension.extensionName))
			{
				hasExtension = true;
				break;
			}
		}

		if (!hasExtension)	return false;
	}

	return true;
}

bool VulkanRenderer::m_CheckPhysicalDeviceSuitable(VkPhysicalDevice device)
{
	// Get The Information About The Device Itself (ID, name, vendor, api version, driver version, etc)
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	// Print Device Properties
	printf("Api Version: %d\nDevice ID: %d\nDevice Name: %s\n\n",
		deviceProperties.apiVersion, deviceProperties.deviceID, deviceProperties.deviceName);

	// Get The Information About The Device Features (supports Geo shader, tess shader, wide lines etc)
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	//Get Queue Families.
	QueueFamilyIndices indices = m_GetQueueFamilies(device);

	return indices.IsValid();
}

bool VulkanRenderer::m_CheckValidationLayersSupport()
{
	// Get Supported Validation Layers Count.
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	// Get The Names Of All The Available Layers.
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	// Check if everything in Validation Layer Exists in Available Layers.
	for (const char* layerName : m_ValidationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) 
		{
			if (strcmp(layerName, layerProperties.layerName) == 0) 
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound) return false;
	}

	return true;
}

QueueFamilyIndices VulkanRenderer::m_GetQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	// Get The Queue Family Count From This Phsyical Device.
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	// Populate Data.
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	// Check if We Have Queue Families.
	int i = 0;	// The Index of Que Families.
	for (auto& queueFamily : queueFamilies)
	{
		// Check if The Current Queue Family has A Valid Queue Count & A Graphics Queue.
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;	// Set The Index Of Graphics Queue Family.

		// Stop if Queue Family is Already in a valid state.
		if (indices.IsValid()) break;

		// Continue
		i++;	
	}

	return indices;
}