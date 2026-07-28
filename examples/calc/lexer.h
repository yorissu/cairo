
#ifndef __lexer_h__
#define __lexer_h__

#include <stdbool.h>
#include <stddef.h>

typedef enum {
	token_kind_number,
	token_kind_op    ,
	token_kind_end   ,
	token_kind_error ,
} token_kind_e;

typedef struct {
	token_kind_e type ;
	int          value;
	char         op   ;
} token_s;

typedef struct {
	const char* input;
	size_t      pos  ;
} lexer_s;

void lexer_init(lexer_s* const lexer, const char* input);

token_s lexer_next(lexer_s* const lexer);

#endif
