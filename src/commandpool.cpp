#include "commandpool.h"

namespace ToyEngine
{
	CommandPool::CommandPool(VkDevice const& device, uint32_t queueFamilyIndex, VkCommandPoolCreateFlagBits flag)
	{
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndex;
		//指令修改的属性、指令池的内存属性
		//VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT：指令池中的指令缓冲区可以被重新分配
		//VK_COMMAND_POOL_CREATE_TRANSIENT_BIT：指令池中的指令缓冲区是一次性的，适合一次性指令
		poolInfo.flags = flag;

		if (vkCreateCommandPool(device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create command pool.");
		}
	}

	CommandPool::~CommandPool()
	{
		if (m_commandPool != VK_NULL_HANDLE)
		{
			vkDestroyCommandPool(vkContext.vk_device, m_commandPool, nullptr);
		}
	}

	CommandPoolPtr CommandPool::create(VkDevice const& device,
		uint32_t queueFamilyIndex,
		VkCommandPoolCreateFlagBits flags)
	{
		return std::make_shared<CommandPool>(device, queueFamilyIndex, flags);
	}
} // ToyEngine