
#include "lexer.h"

#include "cairo.h"

cairo_test_new(lexer, empty_yields_end) {
	lexer_s lexer;
	lexer_init(&lexer, "");
	cairo_assert_eq(lexer_next(&lexer).type, token_kind_end);
}

cairo_test_new(lexer, whitespace_only_yields_end) {
	lexer_s lexer;
	lexer_init(&lexer, "    ");
	cairo_assert_eq(lexer_next(&lexer).type, token_kind_end);
}

cairo_test_new(lexer, single_number) {
	lexer_s lexer;
	lexer_init(&lexer, "42");
	const token_s t = lexer_next(&lexer);
	cairo_assert_eq(t.type, token_kind_number);
	cairo_assert_eq(t.value, 42);
	cairo_assert_eq(lexer_next(&lexer).type, token_kind_end);
}

cairo_test_new(lexer, negative_number) {
	lexer_s lexer;
	lexer_init(&lexer, "-17");
	const token_s t = lexer_next(&lexer);
	cairo_assert_eq(t.type, token_kind_number);
	cairo_assert_eq(t.value, -17);
}

cairo_test_new(lexer, operators) {
	lexer_s lexer;
	lexer_init(&lexer, "+ - * /");
	cairo_assert_eq(lexer_next(&lexer).op, '+');
	cairo_assert_eq(lexer_next(&lexer).op, '-');
	cairo_assert_eq(lexer_next(&lexer).op, '*');
	cairo_assert_eq(lexer_next(&lexer).op, '/');
}

cairo_test_new(lexer, mixed_stream) {
	lexer_s lexer;
	lexer_init(&lexer, "3 4 +");
	token_s t = lexer_next(&lexer);
	cairo_assert_eq(t.type, token_kind_number);
	cairo_assert_eq(t.value, 3);
	t = lexer_next(&lexer);
	cairo_assert_eq(t.type, token_kind_number);
	cairo_assert_eq(t.value, 4);
	t = lexer_next(&lexer);
	cairo_assert_eq(t.type, token_kind_op);
	cairo_assert_eq(t.op, '+');
	cairo_assert_eq(lexer_next(&lexer).type, token_kind_end);
}

cairo_test_new(lexer, malformed_token) {
	lexer_s lexer;
	lexer_init(&lexer, "12x");
	cairo_assert_eq(lexer_next(&lexer).type, token_kind_error);
}

cairo_test_new(lexer, lone_minus_is_operator) {
	lexer_s lexer;
	lexer_init(&lexer, "-");
	const token_s t = lexer_next(&lexer);
	cairo_assert_eq(t.type, token_kind_op);
	cairo_assert_eq(t.op, '-');
}

cairo_test_new(lexer, sign_without_digits_is_error) {
	lexer_s lexer;
	lexer_init(&lexer, "-x");
	cairo_assert_eq(lexer_next(&lexer).type, token_kind_error);
}
