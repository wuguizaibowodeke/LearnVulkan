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

		m_pipeline = Pipeline::create(vkContext.vk_device);
		createPipeline();
	}

	void Application::mainLoop()
	{
		while (!m_window->shouldClose())
		{
			m_window->pollEvents();
		}
	}

	void Application::cleanup()
	{
		m_pipeline.reset();
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

		m_pipeline->build();
	}
} // ToyEngine