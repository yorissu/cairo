
#include "cairo.h"

cairo_test_new(demo1, test996) {
	cairo_should_fail(cairo_expect_true(false));
	cairo_test_defered
}

cairo_test_new(demo1, test997) {
	cairo_should_fail(cairo_expect_true(true));
	cairo_test_defered
}

cairo_test_new(demo1, test998) {
	cairo_should_fail(cairo_assert_true(false));
}

cairo_test_new(demo1, test999) {
	cairo_should_fail(cairo_assert_true(true));
}

cairo_test_new(demo1, test1000) {
	cairo_assert_true(true);
}

int main(int argc, const char* argv[]) {
	return cairo_tests_run(cairo_args_new(argc, argv));
}
