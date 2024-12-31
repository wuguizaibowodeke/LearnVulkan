#include "logger.h"
#define  GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "toy2d.h"
#include "context.h"

int main()
{
	Log::Init();
	if (!glfwInit())
	{
		LOG_E("Failed to initialize GLFW");
		return -1;
	}

	//在创建窗口前，必须调用glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API)。
	// GLFW同GLM一样最初是为OpenGL设计的，GLFW_CLIENT_API的默认设置是GLFW_OPENGL_API，
	// 这种情况下，GLFW会在创建窗口时创建OpenGL的上下文，这对于Vulkan而言是多余的，所以向GLFW说明不需要OpenGL的API。
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, /*isResizable*/GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Sandbox", nullptr, nullptr);
	if (!window)
	{
		LOG_E("Failed to create window");
		glfwTerminate();
		return -1;
	}
	ToyEngine::init();

	glfwCreateWindowSurface(ToyEngine::Context::getInstance().vk_instance, window, nullptr, nullptr);

	glfwMakeContextCurrent(window);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	ToyEngine::Quit();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
