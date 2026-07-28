
#include "cairo.h"

// Custom type-generic absolute value macro utilizing C11 _Generic
#define generic_abs(x) _Generic((x), \
	int:    abs,                     \
	float:  fabsf,                   \
	double: fabs                     \
)(x)

cairo_test_new(c11_maths, generic_selection) {
	int integer_val = -100;
	float float_val = -25.5f;
	double double_val = -10.123456;

	cairo_assert_eq(generic_abs(integer_val), 100);
	cairo_assert_nr(generic_abs(float_val), 25.5f, 1e-5);
	cairo_assert_nr(generic_abs(double_val), 10.123456, 1e-6);
}

cairo_test_new(c11_maths, floating_point_precision) {
	double value_a = 0.1;
	double value_b = 0.2;
	double sum = value_a + value_b;

	// Verifies floating-point sums within strict epsilon limits
	cairo_assert_nr(sum, 0.3, 1e-9);
}
