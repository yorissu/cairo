
#include "cairo.h"

struct Dataset {
	int record_id;
	struct {                /* Anonymous nested struct */
		double latitude;
		double longitude;
	};
	union {                 /* Anonymous nested union */
		uint32_t raw_value;
		uint8_t  octets[4];
	};
};

cairo_test_new(c11_structs, anonymous_members) {
	struct Dataset data = {
		.record_id = 42,
		.latitude = 37.7749,
		.longitude = -122.4194,
		.raw_value = 0xAABBCCDD
	};

	cairo_assert_eq(data.record_id, 42);
	cairo_assert_nr(data.latitude, 37.7749, 1e-4);
	cairo_assert_nr(data.longitude, -122.4194, 1e-4);
	cairo_assert_eq(data.raw_value, 0xAABBCCDD);

	// Verify union overlay (assuming little-endian execution context for check)
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
	cairo_assert_eq(data.octets[0], 0xDD);
#endif
}

cairo_test_new(c11_structs, compound_literals) {
	struct Coordinates { int x; int y; };
	struct Coordinates point;

	// Assignment using a compound literal
	point = (struct Coordinates){ .x = 10, .y = 20 };

	cairo_assert_eq(point.x, 10);
	cairo_assert_eq(point.y, 20);
}
