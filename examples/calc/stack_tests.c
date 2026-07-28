
#include "stack.h"

#include "cairo.h"

cairo_test_new(stack, starts_empty) {
	stack_s stack;
	stack_init(&stack);
	cairo_assert_true(stack_is_empty(&stack));
	cairo_assert_eq(stack_count(&stack), (size_t)0);
}

cairo_test_new(stack, push_pop_lifo) {
	stack_s stack;
	stack_init(&stack);
	stack_push(&stack, 1);
	stack_push(&stack, 2);
	stack_push(&stack, 3);
	int out = 0;
	cairo_assert_true(stack_pop(&stack, &out));
	cairo_assert_eq(out, 3);
	cairo_assert_true(stack_pop(&stack, &out));
	cairo_assert_eq(out, 2);
	cairo_assert_true(stack_pop(&stack, &out));
	cairo_assert_eq(out, 1);
	cairo_assert_true(stack_is_empty(&stack));
}

cairo_test_new(stack, pop_empty_fails) {
	stack_s stack;
	stack_init(&stack);
	int out = -1;
	cairo_assert_false(stack_pop(&stack, &out));
	cairo_assert_eq(out, -1);
}

cairo_test_new(stack, push_full_fails) {
	stack_s stack;
	stack_init(&stack);
	for (int index = 0; index < stack_capacity; ++index) {
		cairo_assert_true(stack_push(&stack, index));
	}
	cairo_assert_false(stack_push(&stack, 999));
	cairo_assert_eq(stack_count(&stack), (size_t)stack_capacity);
}
