
#include "cairo.h"

cairo_test_new(demo, test996) {
	cairo_should_fail(cairo_expect_true(false));
	cairo_test_defered
}

cairo_test_new(demo, test997) {
	cairo_should_fail(cairo_expect_true(true));
	cairo_test_defered
}

cairo_test_new(demo, test998) {
	cairo_should_fail(cairo_assert_true(false));
}

cairo_test_new(demo, test999) {
	cairo_should_fail(cairo_assert_true(true));
}

int main(int argc, const char* argv[]) {
	return cairo_tests_run(cairo_args_new(argc, argv));
}
