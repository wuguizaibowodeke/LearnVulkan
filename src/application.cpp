#include "application.h"
#include "logger.h"
#include "context.h"

namespace ToyEngine
{

	void Application::run()
	{
		Log::Init();
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

	void Application::initWindow()
	{
		m_window = VkWindow::creat(WIDTH, HEIGHT);
	}

	void Application::initVulkan()
	{
		Context::Init(true, m_window->getWindow());
		m_swapChain = SwapChain::create(vkContext.vk_device,
			vkContext.vk_surface, m_window);

		m_renderpass = Renderpass::create(vkContext.vk_device);
		createRenderpass();

		m_swapChain->createFramebuffers(m_renderpass);

		m_pipeline = Pipeline::create(vkContext.vk_device, m_renderpass);
		createPipeline();

		m_commandPool = CommandPool::create(vkContext.vk_device, vkContext.vk_graphicsQueueFamilyIndex.value());

		m_commandBuffers.resize(m_swapChain->getImageCount());
		for (size_t i = 0; i < m_swapChain->getImageCount(); i++)
		{
			m_commandBuffers[i] = CommandBuffer::create(vkContext.vk_device, m_commandPool);

			m_commandBuffers[i]->begin();
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = m_renderpass->getRenderPass();
			renderPassInfo.framebuffer = m_swapChain->getFramebuffers()[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = m_swapChain->getExtent();

			VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			m_commandBuffers[i]->beginRenderPass(renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			m_commandBuffers[i]->bindGraphicPipeline(m_pipeline->getPipeline());
			m_commandBuffers[i]->draw(3);
			m_commandBuffers[i]->endRenderPass();
			m_commandBuffers[i]->end();
		}

		for (int i = 0; i < m_swapChain->getImageCount(); i++)
		{
			auto imageAvailableSemaphore = Semaphore::create(vkContext.vk_device);
			m_imageAvailableSemaphores.push_back(Semaphore::create(vkContext.vk_device));

			auto renderFinishedSemaphore = Semaphore::create(vkContext.vk_device);
			m_renderFinishedSemaphores.push_back(Semaphore::create(vkContext.vk_device));

			auto fence = Fence::create(vkContext.vk_device);
			m_fences.push_back(fence);
		}
	}

	void Application::mainLoop()
	{
		while (!m_window->shouldClose())
		{
			m_window->pollEvents();

			render();
		}

		vkDeviceWaitIdle(vkContext.vk_device);
	}

	void Application::render()
	{
		//等待上一帧的渲染完成
		m_fences[m_currentFrame]->block();

		//获取交换链中的下一帧
		uint32_t imageIndex = 0;
		vkAcquireNextImageKHR(vkContext.vk_device, m_swapChain->getSwapChain(), UINT64_MAX,
			m_imageAvailableSemaphores[m_currentFrame]->getSemaphore(), VK_NULL_HANDLE, &imageIndex);

		//构建提交信息
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		//同步信息，渲染对于显示图像的依赖，显示完毕后，才输出颜色
		VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame]->getSemaphore() };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		//提交的命令缓冲
		auto commandBuffer = m_commandBuffers[imageIndex]->getCommandBuffer();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		//提交的信号量
		VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame]->getSemaphore() };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		//提交信号量
		m_fences[m_currentFrame]->resetFence();

		//提交命令
		if (vkQueueSubmit(vkContext.vk_graphicsQueue, 1, &submitInfo, m_fences[m_currentFrame]->getFence()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit draw command buffer.");
		}

		//提交显示
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_swapChain->getSwapChain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(vkContext.vk_presentQueue, &presentInfo);

		m_currentFrame = (m_currentFrame + 1) % m_swapChain->getImageCount();
	}

	void Application::cleanup()
	{
	    for(auto &fence : m_fences)
		{
			fence.reset();
		}
		for(auto &semaphore : m_renderFinishedSemaphores)
		{
			semaphore.reset();
		}
		for(auto &semaphore : m_imageAvailableSemaphores)
		{
			semaphore.reset();
		}
		for (auto& commandBuffer : m_commandBuffers)
		{
			commandBuffer.reset();
		}
		m_commandPool.reset();
		m_pipeline.reset();
		m_renderpass.reset();
		m_swapChain.reset();
		Context::Quit();
		m_window.reset();
	}

	void Application::createPipeline()
	{
		//设置视口
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)WIDTH;
		viewport.height = (float)HEIGHT;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		m_pipeline->setViewport({ viewport });

		VkRect2D scissors{};//裁剪矩形
		scissors.offset = { 0, 0 };
		scissors.extent = { WIDTH, HEIGHT };
		m_pipeline->setScissors({ scissors });

		std::vector<ShaderPtr> shaderGroup;
		auto vshader = Shader::create(vkContext.vk_device, "../vs.spv", "main", VK_SHADER_STAGE_VERTEX_BIT);
		auto fshader = Shader::create(vkContext.vk_device, "../fs.spv", "main", VK_SHADER_STAGE_FRAGMENT_BIT);
		shaderGroup.push_back(vshader);
		shaderGroup.push_back(fshader);

		m_pipeline->setShaderGroup(shaderGroup);

		//顶点的排布模式
		m_pipeline->m_vertexInputInfo.vertexBindingDescriptionCount = 0;
		m_pipeline->m_vertexInputInfo.pVertexBindingDescriptions = nullptr;
		m_pipeline->m_vertexInputInfo.vertexAttributeDescriptionCount = 0;
		m_pipeline->m_vertexInputInfo.pVertexAttributeDescriptions = nullptr;

		//图元装配
		m_pipeline->m_inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		m_pipeline->m_inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		m_pipeline->m_inputAssembly.primitiveRestartEnable = VK_FALSE;

		//光栅化设置
		m_pipeline->m_rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		m_pipeline->m_rasterizer.polygonMode = VK_POLYGON_MODE_FILL;//其他模式需要启用gpu特性
		m_pipeline->m_rasterizer.lineWidth = 1.0f;//大于1.0f需要启用gpu特性
		m_pipeline->m_rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		m_pipeline->m_rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

		m_pipeline->m_rasterizer.depthBiasEnable = VK_FALSE;
		m_pipeline->m_rasterizer.depthBiasConstantFactor = 0.0f;
		m_pipeline->m_rasterizer.depthBiasClamp = 0.0f;
		m_pipeline->m_rasterizer.depthBiasSlopeFactor = 0.0f;

		//TODO:多重采样
		m_pipeline->m_multisampling.sampleShadingEnable = VK_FALSE;
		m_pipeline->m_multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		m_pipeline->m_multisampling.minSampleShading = 1.0f;
		m_pipeline->m_multisampling.pSampleMask = nullptr;
		m_pipeline->m_multisampling.alphaToCoverageEnable = VK_FALSE;
		m_pipeline->m_multisampling.alphaToOneEnable = VK_FALSE;

		//TODO:深度与模板测试

		//颜色混合
		//这个是颜色混合掩码，得到的混合结果，按照通道与掩码进行AND操作，输出
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		m_pipeline->pushBlendAttachment(colorBlendAttachment);

		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
		//blend有两种计算方式，第一种如上所属，进行alpha为基础的计算，第二种是进行位运算
		//如果开启了logicOp,那么上方设置的alpha为基础的运算失效
		//colorWrite掩码仍然有效，即使开启了logicOp
		//因为我们可能会有多个FrameBuffer输出，所以可能需要多个blendAttachment
		m_pipeline->m_colorBlending.logicOpEnable = VK_FALSE;
		m_pipeline->m_colorBlending.logicOp = VK_LOGIC_OP_COPY;
		m_pipeline->m_colorBlending.blendConstants[0] = 0.0f;
		m_pipeline->m_colorBlending.blendConstants[1] = 0.0f;
		m_pipeline->m_colorBlending.blendConstants[2] = 0.0f;
		m_pipeline->m_colorBlending.blendConstants[3] = 0.0f;

		//uniform的传递
		m_pipeline->m_layout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		m_pipeline->m_layout.setLayoutCount = 0;
		m_pipeline->m_layout.pSetLayouts = nullptr;
		m_pipeline->m_layout.pushConstantRangeCount = 0;
		m_pipeline->m_layout.pPushConstantRanges = nullptr;

		m_pipeline->buildPipeline();
	}

	void Application::createRenderpass()
	{
		//输入画布的描述
		VkAttachmentDescription attachmentDes{};
		attachmentDes.format = m_swapChain->getImageFormat();
		attachmentDes.samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDes.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDes.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDes.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDes.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDes.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDes.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		m_renderpass->addAttachmentDescription(attachmentDes);

		//对于画布的索引设置及格式要求
		VkAttachmentReference attachmentRef{};
		attachmentRef.attachment = 0;
		attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		//创建子流程
		Subpass subpass{};
		subpass.addColorAttachmentReference(attachmentRef);
		subpass.buildSubpassDescription();

		m_renderpass->addSubpass(subpass);

		//子流程之间的依赖关系
		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		m_renderpass->addDependency(dependency);

		m_renderpass->buildRenderpass();
	}

} // ToyEngine