#include <iostream>
#include "logger.h"
#include "GLFW/glfw3.h"

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
		std::cerr << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
