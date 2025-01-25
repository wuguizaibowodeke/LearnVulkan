#include "vkWindow.h"

namespace ToyEngine
{
	VkWindow::VkWindow(unsigned int width, unsigned int height)
	{
		m_width = width;
		m_height = height;
		glfwInit();

		//设置环境，关闭openGL API并且禁止窗口调整大小
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_window = glfwCreateWindow(m_width, m_height, "Vulkan Window", nullptr, nullptr);
		if (m_window == nullptr)
		{
			throw std::runtime_error("Failed to create GLFW window.");
		}
	}

	VkWindow::~VkWindow()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	bool VkWindow::shouldClose() const
	{
		return glfwWindowShouldClose(m_window);
	}

	void VkWindow::pollEvents() const
	{
		glfwPollEvents();
	}

	GLFWwindow* VkWindow::getWindow() const
	{
		return m_window;
	}

	WindowPtr VkWindow::creat(unsigned int width, unsigned int height)
	{
		return std::make_shared<VkWindow>(width, height);
	}

} // ToyEngine