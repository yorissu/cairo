
#include "cairo.h"

int main(int argc, const char* argv[]) {
	cairo_args_s args = cairo_args_new(argc, argv);
	args.exclude = "lexer.lone_minus_is_operator";
	return cairo_tests_run(args);
}
