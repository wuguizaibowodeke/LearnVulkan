#pragma once

#include "base.h"
#include "context.h"

namespace ToyEngine
{
	class Fence;
	using FencePtr = std::shared_ptr<Fence>;
	class Fence
	{
	 public:
		static FencePtr create(const VkDevice& device, bool signaled = true);

		Fence(const VkDevice& device, bool signaled = true);

		~Fence();

		[[nodiscard]] VkFence getFence() const;

		void resetFence();

		void block(uint64_t timeout = UINT64_MAX);

	 private:
		VkFence m_fence{ VK_NULL_HANDLE };
	};
}

