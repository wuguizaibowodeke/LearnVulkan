#include "vulkan/vulkan.hpp"
#include <memory>

namespace toy2d
{

	class Context final // final means that this class cannot be inherited from
	{
	 public:
		~Context();
		Context(const Context&) = delete; // delete means that this function cannot be called
		Context& operator=(const Context&) = delete;

		static void Init();
		static void Quit();
		static Context& getInstance();

	 public:
		vk::Instance instance;

	 private:
		Context();

	 private:
		static std::unique_ptr<Context> m_instance;
	};

} // toy2d


