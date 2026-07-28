
#include "lexer.h"

void lexer_init(lexer_s* const lexer, const char* input) {
	lexer->input = input;
	lexer->pos   = 0    ;
}

static bool lexer_is_op(const char c) {
	return ('+' == c) || ('-' == c) || ('*' == c) || ('/' == c);
}

static bool lexer_is_digit(const char c) {
	return (c >= '0') && (c <= '9');
}

token_s lexer_next(lexer_s* const lexer) {
	const char* const in = lexer->input;
	while (' ' == in[lexer->pos]) ++lexer->pos;

	if ('\0' == in[lexer->pos]) return (const token_s) {
		.type = token_kind_end,
	};

	const size_t start = lexer->pos;
	while ((in[lexer->pos] != '\0') && (in[lexer->pos] != ' ')) ++lexer->pos;
	const char* const tok = in + start        ;
	const size_t      len = lexer->pos - start;

	if ((1 == len) && lexer_is_op(tok[0])) return (const token_s) {
		.type = token_kind_op,
		.op   = tok[0]       ,
	};

	size_t index = 0    ;
	bool   neg   = false;
	if (('-' == tok[0]) || ('+' == tok[0])) {
		if (1 == len) return (const token_s) {
			.type = token_kind_error,
		};
		neg   = ('-' == tok[0]);
		index = 1              ;
	}

	int acc = 0;
	for (; index < len; ++index) {
		if (!lexer_is_digit(tok[index])) return (const token_s) {
			.type = token_kind_error,
		};
		acc = acc * 10 + (tok[index] - '0');
	}

	return (const token_s) {
		.type  = token_kind_number,
		.value = neg ? -acc : acc ,
	};
}
