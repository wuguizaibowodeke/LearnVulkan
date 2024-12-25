#include "context.h"
#include "logger.h"

namespace toy2d
{
	std::unique_ptr<Context> Context::m_instance = nullptr;

	Context::Context()
	{
		vk::InstanceCreateInfo createInfo;

		// Enable validation layers
		std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};

		/*获取所有的层名字
		auto layers = vk::enumerateInstanceLayerProperties();
		for(auto& layer : layers)
		{
			std::cout << layer.layerName << std::endl;
		}*/

		vk::ApplicationInfo appInfo;
		appInfo.setApiVersion(VK_API_VERSION_1_3);

		// This is the information that is used to create the instance.
		createInfo.setPApplicationInfo(&appInfo);
		createInfo.setPEnabledLayerNames(layers);
		// This instance is used to interface with the Vulkan API.
		instance = vk::createInstance(createInfo);
	}

	Context::~Context()
	{
		instance.destroy();
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

} // toy2d