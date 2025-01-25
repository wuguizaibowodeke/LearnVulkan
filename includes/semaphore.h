#pragma once

#include "base.h"
#include "context.h"

namespace ToyEngine
{
	class Semaphore;
    using SemaphorePtr = std::shared_ptr<Semaphore>;
	class Semaphore
	{
	 public:
		static SemaphorePtr create(const VkDevice& device);

		Semaphore(const VkDevice& device);

		~Semaphore();

		[[nodiscard]] VkSemaphore getSemaphore() const;

	 private:
		VkSemaphore m_semaphore{ VK_NULL_HANDLE };

	};

} // ToyEngine

