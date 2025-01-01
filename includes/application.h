#pragma once

#include "base.h"
#include "vkWindow.h"
#include "swapChain.h"
#include "shader.h"
#include "pipeline.h"

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

	 private:
		WindowPtr m_window;
		SwapChainPtr m_swapChain;
		PipelinePtr m_pipeline;
	};

} // ToyEngine

