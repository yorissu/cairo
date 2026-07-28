
#include "stack.h"

#include "cairo.h"

cairo_test_new(stack, starts_empty) {
	stack_s stack;
	stack_init(&stack);
	cairo_assert_true(stack_is_empty(&stack));
	cairo_assert_false(stack_is_full(&stack));
	cairo_assert_eq(stack_count(&stack), (size_t)0);
}

cairo_test_new(stack, push_increments_count) {
	stack_s stack;
	stack_init(&stack);
	cairo_assert_true(stack_push(&stack, 42));
	cairo_assert_eq(stack_count(&stack), (size_t)1);
	cairo_assert_false(stack_is_empty(&stack));
}

cairo_test_new(stack, pops_in_reverse_order) {
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

cairo_test_new(stack, peek_leaves_element) {
	stack_s stack;
	stack_init(&stack);
	stack_push(&stack, 7);
	int out = 0;
	cairo_assert_true(stack_peek(&stack, &out));
	cairo_assert_eq(out, 7);
	cairo_assert_eq(stack_count(&stack), (size_t)1);
}

cairo_test_new(stack, pop_empty_fails) {
	stack_s stack;
	stack_init(&stack);
	int out = -1;
	cairo_assert_false(stack_pop(&stack, &out));
	cairo_assert_eq(out, -1);
}

cairo_test_new(stack, peek_empty_fails) {
	stack_s stack;
	stack_init(&stack);
	int out = -1;
	cairo_assert_false(stack_peek(&stack, &out));
}

cairo_test_new(stack, fills_to_capacity) {
	stack_s stack;
	stack_init(&stack);
	for (int index = 0; index < stack_capacity; ++index) {
		cairo_assert_true(stack_push(&stack, index));
	}
	cairo_assert_true(stack_is_full(&stack));
	cairo_assert_eq(stack_count(&stack), (size_t)stack_capacity);
}

cairo_test_new(stack, push_when_full_fails) {
	stack_s stack;
	stack_init(&stack);
	for (int index = 0; index < stack_capacity; ++index) {
		stack_push(&stack, index);
	}
	cairo_assert_false(stack_push(&stack, 999));
	cairo_assert_eq(stack_count(&stack), (size_t)stack_capacity);
}

int main(int argc, const char* argv[]) {
	return cairo_tests_run(cairo_args_new(argc, argv));
}
