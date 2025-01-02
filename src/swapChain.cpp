#include "swapChain.h"
#include "logger.h"

namespace ToyEngine
{

	ToyEngine::SwapChain::SwapChain(const VkDevice& device,
		const VkSurfaceKHR& surface,
		const WindowPtr& window)
	{
		m_window = window;
		//获取交换链支持信息
		auto swapChainSupportInfo = querySwapChainSupport();
		//选择交换链表面格式
		auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupportInfo.m_Formats);
		//选择交换链呈现模式
		auto presentMode = chooseSwapPresentMode(swapChainSupportInfo.m_PresentModes);
		//选择交换链分辨率
		auto extent = chooseSwapExtent(swapChainSupportInfo.m_Capabilities);
		//交换链图像缓冲数量
		m_imageCount = swapChainSupportInfo.m_Capabilities.minImageCount + 1;

		if (swapChainSupportInfo.m_Capabilities.maxImageCount > 0 &&
			m_imageCount > swapChainSupportInfo.m_Capabilities.maxImageCount)
		{
			m_imageCount = swapChainSupportInfo.m_Capabilities.maxImageCount;
		}

		//此处初始化必须置空，确保参数出现随机值
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = m_imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		//图像包含的层次，VR中为2
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		//由于交换链的图像，会被用渲染或者显示，而渲染显示分别使用不同的队列，所以会出现两个队列使用同一个交换链的情况
		//这种情况下，需要设置交换链的图像被两个队列使用
		if (vkContext.vk_graphicsQueueFamilyIndex.value() != vkContext.vk_presentQueueFamilyIndex.value())
		{
			uint32_t queueFamilyIndices[] =
				{ vkContext.vk_graphicsQueueFamilyIndex.value(), vkContext.vk_presentQueueFamilyIndex.value() };
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;//被某一个队列族独占，性能会更好
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		//交换链的图像的初始变化，比如是否需要反转
		createInfo.preTransform = swapChainSupportInfo.m_Capabilities.currentTransform;
		//透明度，一般不透明，不与原来窗体当中的内容混合
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		//呈现模式
		createInfo.presentMode = presentMode;
		//是否裁剪，一般不裁剪。当前窗体被挡住的部分不进行绘制，但是会影响回读
		createInfo.clipped = VK_TRUE;

		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create swap chain.");
		}
		else
		{
			LOG_I("Swap chain created successfully.");
		}

		m_imageFormat = surfaceFormat.format;
		m_extent = extent;

		//系统可能船舰更多的图像，当前的imageCount是最小值
		vkGetSwapchainImagesKHR(device, m_swapChain, &m_imageCount, nullptr);
		m_images.resize(m_imageCount);
		vkGetSwapchainImagesKHR(device, m_swapChain, &m_imageCount, m_images.data());

		//创建imageView
		m_imageViews.resize(m_imageCount);
		for (size_t i = 0; i < m_imageCount; i++)
		{
			m_imageViews[i] = createImageView(m_images[i], m_imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		}
	}

	ToyEngine::SwapChain::~SwapChain()
	{
		for (auto imageView : m_imageViews)
		{
			vkDestroyImageView(vkContext.vk_device, imageView, nullptr);
		}

		for(auto framebuffer : m_framebuffers)
		{
			vkDestroyFramebuffer(vkContext.vk_device, framebuffer, nullptr);
		}

		if (m_swapChain != VK_NULL_HANDLE)
		{
			vkDestroySwapchainKHR(vkContext.vk_device, m_swapChain, nullptr);
		}

		m_window.reset();
	}

	SwapChainPtr ToyEngine::SwapChain::create(const VkDevice& device,
		const VkSurfaceKHR& surface,
		const WindowPtr& window)
	{
		return std::make_shared<SwapChain>(device, surface, window);
	}

	SwapChainSupportInfo SwapChain::querySwapChainSupport()
	{
		SwapChainSupportInfo info;
		//获取基础特性
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Context::getInstance().vk_physicalDevice,
			Context::getInstance().vk_surface,
			&info.m_Capabilities);

		//获取表面支持格式
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(Context::getInstance().vk_physicalDevice,
			Context::getInstance().vk_surface,
			&formatCount,
			nullptr);

		if (formatCount != 0)
		{
			info.m_Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(Context::getInstance().vk_physicalDevice,
				Context::getInstance().vk_surface,
				&formatCount,
				info.m_Formats.data());
		}

		//获取表面支持呈现模式
		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(Context::getInstance().vk_physicalDevice,
			Context::getInstance().vk_surface,
			&presentModeCount,
			nullptr);

		if (presentModeCount != 0)
		{
			info.m_PresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(Context::getInstance().vk_physicalDevice,
				Context::getInstance().vk_surface,
				&presentModeCount,
				info.m_PresentModes.data());
		}

		return info;
	}

	VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		//如果只返回了一个未定义的格式，那么就没有首选格式，我们可以自由选择任何格式
		if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			return { VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}

		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		//在设备上，只有FIFO模式是强制支持的
		//如果在移动设备上，为了节省电源，优先选择FIFO
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return bestMode;
	}

	VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		//如果出现以下情况，说明系统不允许我们自己选择交换链的分辨率
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}

		int width = 0, height = 0;
		glfwGetFramebufferSize(m_window->getWindow(), &width, &height);

		VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		//规定在max与min之间
		actualExtent.width =
			std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height =
			std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}

	VkImageView SwapChain::createImageView(VkImage image,
		VkFormat format,
		VkImageAspectFlagBits aspectFlags,
		uint32_t mipLevels)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;
		createInfo.subresourceRange.aspectMask = aspectFlags;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = mipLevels;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkImageView imageView{ VK_NULL_HANDLE };
		if (vkCreateImageView(Context::getInstance().vk_device, &createInfo, nullptr, &imageView) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image views.");
		}

		return imageView;
	}

	void SwapChain::createFramebuffers(const RenderpassPtr& renderPass)
	{
		m_framebuffers.resize(m_imageCount);
		for (size_t i = 0; i < m_imageCount; i++)
		{
			//framebuffer 保存了一帧的数据，n个colorAttachment，1个depthAttachment/stencilAttachment
			VkImageView attachments[] = { m_imageViews[i] };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass->getRenderPass();
			framebufferInfo.attachmentCount = static_cast<uint32_t>(std::size(attachments));
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = m_extent.width;
			framebufferInfo.height = m_extent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(vkContext.vk_device, &framebufferInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create framebuffer.");
			}
		}
	}
} // ToyEngine