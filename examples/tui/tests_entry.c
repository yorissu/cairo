
#include "cairo.h"

cairo_test_new(demo, test000) {
}

cairo_test_new(demo, test001) {
	cairo_test_skip();
}

cairo_test_new(demo, test002) {
	cairo_assert_true(true);
}

cairo_test_new(demo, test003) {
	cairo_assert_false(false);
}

cairo_test_new(demo, test004) {
	cairo_assert_eq(0, 0);
}

cairo_test_new(demo, test005) {
	cairo_assert_neq(0, 1);
}

cairo_test_new(demo, test006) {
	cairo_assert_gt(1, 0);
}

cairo_test_new(demo, test007) {
	cairo_assert_ge(1, 1);
}

cairo_test_new(demo, test008) {
	cairo_assert_lt(0, 1);
}

cairo_test_new(demo, test009) {
	cairo_assert_le(1, 1);
}

cairo_test_new(demo, test010) {
	cairo_assert_nr(0, 1e-7, 1e-6);
}

cairo_test_new(demo, test011) {
	cairo_assert_nnr(0, 1, 1e-6);
}

cairo_test_new(demo, test012) {
	cairo_assert_nreq(0, 1e-7);
}

cairo_test_new(demo, test013) {
	cairo_assert_nnreq(0, 1);
}

cairo_test_new(demo, test014) {
	cairo_assert_streq("foo", "foo");
}

cairo_test_new(demo, test015) {
	cairo_assert_strneq("foo", "bar");
}

cairo_test_new(demo, test016) {
	cairo_expect_true(true);
	cairo_test_defered {}
}

cairo_test_new(demo, test017) {
	cairo_expect_false(false);
	cairo_test_defered {}
}

cairo_test_new(demo, test018) {
	cairo_expect_eq(0, 0);
	cairo_test_defered {}
}

cairo_test_new(demo, test019) {
	cairo_expect_neq(0, 1);
	cairo_test_defered {}
}

cairo_test_new(demo, test020) {
	cairo_expect_gt(1, 0);
	cairo_test_defered {}
}

cairo_test_new(demo, test021) {
	cairo_expect_ge(1, 1);
	cairo_test_defered {}
}

cairo_test_new(demo, test022) {
	cairo_expect_lt(0, 1);
	cairo_test_defered {}
}

cairo_test_new(demo, test023) {
	cairo_expect_le(1, 1);
	cairo_test_defered {}
}

cairo_test_new(demo, test024) {
	cairo_expect_nr(0, 1e-7, 1e-6);
	cairo_test_defered {}
}

cairo_test_new(demo, test025) {
	cairo_expect_nnr(0, 1, 1e-6);
	cairo_test_defered {}
}

cairo_test_new(demo, test026) {
	cairo_expect_nreq(0, 1e-7);
	cairo_test_defered {}
}

cairo_test_new(demo, test027) {
	cairo_expect_nnreq(0, 1);
	cairo_test_defered {}
}

cairo_test_new(demo, test028) {
	cairo_expect_streq("foo", "foo");
	cairo_test_defered {}
}

cairo_test_new(demo, test029) {
	cairo_expect_strneq("foo", "bar");
	cairo_test_defered {}
}

cairo_test_new(demo, test030) {
	cairo_should_fail(cairo_assert_true(false));
}

cairo_test_new(demo, test031) {
	cairo_should_fail(cairo_expect_true(false));
	cairo_test_defered {}
}

int main(int argc, const char* argv[]) {
	return cairo_tests_run(cairo_args_new(argc, argv));
}
