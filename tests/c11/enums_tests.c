
#include "cairo.h"

enum ProcessState {
	STATE_IDLE = 0,
	STATE_ACTIVE = 1,
	STATE_PAUSED = 2,
	STATE_HALTED = 3
};

enum StatusCodes {
	STATUS_OK = 0,
	STATUS_ERR_NOT_FOUND = -1,
	STATUS_ERR_TIMEOUT = -2
};

cairo_test_new(c11_enums, state_comparisons) {
	enum ProcessState current = STATE_ACTIVE;
	cairo_assert_eq(current, STATE_ACTIVE);
	cairo_assert_eq((int)current, 1);

	current = STATE_HALTED;
	cairo_assert_neq(current, STATE_ACTIVE);
}

cairo_test_new(c11_enums, negative_bounds) {
	enum StatusCodes code = STATUS_ERR_TIMEOUT;
	cairo_assert_eq(code, -2);
	cairo_assert_lt(code, STATUS_OK);
}

cairo_test_new(c11_enums, arithmetic_operations) {
	enum ProcessState current = STATE_IDLE;

	// Enum values promote to integers during operations
	unsigned int updated_state = current + 2;
	cairo_assert_eq(updated_state, STATE_PAUSED);
}
