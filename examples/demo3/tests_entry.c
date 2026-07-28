
#include "cairo.h"

cairo_test_new(demo2, test01) {
	cairo_assert_eq(0, 0);
}

cairo_test_new(demo2, test02) {
	cairo_assert_eq(1, 1);
}

cairo_test_new(demo2, test03) {
	cairo_assert_eq(1, 2);
}

cairo_test_new(demo2, test04) {
	cairo_assert_eq((
		5 + 9 * 7
	), (
		60 + 8
		+ 1
	));
}

cairo_test_new(demo2, test05) {
	cairo_test_skip();
}

int main(const int argc, const char* argv[]) {
	return cairo_tests_run(cairo_args_new(argc, argv));
}
