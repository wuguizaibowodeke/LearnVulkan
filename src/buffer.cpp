#include "buffer.h"
#include "context.h"
#include "commandpool.h"
#include "commandBuffer.h"

namespace ToyEngine
{
	BufferPtr Buffer::create(VkDevice const& device,
		VkPhysicalDevice const& physicalDevice,
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties)
	{
		return std::make_shared<Buffer>(device, physicalDevice,  size, usage, properties);
	}

	Buffer::Buffer(VkDevice const& device,
		VkPhysicalDevice const& physicalDevice,
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties)
	{
		m_device = device;
		m_physicalDevice = physicalDevice;
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create buffer.");
		}

		VkMemoryRequirements memRequirements{};
		vkGetBufferMemoryRequirements(device, m_buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &m_bufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate buffer memory.");
		}

		vkBindBufferMemory(device, m_buffer, m_bufferMemory, 0);
	}

	Buffer::~Buffer()
	{
		if(m_buffer != VK_NULL_HANDLE)
		{
			vkDestroyBuffer(m_device, m_buffer, nullptr);
		}
		if(m_bufferMemory != VK_NULL_HANDLE)
		{
			vkFreeMemory(m_device, m_bufferMemory, nullptr);
		}
	}

	void Buffer::copyBuffer(const VkBuffer& srcBuffer,const VkBuffer& dstBuffer, VkDeviceSize size)
	{
		auto commandPool = CommandPool::create(m_device, vkContext.vk_graphicsQueueFamilyIndex.value());
		auto commandBuffer = CommandBuffer::create(m_device, commandPool);

		commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		commandBuffer->copyBuffer(srcBuffer, dstBuffer, 1, {copyRegion});

		commandBuffer->end();

		commandBuffer->submitSync(vkContext.vk_graphicsQueue, VK_NULL_HANDLE);
	}

	VkBuffer Buffer::getBuffer() const
	{
		return m_buffer;
	}

	VkDeviceMemory Buffer::getBufferMemory() const
	{
		return m_bufferMemory;
	}

	uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

		for(uint32_t i = 0; i < memProperties.memoryTypeCount;++i)
		{
			if((typeFilter & (1 << i)) && ((memProperties.memoryTypes[i].propertyFlags & properties) == properties))
			{
				return i;
			}
		}

		throw std::runtime_error("");
	}

	void Buffer::updateBufferByMap(void* data, size_t size)
	{
		void* mappedData = nullptr;
		vkMapMemory(m_device, m_bufferMemory, 0, size, 0, &mappedData);
		memcpy(mappedData, data, size);
		vkUnmapMemory(m_device, m_bufferMemory);
	}

	void Buffer::updateBufferByStage(void* data, size_t size)
	{
		BufferPtr staging = Buffer::create(m_device, m_physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		staging->updateBufferByMap(data, size);
		copyBuffer(staging->getBuffer(), m_buffer, static_cast<VkDeviceSize>(size));
	}
} // ToyEngine