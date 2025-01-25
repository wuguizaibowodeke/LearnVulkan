#pragma once

#include "base.h"

namespace ToyEngine
{
	class VkWindow;
	using WindowPtr = std::shared_ptr<VkWindow>;
	class VkWindow
	{
	 public:
		VkWindow(unsigned int width, unsigned int height);

		~VkWindow();

		[[nodiscard]] bool shouldClose() const;

		void pollEvents() const;

		[[nodiscard]] GLFWwindow* getWindow() const;

		static WindowPtr creat(unsigned int width, unsigned int height);

	 private:
		GLFWwindow* m_window{ nullptr };

		unsigned int m_width{ 0 };
		unsigned int m_height{ 0 };
	};

} // ToyEngine

