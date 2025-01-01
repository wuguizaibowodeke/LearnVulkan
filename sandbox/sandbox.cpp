#include "application.h"
#include "toy2d.h"
#include "context.h"

int main()
{
	ToyEngine::Application app{};

	try
	{
		app.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return 0;

}
