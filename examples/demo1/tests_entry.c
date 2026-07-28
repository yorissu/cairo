
#include "cairo.h"

cairo_test_new(demo1, test00) {
	cairo_test_skip();
}

cairo_test_new(demo1, test01) {
	cairo_assert_true(1);
}

cairo_test_new(demo1, test02) {
	cairo_assert_true(true);
}

cairo_test_new(demo1, test03) {
	cairo_assert_false(0);
}

cairo_test_new(demo1, test04) {
	cairo_assert_false(false);
}

cairo_test_new(demo1, test05) {
	cairo_assert_eq(1, 1);
}

cairo_test_new(demo1, test06) {
	cairo_assert_neq(0, 1);
}

cairo_test_new(demo1, test07) {
	cairo_assert_gt(1, 0);
}

cairo_test_new(demo1, test08) {
	cairo_assert_ge(1, 0);
	cairo_assert_ge(1, 1);
}

cairo_test_new(demo1, test09) {
	cairo_assert_lt(0, 1);
}

cairo_test_new(demo1, test10) {
	cairo_assert_le(0, 1);
	cairo_assert_le(1, 1);
}

cairo_test_new(demo1, test11) {
	cairo_assert_nr(0, 1e-7, 1e-6);
}

cairo_test_new(demo1, test12) {
	cairo_assert_nreq(0, 1e-7);
}

cairo_test_new(demo1, test13) {
	cairo_assert_true(false);
}

int main(void) {
	#define _set_args(...)                                                     \
		const char* argv[] = { __VA_ARGS__ };                                  \
		const int argc = sizeof(argv) / sizeof(*argv)

	{
		(void)printf("# ------------------------------\n");
		(void)printf("# default\n");
		_set_args("demo1");
		const int ecode = cairo_tests_run(cairo_args_new(argc, argv));
		(void)printf("ecode: %d\n", ecode);
	}

	{
		(void)printf("# ------------------------------\n");
		(void)printf("# pattern\n");
		_set_args("demo1");
		cairo_args_s args = cairo_args_new(argc, argv);
		args.pattern = "*test1*";
		const int ecode = cairo_tests_run(args);
		(void)printf("ecode: %d\n", ecode);
	}

	{
		(void)printf("# ------------------------------\n");
		(void)printf("# exclude\n");
		_set_args("demo1");
		cairo_args_s args = cairo_args_new(argc, argv);
		args.exclude = "*test1*";
		const int ecode = cairo_tests_run(args);
		(void)printf("ecode: %d\n", ecode);
	}

	{
		(void)printf("# ------------------------------\n");
		(void)printf("# pattern+exclude\n");
		_set_args("demo1");
		cairo_args_s args = cairo_args_new(argc, argv);
		args.pattern = "*test1*";
		args.exclude = "*test11";
		const int ecode = cairo_tests_run(args);
		(void)printf("ecode: %d\n", ecode);
	}

	{
		(void)printf("# ------------------------------\n");
		(void)printf("# pattern+repeat\n");
		_set_args("demo1");
		cairo_args_s args = cairo_args_new(argc, argv);
		args.pattern = "*test0*";
		args.repeat = 3;
		const int ecode = cairo_tests_run(args);
		(void)printf("ecode: %d\n", ecode);
	}

	{
		(void)printf("# ------------------------------\n");
		(void)printf("# ecode\n");
		_set_args("demo1");
		cairo_args_s args = cairo_args_new(argc, argv);
		args.ecode = 69;
		const int ecode = cairo_tests_run(args);
		(void)printf("ecode: %d\n", ecode);
	}

	{
		(void)printf("# ------------------------------\n");
		(void)printf("# list\n");
		_set_args("demo1");
		cairo_args_s args = cairo_args_new(argc, argv);
		args.list = true;
		const int ecode = cairo_tests_run(args);
		(void)printf("ecode: %d\n", ecode);
	}

	(void)printf("# ------------------------------\n");

	return 0;
}
