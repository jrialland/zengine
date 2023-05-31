#include "Application.hpp"
#include "SkyboxPass.hpp"

class Example1 : public Application
{

	void init() override
	{

		get_rendering_system().add_pass("skybox", std::make_shared<SkyboxPass>());
	}
};

#ifdef _WIN32

#include <Windows.h>

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
		Example1 example1;
		return example1.run(0, nullptr);
	}
	catch (const std::exception &e)
	{
		MessageBoxA(nullptr, e.what(), "Error", MB_OK);
		return 1;
	}
}

#else

int main(int argc, char **argv)
{
	Example1 example1;
	return example1.run(argc, argv);
}

#endif
