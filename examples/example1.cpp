#include "Window.hpp"

#include <glog/logging.h>

int main(int argc, char **argv) {
	// Initialize Google's logging library.
	google::InitGoogleLogging(argv[0]);

	Window window;
	return window.render_loop(argc, argv);
}
