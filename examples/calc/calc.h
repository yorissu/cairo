
#ifndef __calc_h__
#define __calc_h__

#include <stdbool.h>

typedef enum {
	calc_status_ok           ,
	calc_status_err_bad_token,
	calc_status_err_underflow,
	calc_status_err_div_zero ,
	calc_status_err_leftover ,
	calc_status_err_overflow ,
} calc_status_e;

calc_status_e calc_eval(const char* expr, int* const out);

#endif
