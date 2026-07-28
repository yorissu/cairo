
#include "calc.h"

#include "cairo.h"

cairo_test_new(calc, single_value) {
	int out = 0;
	cairo_assert_eq(calc_eval("42", &out), calc_status_ok);
	cairo_assert_eq(out, 42);
}

cairo_test_new(calc, simple_add) {
	int out = 0;
	cairo_assert_eq(calc_eval("3 4 +", &out), calc_status_ok);
	cairo_assert_eq(out, 7);
}

cairo_test_new(calc, all_operators) {
	int out = 0;
	cairo_assert_eq(calc_eval("10 3 -", &out), calc_status_ok);
	cairo_assert_eq(out, 7);
	cairo_assert_eq(calc_eval("6 7 *", &out), calc_status_ok);
	cairo_assert_eq(out, 42);
	cairo_assert_eq(calc_eval("20 4 /", &out), calc_status_ok);
	cairo_assert_eq(out, 5);
}

cairo_test_new(calc, nested_expression) {
	int out = 0;
	cairo_assert_eq(calc_eval("5 1 2 + 4 * +", &out), calc_status_ok);
	cairo_assert_eq(out, 17);
}

cairo_test_new(calc, negative_operands) {
	int out = 0;
	cairo_assert_eq(calc_eval("-3 -4 *", &out), calc_status_ok);
	cairo_assert_eq(out, 12);
}

cairo_test_new(calc, bad_token_reported) {
	int out = 0;
	cairo_assert_eq(calc_eval("3 x +", &out), calc_status_err_bad_token);
}

cairo_test_new(calc, underflow_reported) {
	int out = 0;
	cairo_assert_eq(calc_eval("3 +", &out), calc_status_err_underflow);
}

cairo_test_new(calc, div_zero_reported) {
	int out = 0;
	cairo_assert_eq(calc_eval("5 0 /", &out), calc_status_err_div_zero);
}

cairo_test_new(calc, leftover_reported) {
	int out = 0;
	cairo_assert_eq(calc_eval("1 2 3 +", &out), calc_status_err_leftover);
}

cairo_test_new(calc, empty_is_leftover) {
	int out = 0;
	cairo_assert_eq(calc_eval("", &out), calc_status_err_leftover);
}
