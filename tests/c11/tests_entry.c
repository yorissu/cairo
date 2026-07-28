
#include "cairo.h"

int main(const int argc, const char* argv[]) {
	return cairo_tests_run(cairo_args_new(argc, argv));
}
