#include "context.h"
#include "logger.h"
#include "base.h"

namespace ToyEngine
{
	std::unique_ptr<Context> Context::m_instance = nullptr;

	ContextPtr Context::create(bool enableValidationLayers)
	{
		//return std::make_shared<Context>(enableValidationLayers);
		return nullptr;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		std::cout << "Validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

	static VkResult CreatDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

		if(func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	static void DestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT* pDebugMessenger,
		const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

		if(func != nullptr)
		{
			func(instance, *pDebugMessenger, pAllocator);
		}
	}

	Context::Context(bool enableValidationLayers, GLFWwindow* window)
		: m_enableValidationLayers(enableValidationLayers)
	{
		m_instanceLayers.push_back("VK_LAYER_KHRONOS_validation");

		m_deviceRequiredExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		//printAvailableExtensions();
		createInstance();
		pickPhysicalDevice();
		createSurface(window);
		queryQueueFamilyIndices();
		createLogicalDevice();
		getGraphicsQueue();

	}

	Context::~Context()
	{
		if(m_enableValidationLayers)
		{
			DestroyDebugUtilsMessengerEXT(vk_instance, &m_debugger, nullptr);
		}
		vkDestroySurfaceKHR(vk_instance, vk_surface, nullptr);
		vkDestroyDevice(vk_device, nullptr);
		vkDestroyInstance(vk_instance, nullptr);
	}

	Context& Context::getInstance()
	{
		return *m_instance;
	}

	void Context::Init(bool enableValidationLayers, GLFWwindow* window)
	{
		m_instance.reset(new Context(enableValidationLayers, window));
	}

	void Context::Quit()
	{
		m_instance.reset();
	}

	void Context::createInstance()
	{
		if(m_enableValidationLayers && !checkValidationLayerSupport())
		{
			LOG_E("Validation layers requested, but not available.");
			throw std::runtime_error("Validation layers requested, but not available.");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Toy2D";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Toy2D";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		//extensions
		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		//layer
		if(m_enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_instanceLayers.size());
			createInfo.ppEnabledLayerNames = m_instanceLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		if(vkCreateInstance(&createInfo, nullptr, &vk_instance) != VK_SUCCESS)
		{
			LOG_E("Failed to create Vulkan instance.");
			throw std::runtime_error("Failed to create Vulkan instance.");
		}
		else
		{
			LOG_I("Vulkan instance created successfully.");
		}

		setDebugger();
	}

	void Context::printAvailableExtensions()
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		LOG_I("Available extensions:");
		for(const auto& extension : extensions)
		{
			LOG_I("\t{}", extension.extensionName);
		}
	}

	std::vector<const char*> Context::getRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}

	bool Context::checkValidationLayerSupport()
	{
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for(const auto& layerName : m_instanceLayers)
		{
			bool layerFound = false;

			for(const auto& layerProperties : availableLayers)
			{
				if(std::strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if(!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	void Context::setDebugger()
	{
		if(!m_enableValidationLayers)
			return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;

		if(CreatDebugUtilsMessengerEXT(vk_instance, &createInfo, nullptr, &m_debugger) != VK_SUCCESS)
		{
			LOG_E("Failed to set up debugger.");
			throw std::runtime_error("Failed to set up debugger.");
		}
		else
		{
			LOG_I("Debugger set up successfully.");
		}
	}

	int Context::rateDeviceSuitability(VkPhysicalDevice device)
	{
		int score = 0;

		//设备名称、类型、支持的Vulkan版本等
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		//纹理压缩 浮点数运算特性 多视口渲染等
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}

		score += static_cast<int>(deviceProperties.limits.maxImageDimension2D);

		if(!deviceFeatures.geometryShader)
		{
			return 0;
		}

		return score;
	}

	bool Context::isDeviceSuitable(VkPhysicalDevice device)
	{
		//设备名称、类型、支持的Vulkan版本等
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		//纹理压缩 浮点数运算特性 多视口渲染等
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
	}

	void Context::pickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(vk_instance, &deviceCount, nullptr);

		if(deviceCount == 0)
		{
			LOG_E("Failed to find GPUs with Vulkan support.");
			throw std::runtime_error("Failed to find GPUs with Vulkan support.");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(vk_instance, &deviceCount, devices.data());

		std::multimap<int, VkPhysicalDevice> candidates;
		for(const auto& device : devices)
		{
			int score = rateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}

		if(candidates.rbegin()->first > 0 && isDeviceSuitable(candidates.rbegin()->second))
		{
			vk_physicalDevice = candidates.rbegin()->second;
		}

		if(vk_physicalDevice == VK_NULL_HANDLE)
		{
			LOG_E("Failed to find a suitable GPU.");
			throw std::runtime_error("Failed to find a suitable GPU.");
		}
	}

	void Context::queryQueueFamilyIndices()
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(vk_physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vk_physicalDevice, &queueFamilyCount, queueFamilies.data());

		for(uint32_t i = 0; i < queueFamilyCount; i++)
		{
			if(queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				vk_graphicsQueueFamilyIndex = i;
			}
			//寻找支持显示的队列族
			VkBool32 presentSupport = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(vk_physicalDevice, i, vk_surface, &presentSupport);
			if(presentSupport)
			{
				vk_presentQueueFamilyIndex = i;
			}

			if(vk_graphicsQueueFamilyIndex.has_value() && vk_presentQueueFamilyIndex.has_value())
			{
				break;
			}
		}

		if(!vk_graphicsQueueFamilyIndex.has_value() || !vk_presentQueueFamilyIndex.has_value())
		{
			LOG_E("Failed to find a suitable queue family.");
			throw std::runtime_error("Failed to find a suitable queue family.");
		}
	}

	void Context::createLogicalDevice()
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		std::set<uint32_t> queueFamilyIndices = { vk_graphicsQueueFamilyIndex.value(), vk_presentQueueFamilyIndex.value() };

		float queuePriority = 1.0f;
		for(uint32_t queueFamilyIndex : queueFamilyIndices)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceRequiredExtensions.size());
		createInfo.ppEnabledExtensionNames = m_deviceRequiredExtensions.data();

		if(m_enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_instanceLayers.size());
			createInfo.ppEnabledLayerNames = m_instanceLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		if(vkCreateDevice(vk_physicalDevice, &createInfo, nullptr, &vk_device) != VK_SUCCESS)
		{
			LOG_E("Failed to create logical device.");
			throw std::runtime_error("Failed to create logical device.");
		}
		else
		{
			LOG_I("Logical device created successfully.");
		}

		vkGetDeviceQueue(vk_device,vk_graphicsQueueFamilyIndex.value(), 0, &vk_graphicsQueue);
		vkGetDeviceQueue(vk_device, vk_presentQueueFamilyIndex.value(), 0, &vk_presentQueue);
	}

	void Context::getGraphicsQueue()
	{

	}

	void Context::createSurface(GLFWwindow* window)
	{
		if(glfwCreateWindowSurface(vk_instance, window, nullptr, &vk_surface) != VK_SUCCESS)
		{
			LOG_E("Failed to create window surface.");
			throw std::runtime_error("Failed to create window surface.");
		}
		else
		{
			LOG_I("Window surface created successfully.");
		}
	}

} // toy2d