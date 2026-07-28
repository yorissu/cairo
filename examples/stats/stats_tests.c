
/// 
/// This example is meant to fail. It shows what cairo prints when assertions do
/// not hold: the source line, a caret under the failing expression, and both
/// evaluated operands.
///
/// The return value of cairo_run_all is deliberately discarded so that the
/// aggregate `run` targets in the build files carry on to the next example
/// instead of stopping here. Drop the cast if you would rather it fail loudly.
/// 
/// The assertions in this file are all correct. The implementation they test is
/// not - every failure below is a real bug in stats.c, reported by cairo.
/// 

#include "stats.h"

#include "cairo.h"

#include <string.h>

cairo_test_new(stats, min_of_a_mixed_set) {
	const double values[] = { 4.0, -2.0, 9.0, 1.0, };
	cairo_assert_nreq(stats_min(values, 4), -2.0);
}

cairo_test_new(stats, min_of_an_empty_set) {
	cairo_assert_nreq(stats_min(NULL, 0), 0.0);
}

cairo_test_new(stats, max_of_a_mixed_set) {
	const double values[] = { 4.0, -2.0, 9.0, 1.0, };
	cairo_assert_nreq(stats_max(values, 4), 9.0);
}

cairo_test_new(stats, mean_of_an_even_spread) {
	const double values[] = { 2.0, 4.0, 6.0, 8.0, };
	cairo_assert_nr(stats_mean(values, 4), 5.0, 1e-9);
}

cairo_test_new(stats, mean_of_an_empty_set) {
	cairo_assert_nreq(stats_mean(NULL, 0), 0.0);
}

cairo_test_new(stats, count_above_excludes_the_threshold) {
	const double values[] = { 1.0, 2.0, 3.0, 4.0, };
	cairo_assert_eq(stats_count_above(values, 4, 2.0), (size_t)2);
}

cairo_test_new(stats, count_above_a_ceiling_is_empty) {
	const double values[] = { 1.0, 2.0, 3.0, };
	cairo_assert_eq(stats_count_above(values, 3, 99.0), (size_t)0);
}

cairo_test_new(stats, describe_below_zero) {
	cairo_assert_eq(strcmp(stats_describe(-0.5), "negative"), 0);
}

cairo_test_new(stats, describe_below_one) {
	cairo_assert_eq(strcmp(stats_describe(0.5), "low"), 0);
}

cairo_test_new(stats, describe_at_and_above_one) {
	cairo_assert_eq(strcmp(stats_describe(5.0), "high"), 0);
}

cairo_test_new(stats, is_sorted_accepts_ascending) {
	const double values[] = { 1.0, 2.0, 3.0, };
	cairo_assert_true(stats_is_sorted(values, 3));
}

cairo_test_new(stats, is_sorted_rejects_a_leading_drop) {
	const double values[] = { 3.0, 1.0, 2.0, };
	cairo_assert_false(stats_is_sorted(values, 3));
}

cairo_test_new(stats, variance_is_not_implemented_yet) {
	cairo_test_skip();
}

int main(const int argc, const char* argv[]) {
	(void)cairo_tests_run(cairo_args_new(argc, argv));
	return 0;
}
