#include "pipeline.h"
#include "logger.h"

namespace ToyEngine
{
	PipelinePtr Pipeline::create(VkDevice const& device, const RenderpassPtr& renderpass)
	{
		return std::make_shared<Pipeline>(device, renderpass);
	}

	Pipeline::Pipeline(VkDevice const& device, const RenderpassPtr& renderpass)
	{
		m_renderpass = renderpass;

		m_vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		m_inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		m_viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		m_rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		m_multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		m_colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		m_depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		m_layout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	}

	Pipeline::~Pipeline()
	{
		m_renderpass.reset();

		if(m_pipelineLayout != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(vkContext.vk_device, m_pipelineLayout, nullptr);
		}

		if(m_pipeline != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(vkContext.vk_device, m_pipeline, nullptr);
		}
	}

	void Pipeline::setShaderGroup(const std::vector<ShaderPtr>& shaders)
	{
		m_shaders = shaders;
	}

	void Pipeline::buildPipeline()
	{
		//设置shader
		std::vector<VkPipelineShaderStageCreateInfo> shaderCreateInfos;
		for (auto& shader : m_shaders)
		{
			VkPipelineShaderStageCreateInfo shaderCreateInfo{};
			shaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderCreateInfo.stage = shader->getStage();
			shaderCreateInfo.module = shader->getShaderModule();
			shaderCreateInfo.pName = shader->getEntryPoint().c_str();
			shaderCreateInfos.push_back(shaderCreateInfo);
		}

		//设置视口与剪裁
		m_viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		m_viewportState.viewportCount = static_cast<uint32_t>(m_viewports.size());
		m_viewportState.pViewports = m_viewports.data();
		m_viewportState.scissorCount = static_cast<uint32_t>(m_scissors.size());
		m_viewportState.pScissors = m_scissors.data();

		//设置颜色混合
		m_colorBlending.attachmentCount = static_cast<uint32_t>(m_colorBlendAttachment.size());
		m_colorBlending.pAttachments = m_colorBlendAttachment.data();

		//layout生成
		if(m_pipelineLayout != VK_NULL_HANDLE)
		{
			vkDestroyPipelineLayout(vkContext.vk_device, m_pipelineLayout, nullptr);
		}

		if(vkCreatePipelineLayout(vkContext.vk_device, &m_layout, nullptr, &m_pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create pipeline layout.");
		}

		VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderCreateInfos.size());
		pipelineCreateInfo.pStages = shaderCreateInfos.data();
		pipelineCreateInfo.pVertexInputState = &m_vertexInputInfo;
		pipelineCreateInfo.pInputAssemblyState = &m_inputAssembly;
		pipelineCreateInfo.pViewportState = &m_viewportState;
		pipelineCreateInfo.pRasterizationState = &m_rasterizer;
		pipelineCreateInfo.pMultisampleState = &m_multisampling;
		pipelineCreateInfo.pColorBlendState = &m_colorBlending;
		pipelineCreateInfo.pDepthStencilState = nullptr;//Todo:add depth stencil
		pipelineCreateInfo.layout = m_pipelineLayout;
		pipelineCreateInfo.renderPass = m_renderpass->getRenderPass();
		pipelineCreateInfo.subpass = 0;
		//以存在的pipeline为基础进行创建，会更快，但是需要指定flags为VK_PIPELINE_CREATE_DERIVATIVE_BIT
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCreateInfo.basePipelineIndex = -1;

		if(m_pipeline != VK_NULL_HANDLE)
		{
			vkDestroyPipeline(vkContext.vk_device, m_pipeline, nullptr);
		}
		if(vkCreateGraphicsPipelines(vkContext.vk_device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_pipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create graphics pipeline.");
		}
		else
		{
			LOG_I("Pipeline created successfully.");
		}
	}

	void Pipeline::setViewport(const std::vector<VkViewport>& viewports)
	{
		m_viewports = viewports;
	}

	void Pipeline::setScissors(const std::vector<VkRect2D>& scissors)
	{
		m_scissors = scissors;
	}

	void Pipeline::pushBlendAttachment(const VkPipelineColorBlendAttachmentState& attachment)
	{
		m_colorBlendAttachment.push_back(attachment);
	}
} // ToyEngine