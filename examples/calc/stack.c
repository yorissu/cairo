
#include "stack.h"

void stack_init(stack_s* const stack) {
	stack->count = 0;
}

size_t stack_count(const stack_s* const stack) {
	return stack->count;
}

bool stack_is_empty(const stack_s* const stack) {
	return stack->count == 0;
}

bool stack_push(stack_s* const stack, const int value) {
	if (stack_capacity == stack->count) return false;
	stack->data[stack->count++] = value;
	return true;
}

bool stack_pop(stack_s* const stack, int* const out) {
	if (0 == stack->count) return false;
	*out = stack->data[--stack->count];
	return true;
}
