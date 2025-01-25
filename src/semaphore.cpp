#include "semaphore.h"

namespace ToyEngine
{
	SemaphorePtr Semaphore::create(VkDevice const& device)
	{
		return std::make_shared<Semaphore>(device);
	}

	Semaphore::Semaphore(VkDevice const& device)
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_semaphore) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create semaphore.");
		}
	}

	Semaphore::~Semaphore()
	{
		if (m_semaphore != VK_NULL_HANDLE)
		{
			vkDestroySemaphore(vkContext.vk_device, m_semaphore, nullptr);
		}
	}

	VkSemaphore Semaphore::getSemaphore() const
	{
		return m_semaphore;
	}
} // ToyEngine