#pragma once

#include "base.h"

namespace ToyEngine
{
	class Buffer;
	using BufferPtr = std::shared_ptr<Buffer>;
	class Buffer
	{
	 public:
		static BufferPtr create(const VkDevice& device, VkPhysicalDevice const& physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

		Buffer(const VkDevice& device, VkPhysicalDevice const& physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

		~Buffer();

		void copyBuffer(const VkBuffer& srcBuffer,const VkBuffer& dstBuffer, VkDeviceSize size);

		[[nodiscard]] VkBuffer getBuffer() const;

		[[nodiscard]] VkDeviceMemory getBufferMemory() const;

		void updateBufferByMap(void* data, size_t size);

		void updateBufferByStage(void* data, size_t size);

	 private:
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	 private:
		VkBuffer m_buffer{ VK_NULL_HANDLE };
		VkDeviceMemory m_bufferMemory{ VK_NULL_HANDLE };
		VkDevice m_device{ VK_NULL_HANDLE };
		VkPhysicalDevice m_physicalDevice{ VK_NULL_HANDLE };
	};

} // ToyEngine


