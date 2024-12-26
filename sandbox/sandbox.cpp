#include "logger.h"
#define  GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "toy2d.h"

int main()
{
	Log::Init();
	if (!glfwInit())
	{
		LOG_E("Failed to initialize GLFW");
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(800, 600, "Sandbox", nullptr, nullptr);
	if (!window)
	{
		LOG_E("Failed to create window");
		glfwTerminate();
		return -1;
	}
	ToyEngine::init();

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
