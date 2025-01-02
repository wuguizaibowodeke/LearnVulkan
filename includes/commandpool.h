#pragma once

#include "base.h"
#include "context.h"
namespace ToyEngine
{
	class CommandPool;
	using CommandPoolPtr = std::shared_ptr<CommandPool>;
	class CommandPool
	{
	 public:
		CommandPool(const VkDevice& device,
			uint32_t queueFamilyIndex,
			VkCommandPoolCreateFlagBits flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		~CommandPool();

		static CommandPoolPtr create(const VkDevice& device,
			uint32_t queueFamilyIndex,
			VkCommandPoolCreateFlagBits flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		[[nodiscard]] VkCommandPool getCommandPool() const
		{
			return m_commandPool;
		}

	 private:
		VkCommandPool m_commandPool{ VK_NULL_HANDLE };
	};

} // ToyEngine

