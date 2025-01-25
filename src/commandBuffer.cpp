#include "commandBuffer.h"

namespace ToyEngine
{
	CommandBufferPtr CommandBuffer::create(VkDevice const& device, const CommandPoolPtr& commandPool, bool asSecondary)
	{
		return std::make_shared<CommandBuffer>(device, commandPool, asSecondary);
	}

	CommandBuffer::CommandBuffer(VkDevice const& device, const CommandPoolPtr& commandPool, bool asSecondary)
	{
		m_commandPool = commandPool;
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandBufferCount = 1;
		allocInfo.commandPool = commandPool->getCommandPool();
		allocInfo.level = asSecondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		if (vkAllocateCommandBuffers(device, &allocInfo, &m_commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers.");
		}

	}

	CommandBuffer::~CommandBuffer()
	{
		if (m_commandBuffer != VK_NULL_HANDLE)
		{
			vkFreeCommandBuffers(vkContext.vk_device, m_commandPool->getCommandPool(), 1, &m_commandBuffer);
		}
	}

	void CommandBuffer::begin(const VkCommandBufferUsageFlags flags,
		const VkCommandBufferInheritanceInfo& inheritanceInfo)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = flags;
		beginInfo.pInheritanceInfo = &inheritanceInfo;

		if (vkBeginCommandBuffer(m_commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer.");
		}
	}

	void CommandBuffer::beginRenderPass(const VkRenderPassBeginInfo& renderPassInfo, const VkSubpassContents& contents)
	{
		vkCmdBeginRenderPass(m_commandBuffer, &renderPassInfo, contents);
	}

	void CommandBuffer::bindGraphicPipeline(VkPipeline const& pipeline)
	{
		vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	}

	void CommandBuffer::draw(uint32_t vertexCount)
	{
		vkCmdDraw(m_commandBuffer, vertexCount, 1, 0, 0);
	}

	void CommandBuffer::endRenderPass()
	{
		vkCmdEndRenderPass(m_commandBuffer);
	}

	void CommandBuffer::end()
	{
		if (vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer.");
		}
	}

	void CommandBuffer::copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, uint32_t copyInfoCount,
		const std::vector<VkBufferCopy>& copyInfos)
	{
		vkCmdCopyBuffer(m_commandBuffer, srcBuffer, dstBuffer, copyInfoCount, copyInfos.data());
	}

	void CommandBuffer::submitSync(VkQueue const& queue, VkFence const& fence)
	{
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandBuffer;

		if (vkQueueSubmit(queue, 1, &submitInfo, fence) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit command buffer.");
		}

		if (vkQueueWaitIdle(queue) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to wait queue idle.");
		}
	}
} // ToyEngine