#pragma once
#include "base.h"
#include "context.h"
#include "vkWindow.h"
#include "renderpass.h"

namespace ToyEngine
{
	struct SwapChainSupportInfo
	{
		VkSurfaceCapabilitiesKHR m_Capabilities;
		std::vector<VkSurfaceFormatKHR> m_Formats;
		std::vector<VkPresentModeKHR> m_PresentModes;
	};

	class SwapChain;
	using SwapChainPtr = std::shared_ptr<SwapChain>;
	class SwapChain
	{
	 public:
		SwapChain(const VkDevice& device, const VkSurfaceKHR& surface, const WindowPtr& window);

		~SwapChain();

		static SwapChainPtr create(const VkDevice& device, const VkSurfaceKHR& surface, const WindowPtr& window);

		SwapChainSupportInfo querySwapChainSupport();

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		[[nodiscard]] VkSwapchainKHR getSwapChain() const { return m_swapChain; }

		[[nodiscard]] VkFormat getImageFormat() const { return m_imageFormat; }

		[[nodiscard]] VkExtent2D getExtent() const { return m_extent; }

		[[nodiscard]] std::vector<VkImageView> getImageViews() const { return m_imageViews; }

		[[nodiscard]] std::vector<VkFramebuffer> getFramebuffers() const { return m_framebuffers; }

		[[nodiscard]] uint32_t getImageCount() const { return m_imageCount; }

		void createFramebuffers(const RenderpassPtr& renderPass);

	 private:
		VkImageView createImageView(VkImage image,
			VkFormat format,
			VkImageAspectFlagBits aspectFlags,
			uint32_t mipLevels = 1);

	 private:
		VkSwapchainKHR m_swapChain{ VK_NULL_HANDLE };
		WindowPtr m_window{ nullptr };

		VkFormat m_imageFormat;
		VkExtent2D m_extent;

		uint32_t m_imageCount{ 0 };
		//vkimage由swapChain创建，销毁也要由swapChain销毁
		std::vector<VkImage> m_images;
		//对图像的管理
		std::vector<VkImageView> m_imageViews;

		std::vector<VkFramebuffer> m_framebuffers;
	};

} // ToyEngine


