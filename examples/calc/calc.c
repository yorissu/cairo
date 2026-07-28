
#include "calc.h"
#include "lexer.h"
#include "stack.h"

static bool calc_apply(const char op, const int a, const int b,
					   int* const out, calc_status_e* const err) {
	switch (op) {
		case '+': {
			*out = a + b;
			return true;
		} break;

		case '-': {
			*out = a - b;
			return true;
		} break;

		case '*': {
			*out = a * b;
			return true;
		} break;

		case '/': {
			if (0 == b) {
				*err = calc_status_err_div_zero;
				return false;
			}
			*out = a / b;
			return true;
		} break;

		default: {
			*err = calc_status_err_bad_token;
			return false;
		} break;
	}
}

calc_status_e calc_eval(const char* expr, int* const out) {
	lexer_s lexer;
	lexer_init(&lexer, expr);
	stack_s stack;
	stack_init(&stack);

	while (1) {
		const token_s token = lexer_next(&lexer);
		if (token_kind_end == token.type) break;
		if (token_kind_error == token.type) return calc_status_err_bad_token;

		if (token_kind_number == token.type) {
			if (!stack_push(&stack, token.value)) {
				return calc_status_err_overflow;
			}
			continue;
		}

		int a, b;
		if (!stack_pop(&stack, &b) || !stack_pop(&stack, &a)) {
			return calc_status_err_underflow;
		}

		int result;
		calc_status_e err = calc_status_ok;
		if (!calc_apply(token.op, a, b, &result, &err)) return err;
		(void)stack_push(&stack, result);
	}

	int value;
	if (!stack_pop(&stack, &value) || !stack_is_empty(&stack)) {
		return calc_status_err_leftover;
	}

	*out = value;
	return calc_status_ok;
}
