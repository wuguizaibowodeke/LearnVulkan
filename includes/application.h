#pragma once

#include "base.h"
#include "vkWindow.h"
#include "swapChain.h"

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

	 private:
		WindowPtr m_window;
		SwapChainPtr m_swapChain;
	};

} // ToyEngine

