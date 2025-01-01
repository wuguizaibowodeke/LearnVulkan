#pragma once
#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <optional>
#include "base.h"

namespace ToyEngine
{
	class Context;
	using ContextPtr = std::shared_ptr<Context>;
	class Context final // final means that this class cannot be inherited from
	{
	 public:
		~Context();
		Context(const Context&) = delete; // delete means that this function cannot be called
		Context& operator=(const Context&) = delete;

		static ContextPtr create(bool enableValidationLayers);

		static void Init(bool enableValidationLayers, GLFWwindow* window);
		static void Quit();
		static Context& getInstance();

		void printAvailableExtensions();

		std::vector<const char*> getRequiredExtensions();

		//layer
		bool checkValidationLayerSupport();

		void setDebugger();

		int rateDeviceSuitability(VkPhysicalDevice device);

		bool isDeviceSuitable(VkPhysicalDevice device);

	 public:
		VkInstance vk_instance{ VK_NULL_HANDLE };

		VkPhysicalDevice vk_physicalDevice{ VK_NULL_HANDLE };

		//存储当前渲染任务队列族的id
		std::optional<uint32_t> vk_graphicsQueueFamilyIndex;
		VkQueue vk_graphicsQueue{ VK_NULL_HANDLE };

		std::optional<uint32_t> vk_presentQueueFamilyIndex;
		VkQueue vk_presentQueue{ VK_NULL_HANDLE };

		VkDevice vk_device{ VK_NULL_HANDLE };

		VkSurfaceKHR vk_surface{ VK_NULL_HANDLE };

	 private:
		explicit Context(bool enableValidationLayers , GLFWwindow* window);

		void createInstance();

		void pickPhysicalDevice();

		void createLogicalDevice();

		void queryQueueFamilyIndices();

		void getGraphicsQueue();

		void createSurface(GLFWwindow* window);

	 private:
		static std::unique_ptr<Context> m_instance;

		bool m_enableValidationLayers{ false };

		VkDebugUtilsMessengerEXT m_debugger{ VK_NULL_HANDLE };

		std::vector<const char*> m_instanceLayers;

		std::vector<const char*> m_instanceExtensions;

		std::vector<const char*> m_deviceRequiredExtensions;
	};

#define vkContext Context::getInstance()

} // toy2d

/*
 * VkInstanceCreateInfo
 * typedef struct VkInstanceCreateInfo {
    VkStructureType             sType;
    const void*                 pNext;
    VkInstanceCreateFlags       flags;
    const VkApplicationInfo*    pApplicationInfo;
    uint32_t                    enabledLayerCount;
    const char* const*          ppEnabledLayerNames;
    uint32_t                    enabledExtensionCount;
    const char* const*          ppEnabledExtensionNames;
	} VkInstanceCreateInfo;
 *  sType 是该结构体的类型枚举值，必须是VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO
 *  pNext 要么是 nullptr 要么指向其他结构体来扩展该结构体
 *  flags 是 VkInstanceCreateFlagBits 所表示的位域值，用于设置 VkInstance 的行为
 *  pApplicationInfo 要么是 nullptr，要么指向应用信息结构体，用于应用细节设置
 *  enabledLayerCount 是激活的layer的数量
 *  ppEnabledLayerNames 指向数量为 enabledLayerCount 的 layer 字符串数组，用于设置要激活的 layer
 *  enabledExtensionCount 是激活的扩展的数量
 *  ppEnabledExtensionNames 指向数量为 enabledExtensionCount 的扩展字符串数组，用于设置要激活的 instance 扩展
 */

/*
 * VkApplicationInfo
 * typedef struct VkApplicationInfo {
	VkStructureType    sType;
	const void*        pNext;
	const char*        pApplicationName;
	uint32_t           applicationVersion;
	const char*        pEngineName;
	uint32_t           engineVersion;
	uint32_t           apiVersion;
	} VkApplicationInfo;
 *  sType 是该结构体的类型枚举值，必须是VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO
 *  pNext 要么是 nullptr 要么指向其他结构体来扩展该结构体
 *  pApplicationName 要么是 NULL 要么指向一个以空字符为结尾的 UTF-8 字符串，用于表示用户自定义的应用名称
 *  applicationVersion 一个无符号整型,是应用程序的版本号
 *  pEngineName 要么是 NULL 要么指向一个以空字符为结尾的 UTF-8 字符串，用于表示用户自定义引擎的名称
 *  engineVersion 一个无符号整型,是用户自定义引擎版本号
 *  apiVersion 是应用打算使用的 Vulkan 的最高 核心 版本，并且忽略 apiVersion 的 patch 版本
 *
 *  其中 pApplicationName 、 applicationVersion 、pEngineName 和 engineVersion 这几个值随便设置，甚至可以不设置，赋为空都可以，这些参数不影响实例的创建。
 *	而 apiVersion 参数是最为重要的核心参数 ，当创建实例时，该参数用于指定此实例环境中 Vulkan 的 核心 版本 。
 */


