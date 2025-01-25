#include "fence.h"

ToyEngine::FencePtr ToyEngine::Fence::create(VkDevice const& device, bool signaled)
{
	return std::make_shared<Fence>(device, signaled);
}

ToyEngine::Fence::Fence(VkDevice const& device, bool signaled)
{
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	if (vkCreateFence(device, &fenceInfo, nullptr, &m_fence) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create fence.");
	}
}

ToyEngine::Fence::~Fence()
{
	if (m_fence != VK_NULL_HANDLE)
	{
		vkDestroyFence(vkContext.vk_device, m_fence, nullptr);
	}
}

VkFence ToyEngine::Fence::getFence() const
{
	return m_fence;
}

void ToyEngine::Fence::resetFence()
{
	vkResetFences(vkContext.vk_device, 1, &m_fence);
}

void ToyEngine::Fence::block(uint64_t timeout)
{
	vkWaitForFences(vkContext.vk_device, 1, &m_fence, VK_TRUE, timeout);
}
