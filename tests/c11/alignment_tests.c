
#include "cairo.h"

#include <stdalign.h>

struct CacheAlignedBlock {
	alignas(16) int payload;
};

struct PaddedData {
	char head;
	alignas(8) double aligned_element;
};

cairo_test_new(c11_alignment, boundary_specifiers) {
	cairo_assert_eq(alignof(char), 1);
	cairo_assert_ge(alignof(struct CacheAlignedBlock), 16);
}

cairo_test_new(c11_alignment, address_computations) {
	struct CacheAlignedBlock block;
	uintptr_t block_address = (uintptr_t)&block;
	cairo_assert_eq(block_address % 16, 0);

	struct PaddedData data;
	uintptr_t element_address = (uintptr_t)&data.aligned_element;
	cairo_assert_eq(element_address % 8, 0);
}
