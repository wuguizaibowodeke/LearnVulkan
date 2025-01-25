#pragma once

#include "base.h"
#include "context.h"
#include "commandpool.h"

namespace ToyEngine
{
	class CommandBuffer;
	using CommandBufferPtr = std::shared_ptr<CommandBuffer>;
	class CommandBuffer
	{
	 public:
		static CommandBufferPtr create(const VkDevice& device, const CommandPoolPtr& commandPool,
			bool asSecondary = false);

		CommandBuffer(const VkDevice& device, const CommandPoolPtr& commandPool,
			bool asSecondary = false);

		~CommandBuffer();

		//VkCommandBufferUsageFlags
		//VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT：这个标志位指示 这个命令只会被使用提交一次
		//VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT：这个命令buffer 是一个二级缓冲 buffer，用于一个renderpass
		//VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT：这个标志位指示这个命令buffer可以被多个queue同时使用

		//VkCommandBufferInheritanceInfo 如果本命令是二级buffer，那么这个结构体记录了他所属的主命令信息/继承信息
		void begin(const VkCommandBufferUsageFlags flags = 0,
			const VkCommandBufferInheritanceInfo& inheritanceInfo = {});

		void beginRenderPass(const VkRenderPassBeginInfo& renderPassInfo,
			const VkSubpassContents& contents = VK_SUBPASS_CONTENTS_INLINE);

		void bindGraphicPipeline(const VkPipeline& pipeline);

		void draw(uint32_t vertexCount);

		void endRenderPass();

		void end();

		[[nodiscard]] VkCommandBuffer getCommandBuffer() const
		{
			return m_commandBuffer;
		}

		void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, uint32_t copyInfoCount,
			const std::vector<VkBufferCopy>& copyInfos);

		void submitSync(const VkQueue& queue, const VkFence& fence);

	 private:
		VkCommandBuffer m_commandBuffer{ VK_NULL_HANDLE };
		CommandPoolPtr m_commandPool{ nullptr };
	};

} // ToyEngine


