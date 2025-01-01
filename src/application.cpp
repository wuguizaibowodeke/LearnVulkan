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
		m_swapChain.reset();
		Context::Quit();
		m_window.reset();
	}
} // ToyEngine