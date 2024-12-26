#include "context.h"
#include "logger.h"
#include "tool.h"

#include <iostream>

namespace ToyEngine
{
	std::unique_ptr<Context> Context::m_instance = nullptr;

	Context::Context()
	{
		createInstance();
		pickPhysicalDevice();
		queryQueueFamilyIndices();
		createLogicalDevice();
		getGraphicsQueue();
	}

	Context::~Context()
	{
		vk_device.destroy();
		vk_instance.destroy();
	}

	Context& Context::getInstance()
	{
		return *m_instance;
	}

	void Context::Init()
	{
		m_instance.reset(new Context());
	}

	void Context::Quit()
	{
		m_instance.reset();
	}

	void Context::createInstance()
	{
		vk::InstanceCreateInfo createInfo;
		vk::ApplicationInfo appInfo;
		appInfo.setApiVersion(VK_API_VERSION_1_3);
		createInfo.setPApplicationInfo(&appInfo);
		vk_instance = vk::createInstance(createInfo);

		/*获取所有的层名字
		auto layers = vk::enumerateInstanceLayerProperties();
		for(auto& layer : layers)
		{
		std::cout << layer.layerName << std::endl;
		}*/

		std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };

		auto fun = [](const char* e1, const vk::LayerProperties& e2)
		{
		  return std::strcmp(e1, e2.layerName) == 0;
		};

		removeNotSupportedElems<const char*, vk::LayerProperties>(layers,
			vk::enumerateInstanceLayerProperties(),
			fun);
		createInfo.setPEnabledLayerNames(layers);

		vk_instance = vk::createInstance(createInfo);
	}

	void Context::pickPhysicalDevice()
	{
		auto devices = vk_instance.enumeratePhysicalDevices();
		if (devices.empty())
		{
			LOG_E("No physical device found");
			return;
		}

		// 选择第一个物理设备
		vk_physicalDevice = devices[0];
		std::cout << "Physical device: " << vk_physicalDevice.getProperties().deviceName << std::endl;
	}

	void Context::createLogicalDevice()
	{
		vk::DeviceCreateInfo createInfo;
		vk::DeviceQueueCreateInfo queueCreateInfo;
		float priority = 1.0f;
		queueCreateInfo.setPQueuePriorities(&priority)
						.setQueueCount(1)
						.setQueueFamilyIndex(vk_queueFamilyIndices.graphicsQueue.value());

		createInfo.setQueueCreateInfos(queueCreateInfo);
		vk_device = vk_physicalDevice.createDevice(createInfo);
	}

	void Context::queryQueueFamilyIndices()
	{
		auto queueFamilyProperties = vk_physicalDevice.getQueueFamilyProperties();
		for(int i = 0; i < queueFamilyProperties.size(); i++)
		{
			const auto& property = queueFamilyProperties[i];
			if(property.queueFlags & vk::QueueFlagBits::eGraphics)
			{
				vk_queueFamilyIndices.graphicsQueue = i;
				break;
			}
		}

		if(!vk_queueFamilyIndices.graphicsQueue.has_value())
		{
			LOG_E("No graphics queue found");
			return;
		}
	}

	void Context::getGraphicsQueue()
	{
		vk_graphicsQueue = vk_device.getQueue(vk_queueFamilyIndices.graphicsQueue.value(), 0);
	}

} // toy2d