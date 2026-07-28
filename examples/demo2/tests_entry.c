
#include "cairo.h"

cairo_test_new(demo2, check_cairo_format_time_microseconds) {
	cairo_assert_eq(strcmp(_cairo_format_time(500e-6), "500.000us"), 0);
}

cairo_test_new(demo2, check_cairo_format_time_milliseconds) {
	cairo_assert_eq(strcmp(_cairo_format_time(1.5e-3), "1.500ms"), 0);
}

cairo_test_new(demo2, check_cairo_format_time_seconds) {
	cairo_assert_eq(strcmp(_cairo_format_time(2.0), "2.000s"), 0);
}

cairo_test_new(demo2, check_cairo_compare_orders_by_suite) {
	_cairo_test_s a = { .suite = "a", .name = "x", };
	_cairo_test_s b = { .suite = "b", .name = "x", };
	_cairo_test_s* pa = &a;
	_cairo_test_s* pb = &b;
	cairo_assert_lt(_cairo_test_compare(&pa, &pb), 0);
}

cairo_test_new(demo2, check_cairo_compare_orders_by_name) {
	_cairo_test_s a = { .suite = "x", .name = "a", };
	_cairo_test_s b = { .suite = "x", .name = "b", };
	_cairo_test_s* pa = &a;
	_cairo_test_s* pb = &b;
	cairo_assert_lt(_cairo_test_compare(&pa, &pb), 0);
}

cairo_test_new(demo2, check_cairo_compare_null_sinks_to_end) {
	_cairo_test_s a = { .suite = "s", .name = "x", };
	_cairo_test_s* pa = &a;
	_cairo_test_s* pn = NULL;
	cairo_assert_lt(_cairo_test_compare(&pa, &pn), 0);
	cairo_assert_gt(_cairo_test_compare(&pn, &pa), 0);
	cairo_assert_eq(_cairo_test_compare(&pn, &pn), 0);
}

cairo_test_new(demo2, check_empty_test) {
}

cairo_test_new(demo2, check_cairo_test_skip) {
	cairo_test_skip();
}

cairo_test_new(demo2, check_cairo_record_value_null_string) {
	char buffer[65];
	const char* const value = NULL;
	_cairo_format_type(buffer, value);
	cairo_assert_eq(strcmp(buffer, "null"), 0);
}

cairo_test_new(demo2, check_cairo_record_value_null_pointer) {
	char buffer[65];
	_cairo_format_type(buffer, (void*)NULL);
	cairo_assert_eq(strcmp(buffer, "null"), 0);
}

cairo_test_new(demo2, check_cairo_record_value_string) {
	char buffer[65];
	const char* const value = "hello";
	_cairo_format_type(buffer, value);
	cairo_assert_eq(strcmp(buffer, "\"hello\""), 0);
}

cairo_test_new(demo2, check_cairo_record_value_string_array) {
	char buffer[65];
	char value[] = "world";
	_cairo_format_type(buffer, value);
	cairo_assert_eq(strcmp(buffer, "\"world\""), 0);
}

cairo_test_new(demo2, check_cairo_record_value_string_literal) {
	char buffer[65];
	_cairo_format_type(buffer, "literal");
	cairo_assert_eq(strcmp(buffer, "\"literal\""), 0);
}

cairo_test_new(demo2, check_cairo_record_value_empty_string) {
	char buffer[65];
	const char* const value = "";
	_cairo_format_type(buffer, value);
	cairo_assert_eq(strcmp(buffer, "\"\""), 0);
}

cairo_test_new(demo2, check_cairo_record_value_bool_true) {
	char buffer[65];
	const bool value = true;
	_cairo_format_type(buffer, value);
	cairo_assert_eq(strcmp(buffer, "true"), 0);
}

cairo_test_new(demo2, check_cairo_record_value_bool_false) {
	char buffer[65];
	const bool value = false;
	_cairo_format_type(buffer, value);
	cairo_assert_eq(strcmp(buffer, "false"), 0);
}

cairo_test_new(demo2, check_cairo_record_value_characters) {
	char buffer[65];
	_cairo_format_type(buffer, (char)'x');
	cairo_assert_eq(strcmp(buffer, "x"), 0);
	_cairo_format_type(buffer, (signed char)-3);
	cairo_assert_eq(strcmp(buffer, "-3"), 0);
	_cairo_format_type(buffer, (unsigned char)200);
	cairo_assert_eq(strcmp(buffer, "200"), 0);
}

cairo_test_new(demo2, check_cairo_record_value_signed_integers) {
	char buffer[65];
	_cairo_format_type(buffer, (signed short)-7);
	cairo_assert_eq(strcmp(buffer, "-7"), 0);
	_cairo_format_type(buffer, -42);
	cairo_assert_eq(strcmp(buffer, "-42"), 0);
	_cairo_format_type(buffer, -42L);
	cairo_assert_eq(strcmp(buffer, "-42"), 0);
	_cairo_format_type(buffer, -42LL);
	cairo_assert_eq(strcmp(buffer, "-42"), 0);
}

cairo_test_new(demo2, check_cairo_record_value_unsigned_integers) {
	char buffer[65];
	_cairo_format_type(buffer, (unsigned short)9);
	cairo_assert_eq(strcmp(buffer, "9"), 0);
	_cairo_format_type(buffer, 42u);
	cairo_assert_eq(strcmp(buffer, "42"), 0);
	_cairo_format_type(buffer, 42UL);
	cairo_assert_eq(strcmp(buffer, "42"), 0);
	_cairo_format_type(buffer, 42ULL);
	cairo_assert_eq(strcmp(buffer, "42"), 0);
	_cairo_format_type(buffer, (size_t)123);
	cairo_assert_eq(strcmp(buffer, "123"), 0);
}

cairo_test_new(demo2, check_cairo_record_value_floats) {
	char buffer[65];
	_cairo_format_type(buffer, 1.5f);
	cairo_assert_eq(strcmp(buffer, "1.5"), 0);
	_cairo_format_type(buffer, 2.25);
	cairo_assert_eq(strcmp(buffer, "2.25"), 0);
	_cairo_format_type(buffer, 3.125L);
	cairo_assert_eq(strcmp(buffer, "3.125"), 0);
}

cairo_test_new(demo2, check_cairo_record_value_pointer) {
	char buffer[65];
	int object = 0;
	_cairo_format_type(buffer, &object);
	cairo_assert_neq(strcmp(buffer, "null"), 0);
	cairo_assert_gt(strlen(buffer), (size_t)0);
}

cairo_test_new(demo2, check_cairo_record_value_truncates_safely) {
	char buffer[65];
	const char* const value =
		"0123456789012345678901234567890123456789"
		"0123456789012345678901234567890123456789"
		"0123456789012345678901234567890123456789";
	(void)memset(buffer, 'A', sizeof(buffer));
	_cairo_format_type(buffer, value);
	cairo_assert_lt(strlen(buffer), sizeof(buffer));
	cairo_assert_eq(buffer[sizeof(buffer) - 1], '\0');
}

cairo_test_new(demo2, check_cairo_assert_true) {
	cairo_assert_true(1);
	cairo_assert_true(!0);
	cairo_assert_true(true);
}

cairo_test_new(demo2, check_cairo_assert_false) {
	cairo_assert_false(0);
	cairo_assert_false(!1);
	cairo_assert_false(false);
}

cairo_test_new(demo2, check_cairo_assert_eq) {
	cairo_assert_eq(0, 0);
	cairo_assert_eq('a', 'a');
	cairo_assert_eq(NULL, NULL);
}

cairo_test_new(demo2, check_cairo_assert_neq) {
	cairo_assert_neq(0, 1);
	cairo_assert_neq('a', 'b');
}

cairo_test_new(demo2, check_cairo_assert_gt) {
	cairo_assert_gt(1, 0);
	cairo_assert_gt('b', 'a');
	cairo_assert_gt(1.0, 0.0);
}

cairo_test_new(demo2, check_cairo_assert_ge) {
	cairo_assert_ge(1, 0);
	cairo_assert_ge('b', 'a');
	cairo_assert_ge(1.0, 0.0);
	cairo_assert_ge(1, 1);
	cairo_assert_ge('b', 'b');
	cairo_assert_ge(1.0, 1.0);
}

cairo_test_new(demo2, check_cairo_assert_lt) {
	cairo_assert_lt(0, 1);
	cairo_assert_lt('a', 'b');
	cairo_assert_lt(0.0, 1.0);
}

cairo_test_new(demo2, check_cairo_assert_le) {
	cairo_assert_le(0, 1);
	cairo_assert_le('a', 'b');
	cairo_assert_le(0.0, 1.0);
	cairo_assert_le(1, 1);
	cairo_assert_le('b', 'b');
	cairo_assert_le(1.0, 1.0);
}

cairo_test_new(demo2, check_cairo_assert_nr) {
	cairo_assert_nr(0.0, 1e-6, 1e-6);
	cairo_assert_nr(0.0, 1e-7, 1e-6);
}

cairo_test_new(demo2, check_cairo_assert_nreq) {
	cairo_assert_nreq(0.0, 1e-6);
	cairo_assert_nreq(0.0, 1e-7);
}

int main(void) {
	return cairo_tests_run_default();
}
