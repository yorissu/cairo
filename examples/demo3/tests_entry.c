
#define cairo_supress_sign_compare_warnings
#define cairo_disable_verbose_output
#include "cairo.h"



cairo_test_new(demo2, test01) {
	cairo_assert_eq(0, 0);
}

cairo_test_new(demo2, test02) {
	for (int _ = 0; _ < 10000000; ++_) {
		cairo_assert_eq(1, 1);
	}
	cairo_assert_eq(1, 1);
}

// cairo_test_new(demo2, crash) { int* p = NULL; *p = 1; }

cairo_test_new(demo2, test03) {
	cairo_assert_eq(1, 2);

	typedef enum {
		enum_val_a = 0,
		enum_val_b = 1,
	} enum_val_e;
	enum_val_e val = enum_val_a;
	cairo_assert_eq(val, enum_val_b);
	(void)val;
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

cairo_test_new(demo2, test11) {
	cairo_test_skip();
}

cairo_test_new(demo2, test12) {
	cairo_test_skip();
}



cairo_test_new(demo3, test04) {
	cairo_assert_eq(1, 1);
}

cairo_test_new(demo3, test05) {
	cairo_test_skip();
}

cairo_test_new(demo3, test11) {
	cairo_test_skip();
}

cairo_test_new(demo3, test12) {
	cairo_test_skip();
}

cairo_test_new(demo3, test13) {
	cairo_assert_eq(1, 1);
}



cairo_test_new(demo4, test04) {
	cairo_assert_eq(1, 1);
}

cairo_test_new(demo4, test05) {
	cairo_test_skip();
}

cairo_test_new(demo4, test11) {
	cairo_test_skip();
}

cairo_test_new(demo4, test12) {
	cairo_test_skip();
}

cairo_test_new(demo4, test13) {
	cairo_assert_eq(1, 1);
}



cairo_test_new(strctl, equal_literals) {
	cairo_assert_eq("abc", "abc");
}

cairo_test_new(strctl, equal_buffers) {
	char a[] = "hello";
	char b[] = "hello";
	cairo_assert_neq(a, b);
	cairo_assert_streq(a, b);
}

cairo_test_new(strctl, aliased_buffer) {
	char a[] = "world";
	const char* p = a;
	cairo_assert_eq(a, p);
}

cairo_test_new(strctl, different_contents) {
	char a[] = "foo";
	char b[] = "bar";
	cairo_assert_neq(a, b);
	cairo_assert_strneq(a, b);
}

cairo_test_new(strctl, nulls) {
	cairo_assert_streq(NULL, NULL);
	// cairo_assert_streq(NULL, "x");
	// cairo_assert_strneq(NULL, NULL);
	cairo_assert_strneq(NULL, "x");
}



#include <stdlib.h>

cairo_test_new(custom, defer1) {
	void* p = malloc(100);
	// cairo_assert_streq(NULL, "x");
	// cairo_assert_strneq(NULL, NULL);
	cairo_expect_true(false);

	cairo_test_defered {
		free(p);
	}
}

cairo_test_new(custom, defer2) {
	void* p = malloc(200);
	// cairo_assert_streq(NULL, "x");
	// cairo_assert_strneq(NULL, NULL);
	cairo_expect_true(false);

	cairo_test_defered {
		free(p);
	}
}

cairo_test_new(custom, defer3) {
	cairo_expect_true(false);
	cairo_test_defered
}



int main(const int argc, const char* argv[]) {
	(void)argc;
	(void)argv;
	// return cairo_tests_run_default();
	return cairo_tests_run(cairo_args_new(argc, argv));
}
