#pragma once

#include "base.h"
#include "vkWindow.h"
#include "swapChain.h"
#include "shader.h"
#include "pipeline.h"
#include "renderpass.h"
#include "commandpool.h"
#include "commandBuffer.h"

namespace ToyEngine
{
	const int WIDTH = 800;
	const int HEIGHT = 600;

	class Application
	{
	 public:
		Application() = default;

		~Application() = default;

		void run();

	 private:
		void initWindow();

		void initVulkan();

		void mainLoop();

		void cleanup();

		void createPipeline();

		void createRenderpass();

	 private:
		WindowPtr m_window{ nullptr };
		SwapChainPtr m_swapChain{ nullptr };
		PipelinePtr m_pipeline{ nullptr };
		RenderpassPtr m_renderpass{ nullptr };
		CommandPoolPtr m_commandPool{ nullptr };
		std::vector<CommandBufferPtr> m_commandBuffers{};
	};

} // ToyEngine

