
#include "stack.h"

void stack_init(stack_s* const stack) {
	stack->count = 0;
}

size_t stack_count(const stack_s* const stack) {
	return stack->count;
}

bool stack_is_empty(const stack_s* const stack) {
	return 0 == stack->count;
}

bool stack_is_full(const stack_s* const stack) {
	return stack_capacity == stack->count;
}

bool stack_push(stack_s* const stack, const int value) {
	if (stack_is_full(stack)) return false;
	stack->data[stack->count++] = value;
	return true;
}

bool stack_pop(stack_s* const stack, int* const out) {
	if (stack_is_empty(stack)) return false;
	*out = stack->data[--stack->count];
	return true;
}

bool stack_peek(const stack_s* const stack, int* const out) {
	if (stack_is_empty(stack)) return false;
	*out = stack->data[stack->count - 1];
	return true;
}
