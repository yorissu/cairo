
#ifndef __stack_h__
#define __stack_h__

#include <stdbool.h>
#include <stddef.h>

#define stack_capacity 8

typedef struct {
	int    data[stack_capacity];
	size_t count               ;
} stack_s;

void stack_init(stack_s* const stack);

size_t stack_count(const stack_s* const stack);

bool stack_is_empty(const stack_s* const stack);

bool stack_is_full(const stack_s* const stack);

bool stack_push(stack_s* const stack, const int value);

bool stack_pop(stack_s* const stack, int* const out);

bool stack_peek(const stack_s* const stack, int* const out);

#endif
