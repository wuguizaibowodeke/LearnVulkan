#pragma once

#include "base.h"
#include "shader.h"

namespace ToyEngine
{
	class Pipeline;
	using PipelinePtr = std::shared_ptr<Pipeline>;
	class Pipeline
	{
	 public:
		static PipelinePtr create(const VkDevice& device);

		Pipeline(const VkDevice& device);

		~Pipeline();

		void setShaderGroup(const std::vector<ShaderPtr>& shaders);

		void build();

		void setViewport(const std::vector<VkViewport>& viewports);

		void setScissors(const std::vector<VkRect2D>& scissors);

		void pushBlendAttachment(const VkPipelineColorBlendAttachmentState& attachment);

	 public:
		VkPipelineVertexInputStateCreateInfo m_vertexInputInfo{};
		VkPipelineInputAssemblyStateCreateInfo m_inputAssembly{};
		VkPipelineViewportStateCreateInfo m_viewportState{};
		VkPipelineRasterizationStateCreateInfo m_rasterizer{};
		VkPipelineMultisampleStateCreateInfo m_multisampling{};
		std::vector<VkPipelineColorBlendAttachmentState> m_colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo m_colorBlending{};
		VkPipelineDepthStencilStateCreateInfo m_depthStencil{};
		VkPipelineLayoutCreateInfo m_layout{};

		//Todo::renderpass

	 private:
		VkPipeline m_pipeline{ VK_NULL_HANDLE };

		VkPipelineLayout m_pipelineLayout{ VK_NULL_HANDLE };
		std::vector<ShaderPtr> m_shaders;

		std::vector<VkViewport> m_viewports;
		std::vector<VkRect2D> m_scissors;
	};

} // ToyEngine


