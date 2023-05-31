#include "Logging.hpp"
#include "Application.hpp"
#include "SkyboxPass.hpp"

class Example1 : public Application
{

	void init() override
	{

		get_rendering_system().add_pass("skybox", std::make_shared<SkyboxPass>());
	}
};

int main(int argc, char **argv)
{
	try {
		Example1 example1;
		return example1.run(argc, argv);
	} catch(std::exception &e) {
		LOG(ERROR) << e.what();
		return 1;
	}
}