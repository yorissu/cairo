
#ifndef __cairo_h__
#define __cairo_h__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <math.h>

/// keeping everything 'static inline' lets the whole framework live in a single
/// header file without producing duplicate external symbols when it is included
/// in many translation units.
#define _cairo_func static inline

/// needed to resolve 'unused parameter' warning/error in empty test functions.
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ > 201710L)
#	define _cairo_unused [[maybe_unused]]
#elif defined(__GNUC__) || defined(__clang__)
#	define _cairo_unused __attribute__((unused))
#else
#	define _cairo_unused
#endif

/// emits the 'retain' attribute prefix when the toolchain understands it so the
/// linker keeps test records that are never referenced by name.
#if defined(__has_attribute) && __has_attribute(retain)
#	define _cairo_retain retain,
#else
#	define _cairo_retain
#endif

typedef struct _cairo_test_s _cairo_test_s;

/// cross-toolchain handling for special cairo linker section which stores every
/// defined cairo test. each 'cairo_test_new' stores its test's reference in the
/// mentioned cairo section. '_cairo_begin' and '_cairo_end' mark that section's
/// bounds at link time so the cairo runner can walk it without any central test
/// registration list.
#if defined(__ELF__)
#	define _cairo_section                                                      \
		__attribute__((used, _cairo_retain section("cairo")))
extern _cairo_test_s* __start_cairo;
extern _cairo_test_s* __stop_cairo;
#	define _cairo_begin (&__start_cairo)
#	define _cairo_end (&__stop_cairo)
#elif defined(_MSC_VER)
#	pragma section("cairo$a", read, write)
#	pragma section("cairo$m", read, write)
#	pragma section("cairo$z", read, write)
__declspec(allocate("cairo$a")) static _cairo_test_s* _cairo_sec_start = NULL;
__declspec(allocate("cairo$z")) static _cairo_test_s* _cairo_sec_stop  = NULL;
#	define _cairo_section                                                      \
		__declspec(allocate("cairo$m"))
#	define _cairo_begin (&_cairo_sec_start + 1)
#	define _cairo_end (&_cairo_sec_stop)
#elif defined(__APPLE__)
#	define _cairo_section                                                      \
		__attribute__((used, section("__DATA,__cairo,regular,no_dead_strip")))
extern _cairo_test_s* _cairo_sec_start __asm("section$start$__DATA$__cairo");
extern _cairo_test_s* _cairo_sec_stop  __asm("section$end$__DATA$__cairo");
#	define _cairo_begin (&_cairo_sec_start)
#	define _cairo_end (&_cairo_sec_stop)
#else
#	error "cairo: unsupported toolchain!"
#endif

/// returns a monotonic-ish timestamp in seconds that's used to measure how long
/// each test takes.
/// todo: expose clock functions for user to set in non standard environments.
/// fixme: posix clock()'s cpu time (blocking reads as free); windows clock() is
/// wall time at ~1-15ms resolution; both are poor and should be replaced with a
/// better approach.
_cairo_func double _cairo_time_now(void) {
	return (double)clock() / (double)CLOCKS_PER_SEC;
}

/// formats a duration (in seconds) into a short, human-readable string, picking
/// microseconds, milliseconds, or seconds so the number stays readable.
/// note: returns a pointer to function-local static buffer which is not thread-
/// safe and can be overwritten by the next call.
_cairo_func const char* _cairo_format_time(const double time) {
	static char buffer[65];
	const size_t size = sizeof(buffer);
	if (time < 1e-3)     (void)snprintf(buffer, size, "%.3fus", time * 1e6);
	else if (time < 1.0) (void)snprintf(buffer, size, "%.3fms", time * 1e3);
	else                 (void)snprintf(buffer, size, "%.3fs", time);
	return buffer;
}

/// matches provided 'text' against a glob 'pattern'. runs in linear time, using
/// a backtracking star/mark pair rather than recursion. it's used to select and
/// exclude tests by their 'suite.name'.
_cairo_func bool _cairo_get_glob(const char* pattern, const char* text) {
	const char* star = NULL;
	const char* mark = NULL;

	while (*text != '\0') {
		if (('?' == *pattern) || (*pattern == *text)) {
			++pattern;
			++text;
			continue;
		}

		if ('*' == *pattern) {
			star = pattern++;
			mark = text;
			continue;
		}

		if (star != NULL) {
			pattern = star + 1;
			text = ++mark;
			continue;
		}

		return false;
	}

	while ('*' == *pattern) ++pattern;
	return '\0' == *pattern;
}

/// outcome/status of a single test: failed, passed, or explicitly skipped.
typedef enum {
	_cairo_test_status_fail,
	_cairo_test_status_pass,
	_cairo_test_status_skip,
} _cairo_test_status_e;

/// recorded details of failed assertions: an operator and stringified operands,
/// the source location, and the two operand values already formatted to strings
/// for reporting.
typedef struct {
	const char* lhs     ;
	const char* rhs     ;
	const char* op      ;
	const char* file    ;
	size_t      line    ;
	char        lhsb[65];
	char        rhsb[65];
} _cairo_test_fail_s;

/// a registered test that holds the test function, its suite/name identity, the
/// status and failure record, the measured elapsed time, and whether the filter
/// selected it to run.
struct _cairo_test_s {
	void(*func)(_cairo_test_s* const)       ;
	const char*          suite              ;
	const char*          name               ;
	const char*          suite_and_name     ;
	_cairo_test_status_e status             ;
	_cairo_test_fail_s   fail               ;
	double               elapsed            ;
	bool                 shall_run          ;
};

/// builds the internal symbol name for a test function from suite and name.
#define _f(_suite, _name)                                                      \
	_suite ## _ ## _name ## _func

/// builds the internal symbol name for a test instance from suite and name.
#define _t(_suite, _name)                                                      \
	_ ## _suite ## _ ## _name ## _test

/// builds the internal symbol name for a test reference from suite and name.
#define _r(_suite, _name)                                                      \
	_ ## _suite ## _ ## _name ## _ref

/// places the cairo test '_name' reference into the cairo section and points it
/// at the '_value' that is a test object which makes the test discoverable by a
/// cairo runner.
#define _cairo_test_ref(_name, _value)                                         \
	_cairo_section static _cairo_test_s* _name = _value

/// it forward-declares the test function, its '_cairo_test_s' test object, that
/// is initialized to pass, registers that object in the section, and then opens
/// the body of the test function for you to fill in. inside the body the hidden
/// '_test' handle carries status and failure info which is needed by the assert
/// macros to reference. should be used like a function header:
/// cairo_test_new(demo1, test01) {
///     cairo_assert_eq(2 + 2, 4);
/// }
#define cairo_test_new(_suite, _name)                                          \
	static void _f(_suite, _name)(_cairo_unused _cairo_test_s* const _test);   \
	                                                                           \
	static _cairo_test_s _t(_suite, _name) = {                                 \
		.func           = _f(_suite, _name)      ,                             \
		.suite          = #_suite                ,                             \
		.name           = #_name                 ,                             \
		.suite_and_name = #_suite "." #_name     ,                             \
		.status         = _cairo_test_status_fail,                             \
		.fail = {                                                              \
			.lhs  = NULL     ,                                                 \
			.rhs  = NULL     ,                                                 \
			.op   = NULL     ,                                                 \
			.file = NULL     ,                                                 \
			.line = (size_t)0,                                                 \
			.lhsb = {0}      ,                                                 \
			.rhsb = {0}      ,                                                 \
		},                                                                     \
		.elapsed   = 0.0  ,                                                    \
		.shall_run = false,                                                    \
	};                                                                         \
	                                                                           \
	_cairo_test_ref(_r(_suite, _name), &_t(_suite, _name));                    \
	                                                                           \
	static void _f(_suite, _name)(_cairo_unused _cairo_test_s* const _test)

/// runs one test: resets its status to pass, times the call to its body, stores
/// the elapsed duration. the test's body sets its status to fail or skip by the
/// the cairo_assert*/cairo_test_skip macros.
_cairo_func void _cairo_test_run(_cairo_test_s* const test) {
	test->status = _cairo_test_status_pass;
	const double begin = _cairo_time_now();
	test->func(test);
	test->elapsed = _cairo_time_now() - begin;
}

/// qsort comparator over '_cairo_test_s*' which orders tests by suite and name,
/// so the output is grouped and stable.
/// note: null test references exist because the tests section is populated with
/// such values by certain toolchains to handle padding. cairo does so too, when
/// no tests are defined, certain toolchains fail with empty linker section - we
/// create an empty, not-runnable dummy test refernce to a null test to make the
/// linker happy.
_cairo_func int _cairo_test_compare(const void* const a, const void* const b) {
	const _cairo_test_s* const lhs = *(const _cairo_test_s* const*)a;
	const _cairo_test_s* const rhs = *(const _cairo_test_s* const*)b;
	if (NULL == lhs) return (NULL == rhs) ? 0 : 1;
	if (NULL == rhs) return -1;
	const int c = strcmp(lhs->suite, rhs->suite);
	return c != 0 ? c : strcmp(lhs->name, rhs->name);
}

/// returns whether a test's 'suite.name' matches the given glob pattern.
_cairo_func bool _cairo_test_match(const _cairo_test_s* const test,
								   const char* const pattern) {
	return _cairo_get_glob(pattern, test->suite_and_name);
}

// todo: document!
_cairo_func void _cairo_test_report(const _cairo_test_s* const test,
									const bool verbose) {
	if (verbose) {
#ifndef cairo_tab_width
#	define cairo_tab_width 4
#endif
#define _cairo_line_length (256 + (32 * cairo_tab_width))
#define _cairo_max_lines 16

		char   lines[_cairo_max_lines][_cairo_line_length];
		int    numbers[_cairo_max_lines];
		size_t mark_start[_cairo_max_lines];
		size_t mark_end[_cairo_max_lines];
		int    have   = 0;
		size_t column = 1;

		FILE* const src = fopen(test->fail.file, "rt");
		if (src != NULL) {
			const int  target = (int)test->fail.line;
			const int  first  = (target > 1) ? (target - 1) : 1;
			char raw[_cairo_line_length];
			int  no      = 0;
			int  depth   = 0;
			char quote   = '\0';
			bool started = false;
			bool done    = false;
			int  trailing = 0;

			while ((have < _cairo_max_lines) && (fgets(raw, sizeof(raw), src) != NULL)) {
				++no;
				if (no < first) { continue; }

				char* const dst = lines[have];
				size_t count = 0;
				for (const char* s = raw; (*s != '\0') && ((count + 1) < _cairo_line_length); ++s) {
					if (('\n' == *s) || ('\r' == *s)) { break; }
					if ('\t' == *s) {
						const size_t padding = cairo_tab_width - (count % cairo_tab_width);
						for (size_t _ = 0; (_ < padding) && ((count + 1) < _cairo_line_length); ++_) {
							dst[count++] = ' ';
						}
					}
					else {
						dst[count++] = *s;
					}
				}
				dst[count] = '\0';

				numbers[have]   = no;
				mark_start[have] = 0;
				mark_end[have]   = 0;

				if ((no >= target) && !done) {
					const char* const line = lines[have];
					const char* p = line;

					if ((no == target) && !started) {
						const char* const call = strstr(line, "cairo_assert_");
						if (call != NULL) {
							p = call;
							while ((*p != '\0') && (*p != '(')) { ++p; }
						}
					}

					size_t line_open = (size_t)-1;
					for (; *p != '\0'; ++p) {
						const char c = *p;
						if (quote != '\0') {
							if ((c == '\\') && (p[1] != '\0')) { ++p; continue; }
							if (c == quote) { quote = '\0'; }
							continue;
						}
						if ((c == '"') || (c == '\'')) { quote = c; continue; }

						if (c == '(') {
							if (!started) { started = true; line_open = (size_t)(p - line); }
							++depth;
						}
						else if (c == ')') {
							--depth;
							if (0 == depth) {
								size_t s = (line_open != (size_t)-1) ? (line_open + 1) : 0;
								size_t e = (size_t)(p - line);
								while ((s < e) && (line[s] == ' ')) { ++s; }
								while ((e > s) && (line[e - 1] == ' ')) { --e; }
								mark_start[have] = s;
								mark_end[have]   = e;
								done = true;
								break;
							}
						}
					}

					if (!done) {
						size_t s = (line_open != (size_t)-1) ? (line_open + 1) : 0;
						size_t e = strlen(line);
						if (line_open == (size_t)-1) { while (line[s] == ' ') { ++s; } }
						while ((e > s) && (line[e - 1] == ' ')) { --e; }
						mark_start[have] = s;
						mark_end[have]   = e;
					}

					if (mark_end[have] > mark_start[have]) { column = mark_start[have] + 1; }
				}

				++have;
				if (done && (trailing++ > 0)) { break; }
			}
			(void)fclose(src);
		}

		if (test->fail.op != NULL) {
			(void)printf("\n%s:%zu:%zu: %s failed: %s %s %s\n",
				test->fail.file, test->fail.line, column, test->suite_and_name,
				test->fail.lhs, test->fail.op, test->fail.rhs);
		}
		else {
			(void)printf("\n%s:%zu:%zu: %s failed: %s\n",
				test->fail.file, test->fail.line, column, test->suite_and_name,
				(test->fail.lhs != NULL) ? test->fail.lhs : "assertion failed");
		}

		for (int i = 0; i < have; ++i) {
			(void)printf("  %4d | %s\n", numbers[i], lines[i]);
			if (mark_end[i] <= mark_start[i]) continue;
			(void)printf("       | ");
			for (size_t c = 0; c < mark_start[i]; ++c) (void)putchar(' ');
			(void)putchar('^');
			for (size_t c = mark_start[i] + 1; c < mark_end[i]; ++c) (void)putchar('~');
			(void)putchar('\n');
		}

		(void)printf("       |\n");
		if (test->fail.op != NULL) {
			(void)printf("       = left:  %s\n", test->fail.lhsb);
			(void)printf("       = right: %s\n", test->fail.rhsb);
		}
		(void)printf("       (%s)\n", _cairo_format_time(test->elapsed));
	}
	else {
		if (test->fail.op != NULL) {
			(void)printf("\n%s:%zu: %s failed: %s %s %s\n",
				test->fail.file, test->fail.line, test->suite_and_name,
				test->fail.lhs, test->fail.op, test->fail.rhs);
		}
		else {
			(void)printf("\n%s:%zu: %s failed: %s\n",
				test->fail.file, test->fail.line, test->suite_and_name,
				(test->fail.lhs != NULL) ? test->fail.lhs : "assertion failed");
		}
	}
}

/// skips the current test from inside its body. only valid inside a test's body
/// (it uses the hidden '_test' handle and returns from the test function).
#define cairo_test_skip() do {                                                 \
		_test->status = _cairo_test_status_skip;                               \
		return;                                                                \
	} while (0)

/// records a failure into the '_test' handle: sets the stringified operands and
/// operator and source location, and clears value buffers.
#define _cairo_record_fail(_lhs, _rhs, _op, _file, _line) do {                 \
		_test->status       = _cairo_test_status_fail;                         \
		_test->fail.lhs     = _lhs                   ;                         \
		_test->fail.rhs     = _rhs                   ;                         \
		_test->fail.op      = _op                    ;                         \
		_test->fail.file    = _file                  ;                         \
		_test->fail.line    = (size_t)(_line)        ;                         \
		_test->fail.lhsb[0] = '\0'                   ;                         \
		_test->fail.rhsb[0] = '\0'                   ;                         \
	} while (0)

/// evaluates '_expr', then on a falsy result it records the stringified '_expr'
/// as the failure and returns from the enclosing test aborting whatever remains
/// of it.
#define _cairo_assert_unary(_expr) do {                                        \
		if (!(_expr)) {                                                        \
			_cairo_record_fail(#_expr, NULL, NULL, __FILE__, __LINE__);        \
			return;                                                            \
		}                                                                      \
	} while (0)

/// formats a char value into a buffer of provided size as text.
_cairo_func void _cairo_format_char(char* const buffer, const size_t size,
														const char value) {
	(void)snprintf(buffer, size, "%c", value);
}

/// formats a signed char value into a buffer of provided size as text.
_cairo_func void _cairo_format_schar(char* const buffer, const size_t size,
									 const signed char value) {
	(void)snprintf(buffer, size, "%hhd", value);
}

/// formats a unsigned char value into a buffer of provided size as text.
_cairo_func void _cairo_format_uchar(char* const buffer, const size_t size,
									 const unsigned char value) {
	(void)snprintf(buffer, size, "%hhu", value);
}

/// formats a bool value into a buffer of provided size as text.
_cairo_func void _cairo_format_bool(char* const buffer, const size_t size,
														const bool value) {
	(void)snprintf(buffer, size, "%s", value ? "true" : "false");
}

/// formats a signed short value into a buffer of provided size as text.
_cairo_func void _cairo_format_short(char* const buffer, const size_t size,
									 const signed short value) {
	(void)snprintf(buffer, size, "%hd", value);
}

/// formats a unsigned short value into a buffer of provided size as text.
_cairo_func void _cairo_format_ushort(char* const buffer, const size_t size,
									  const unsigned short value) {
	(void)snprintf(buffer, size, "%hu", value);
}

/// formats a signed int value into a buffer of provided size as text.
_cairo_func void _cairo_format_int(char* const buffer, const size_t size,
								   const signed int value) {
	(void)snprintf(buffer, size, "%d", value);
}

/// formats a unsigned int value into a buffer of provided size as text.
_cairo_func void _cairo_format_uint(char* const buffer, const size_t size,
									const unsigned int value) {
	(void)snprintf(buffer, size, "%u", value);
}

/// formats a signed long value into a buffer of provided size as text.
_cairo_func void _cairo_format_long(char* const buffer, const size_t size,
									const signed long value) {
	(void)snprintf(buffer, size, "%ld", value);
}

/// formats a unsigned long value into a buffer of provided size as text.
_cairo_func void _cairo_format_ulong(char* const buffer, const size_t size,
									 const unsigned long value) {
	(void)snprintf(buffer, size, "%lu", value);
}

/// formats a signed long long value into a buffer of provided size as text.
_cairo_func void _cairo_format_llong(char* const buffer, const size_t size,
									 const signed long long value) {
	(void)snprintf(buffer, size, "%lld", value);
}

/// formats a unsigned long long value into a buffer of provided size as text.
_cairo_func void _cairo_format_ullong(char* const buffer, const size_t size,
									  const unsigned long long value) {
	(void)snprintf(buffer, size, "%llu", value);
}

/// formats a float value into a buffer of provided size as text.
_cairo_func void _cairo_format_float(char* const buffer, const size_t size,
														 const float value) {
	(void)snprintf(buffer, size, "%g", (double)value);
}

/// formats a double value into a buffer of provided size as text.
_cairo_func void _cairo_format_double(char* const buffer, const size_t size,
														  const double value) {
	(void)snprintf(buffer, size, "%g", value);
}

/// formats a long double value into a buffer of provided size as text.
_cairo_func void _cairo_format_ldouble(char* const buffer, const size_t size,
									   const long double value) {
	(void)snprintf(buffer, size, "%Lg", value);
}

/// formats a char* value into a buffer of provided size as text.
_cairo_func void _cairo_format_string(char* const buffer, const size_t size,
									  const char* const value) {
	if (NULL == value) { (void)snprintf(buffer, size, "null"); return; }
	(void)snprintf(buffer, size, "\"%s\"", value);
}

/// formats a void* value into a buffer of provided size as text.
_cairo_func void _cairo_format_pointer(char* const buffer, const size_t size,
									   const void* const value) {
	if (NULL == value) { (void)snprintf(buffer, size, "null"); return; }
	(void)snprintf(buffer, size, "%p", value);
}

/// formats a '_value' into character buffer '_buffer' by selecting the matching
/// '_cairo_format_*' formatter for the value's type at compile time. This way a
/// failed assertion can print 'left' and 'right' values without the test caller
/// naming the types.
/// note: the 'default' case handles any pointer type as a raw address.
#define _cairo_record_value(_buffer, _value) _Generic((_value),                \
		char:               _cairo_format_char   ,                             \
		signed char:        _cairo_format_schar  ,                             \
		unsigned char:      _cairo_format_uchar  ,                             \
		_Bool:              _cairo_format_bool   ,                             \
		signed short:       _cairo_format_short  ,                             \
		unsigned short:     _cairo_format_ushort ,                             \
		signed int:         _cairo_format_int    ,                             \
		unsigned int:       _cairo_format_uint   ,                             \
		signed long:        _cairo_format_long   ,                             \
		unsigned long:      _cairo_format_ulong  ,                             \
		signed long long:   _cairo_format_llong  ,                             \
		unsigned long long: _cairo_format_ullong ,                             \
		float:              _cairo_format_float  ,                             \
		double:             _cairo_format_double ,                             \
		long double:        _cairo_format_ldouble,                             \
		char*:              _cairo_format_string ,                             \
		const char*:        _cairo_format_string ,                             \
		default:            _cairo_format_pointer                              \
	)((_buffer), sizeof(_buffer), (_value))

/// evaluates '_lhs' '_op' '_rhs', when that is false it records the stringified
/// operands, the operator, and the source location, formats both operand values
/// into the failure buffers, and returns from the enclosing test.
/// note: all operands are evaluated twice (once to compare and once to format),
/// so avoid arguments with side effects.
#define _cairo_assert_binary(_lhs, _rhs, _op) do {                             \
		if (!((_lhs) _op (_rhs))) {                                            \
			_cairo_record_fail(#_lhs, #_rhs, #_op, __FILE__, __LINE__);        \
			_cairo_record_value(_test->fail.lhsb, _lhs);                       \
			_cairo_record_value(_test->fail.rhsb, _rhs);                       \
			return;                                                            \
		}                                                                      \
	} while (0)

/// evaluates as pass when '_lhs' and '_rhs' are within '_epsilon' of each other
/// (|lhs - rhs| <= epsilon, computed in double). on failure it records operands
/// with the '~=' operator, formats '_lhs' and '_rhs' values into the respective
/// failure buffers, and returns from the enclosing test.
/// note: prefer these over '_eq'/'_neq' for real numbers.
/// note: all operands are evaluated twice (once to compare and once to format),
/// so avoid arguments with side effects.
#define _cairo_assert_loosely(_lhs, _rhs, _epsilon) do {                       \
		if (!(fabs((double)(_lhs) - (double)(_rhs)) <= (_epsilon))) {          \
			_cairo_record_fail(#_lhs, #_rhs, "~=", __FILE__, __LINE__);        \
			_cairo_record_value(_test->fail.lhsb, _lhs);                       \
			_cairo_record_value(_test->fail.rhsb, _rhs);                       \
			return;                                                            \
		}                                                                      \
	} while (0)

/// public assertion, for use inside a test body. on failure, assert records the
/// location and operands and aborts the rest of its test.
/// note: cairo_assert_true(x) - x is truthy.
#define cairo_assert_true(_expr) do {                                          \
		_cairo_assert_unary(_expr);                                            \
	} while (0)

/// public assertion, for use inside a test body. on failure, assert records the
/// location and operands and aborts the rest of its test.
/// note: cairo_assert_false(x) - x is falsy.
#define cairo_assert_false(_expr) do {                                         \
		_cairo_assert_unary(!(_expr));                                         \
	} while (0)

/// public assertion, for use inside a test body. on failure, assert records the
/// location and operands and aborts the rest of its test.
/// note: cairo_assert_eq(lhs, rhs) - lhs == rhs.
#define cairo_assert_eq(_lhs, _rhs) do {                                       \
		_cairo_assert_binary(_lhs, _rhs, ==);                                  \
	} while (0)

/// public assertion, for use inside a test body. on failure, assert records the
/// location and operands and aborts the rest of its test.
/// note: cairo_assert_neq(lhs, rhs) - lhs != rhs.
#define cairo_assert_neq(_lhs, _rhs) do {                                      \
		_cairo_assert_binary(_lhs, _rhs, !=);                                  \
	} while (0)

/// public assertion, for use inside a test body. on failure, assert records the
/// location and operands and aborts the rest of its test.
/// note: cairo_assert_gt(lhs, rhs) - lhs > rhs.
#define cairo_assert_gt(_lhs, _rhs) do {                                       \
		_cairo_assert_binary(_lhs, _rhs, >);                                   \
	} while (0)

/// public assertion, for use inside a test body. on failure, assert records the
/// location and operands and aborts the rest of its test.
/// note: cairo_assert_ge(lhs, rhs) - lhs >= rhs.
#define cairo_assert_ge(_lhs, _rhs) do {                                       \
		_cairo_assert_binary(_lhs, _rhs, >=);                                  \
	} while (0)

/// public assertion, for use inside a test body. on failure, assert records the
/// location and operands and aborts the rest of its test.
/// note: cairo_assert_lt(lhs, rhs) - lhs < rhs.
#define cairo_assert_lt(_lhs, _rhs) do {                                       \
		_cairo_assert_binary(_lhs, _rhs, <);                                   \
	} while (0)

/// public assertion, for use inside a test body. on failure, assert records the
/// location and operands and aborts the rest of its test.
/// note: cairo_assert_le(lhs, rhs) - lhs <= rhs.
#define cairo_assert_le(_lhs, _rhs) do {                                       \
		_cairo_assert_binary(_lhs, _rhs, <=);                                  \
	} while (0)

/// public assertion, for use inside a test body. on failure, assert records the
/// location and operands and aborts the rest of its test.
/// note: cairo_assert_nr(lhs, rhs epsilon) - lhs and rhs are within epsilon.
/// note: use the '_nr'/'_nreq' forms for floating-point comparisons; '_eq' form
/// on floats compares bit-exactly and will surprise you.
#define cairo_assert_nr(_lhs, _rhs, _epsilon) do {                             \
		_cairo_assert_loosely(_lhs, _rhs, _epsilon);                           \
	} while (0)

/// public assertion, for use inside a test body. on failure, assert records the
/// location and operands and aborts the rest of its test.
/// note: cairo_assert_nreq(lhs, rhs) - lhs and rhs are within 1e-6 epsilon.
/// note: use the '_nr'/'_nreq' forms for floating-point comparisons; '_eq' form
/// on floats compares bit-exactly and will surprise you.
#define cairo_assert_nreq(_lhs, _rhs) do {                                     \
		cairo_assert_nr(_lhs, _rhs, 1e-6);                                     \
	} while (0)

/// parse-status of the command line and ordered so the enumerator value doubles
/// as an exit code: 'exit' for a clean, early stop, 'go_on' to keep the program
/// running, 'error' for a usage error.
/// note: keep this order, 'cairo_tests_run' returns '(int)args._status' directly
/// for the non-'go_on' cases, so reordering silently changes exit code mapping.
typedef enum {
	_cairo_args_status_exit ,
	_cairo_args_status_go_on,
	_cairo_args_status_error,
} _cairo_args_status_e;

/// parsed command-line options configuring a tests run: which tests to include,
/// which to exclude, how many times to repeat the tests set, and what exit code
/// to use when tests fail. the 'status' reports how parsing ended.
typedef struct {
	_cairo_args_status_e _status;
	const char*          pattern;
	const char*          exclude;
	size_t               repeat ;
	bool                 verbose;
	size_t               ecode  ;
	bool                 list   ;
} cairo_args_s;

/// returns whether the provided arg equals either the brief (e.g. '-e') or full
/// (e.g. '--example') spelling of an option.
_cairo_func bool _cairo_args_is(const char* const arg, const char* const brief,
													   const char* const full) {
	return strcmp(arg, brief) == 0 ? true : (strcmp(arg, full) == 0);
}

/// parses 'arg' as a base10 number, returning false on empty input, non-digits,
/// overflow, or trailing garbage. used for the numeric options.
_cairo_func bool _cairo_args_number(const char* const arg, size_t* const out) {
	if ((NULL == arg) || ('\0' == *arg)) return false;
	for (const char* digit = arg; *digit != '\0'; ++digit) {
		if ((*digit < '0') || (*digit > '9')) return false;
	}

	char* end = NULL; errno = 0;
	const unsigned long value = strtoul(arg, &end, 10);

	if (ERANGE == errno) return false;
	if (*end != '\0')    return false;
#if SIZE_MAX < ULONG_MAX
	if (value > (unsigned long)SIZE_MAX) return false;
#endif

	*out = (size_t)value;
	return true;
}

/// prints the usage/help text for 'program' to the provided 'stream'.
_cairo_func void _cairo_args_usage(FILE* const stream,
								   const char* const program) {
	(void)fprintf(stream,
		"usage: %s [options]\n"
		"    -p, --pattern <glob>  run only tests whose suite.name matches.\n"
		"    -e, --exclude <glob>  skip tests whose suite.name matches.\n"
		"    -r, --repeat <n>      run selected tests n times.\n"
		"    -v, --verbose         enable/disable verbose output.\n"
		"    -x, --ecode <n>       exit code to use when tests fail.\n"
		"    -l, --list            print all collected tests and exit.\n"
		"    -h, --help            print this message and exit.\n"
		"\n"
		"globs accept * and ?, and are matched against \'suite.name\'.\n",
		program
	);
}

/// prints a one-liner error ('reason', offending 'option', optional 'value') to
/// stderr, followed by the usage text.
_cairo_func void _cairo_args_report(const char* const program,
									const char* const reason,
									const char* const option,
									const char* const value) {
	(void)fprintf(stderr, "%s: %s '%s'.", program, reason, option);
	if (value != NULL) (void)fprintf(stderr, ": '%s'", value);
	(void)fprintf(stderr, "\n");
	_cairo_args_usage(stderr, program);
}

/// returns the default options: run everything, exclude nothing, do not repeat.
/// Use this as a starting point to define custom options or pass it straight to
/// 'cairo_tests_run' to run with defaults.
_cairo_func cairo_args_s cairo_args_default(void) {
	return (const cairo_args_s) {
		._status = _cairo_args_status_go_on,
		.pattern = "*"                     ,
		.exclude = NULL                    ,
		.repeat  = 1                       ,
		.verbose = false                   ,
		.ecode   = 1                       ,
		.list    = false                   ,
	};
}

/// parses 'argc'/'argv' into a 'cairo_args_s', starting from the defaults while
/// following the usage definition for available and allowed options. here is an
/// easy way to use it as follows:
/// int main(const int argc, const char* argv[]) {
///     return cairo_tests_run(cairo_args_new(argc, argv));
/// }
_cairo_func cairo_args_s cairo_args_new(const int argc, const char** argv) {
	size_t       index   = 0                   ;
	const size_t count   = (size_t)argc        ;
	const char*  program = argv[index++]       ;
	cairo_args_s args    = cairo_args_default();

	for (; index < count; ++index) {
		const char* const arg  = argv[index];
		const size_t nindex = index + 1;
		const char* const next = nindex < count ? argv[nindex] : NULL;

		if (_cairo_args_is(arg, "-h", "--help")) {
			_cairo_args_usage(stdout, program);
			args._status = _cairo_args_status_exit;
			break;
		}

		if (_cairo_args_is(arg, "-p", "--pattern")) {
			if (NULL == next) {
				_cairo_args_report(program, "missing value for", arg, NULL);
				args._status = _cairo_args_status_error;
				break;
			}

			args.pattern = next;
			++index;
		}
		else if (_cairo_args_is(arg, "-e", "--exclude")) {
			if (NULL == next) {
				_cairo_args_report(program, "missing value for", arg, NULL);
				args._status = _cairo_args_status_error;
				break;
			}

			args.exclude = next;
			++index;
		}
		else if (_cairo_args_is(arg, "-r", "--repeat")) {
			if (NULL == next) {
				_cairo_args_report(program, "missing value for", arg, NULL);
				args._status = _cairo_args_status_error;
				break;
			}

			if (!_cairo_args_number(next, &args.repeat) || (0 == args.repeat)) {
				_cairo_args_report(
					program, "expected a positive number for", arg, next
				);
				args._status = _cairo_args_status_error;
				break;
			}

			++index;
		}
		else if (_cairo_args_is(arg, "-v", "--verbose")) {
			// todo: maybe throw an error on redefinition?
			args.verbose = true;
		}
		else if (_cairo_args_is(arg, "-x", "--ecode")) {
			if (NULL == next) {
				_cairo_args_report(program, "missing value for", arg, NULL);
				args._status = _cairo_args_status_error;
				break;
			}

			if (!_cairo_args_number(next, &args.ecode) || (0 == args.ecode)) {
				_cairo_args_report(
					program, "expected a positive number for", arg, next
				);
				args._status = _cairo_args_status_error;
				break;
			}

			++index;
		}
		else if (_cairo_args_is(arg, "-l", "--list")) {
			// todo: maybe throw an error on redefinition?
			args.list = true;
		}
		else {
			_cairo_args_report(program, "unknown option", arg, NULL);
			args._status = _cairo_args_status_error;
			break;
		}
	}

	return args;
}

/// the full tests run: the array of test references collected from the section,
/// running tallies (passed/failed/skipped), total elapsed time, and args.
typedef struct {
	_cairo_test_s**     data   ;
	size_t              count  ;
	size_t              passed ;
	size_t              failed ;
	size_t              skipped;
	double              elapsed;
	const cairo_args_s* args   ;
} _cairo_tests_s;

/// builds a '_cairo_tests_s' over the tests in the linker section, with all its
/// tallies zeroed and 'args' attached.
_cairo_func _cairo_tests_s _cairo_tests_new(const cairo_args_s* const args) {
	return (const _cairo_tests_s) {
		.data    = _cairo_begin                       ,
		.count   = (size_t)(_cairo_end - _cairo_begin),
		.passed  = 0                                  ,
		.failed  = 0                                  ,
		.skipped = 0                                  ,
		.elapsed = 0.0                                ,
		.args    = args                               ,
	};
}

/// sorts the tests into suite/name order and marks each's 'shall_run' according
/// to the include pattern and (if set) the exclude pattern.
_cairo_func void _cairo_tests_prepare(_cairo_tests_s* const tests) {
	qsort(tests->data, tests->count, sizeof(*tests->data), _cairo_test_compare);

	for (size_t index = 0; index < tests->count; ++index) {
		_cairo_test_s* const test = tests->data[index];
		if (NULL == test) continue;

		test->shall_run = _cairo_test_match(test, tests->args->pattern);
		const char* const exclude = tests->args->exclude;
		if ((exclude != NULL) && _cairo_test_match(test, exclude)) {
			test->shall_run = false;
		}
	}
}

/// prints the name of every selected ('shall_run' is true) test, grouped by the
/// suite and name: the suite on its own line with each of its test names listed
/// beneath it.
/// note: call after '_cairo_tests_prepare', which sorts and applies filters for
/// the tests and sets the 'shall_run' field.
_cairo_func void _cairo_tests_list(_cairo_tests_s* const tests) {
	const char* suite = NULL;
	size_t      count = 0   ;

	for (size_t index = 0; index < tests->count; ++index) {
		_cairo_test_s* const test = tests->data[index];
		if (NULL == test)     continue;
		if (!test->shall_run) continue;

		if ((NULL == suite) || (strcmp(suite, test->suite) != 0)) {
			suite = test->suite;
			(void)printf("%s\n", suite);
		}

		(void)printf("  %zu. %s\n", ++count, test->name);
	}

	(void)printf("\n%zu tests\n", count);
}

/// runs every selected test in order, updating pass/fail/skip counts, and total
/// elapsed time. skips null records and tests not marked to run.
_cairo_func void _cairo_tests_execute(_cairo_tests_s* const tests) {
	tests->passed  = 0  ;
	tests->failed  = 0  ;
	tests->skipped = 0  ;
	tests->elapsed = 0.0;

	for (size_t index = 0; index < tests->count; ++index) {
		_cairo_test_s* const test = tests->data[index];
		if (NULL == test)     continue;
		if (!test->shall_run) continue;

		_cairo_test_run(test);
		// note: unrunned test is an actual state and should be ignored as it is
		// reserved for internal workings of this project.
		tests->passed  += (_cairo_test_status_pass == test->status);
		tests->failed  += (_cairo_test_status_fail == test->status);
		tests->skipped += (_cairo_test_status_skip == test->status);
		tests->elapsed += test->elapsed                            ;
	}
}

/// prints the per-suite progress line, grouping tests by suite and emitting one
/// character per test - '.' pass, 'F' fail, 'S' skip - with the suite's elapsed
/// time in parentheses, see example:
/// demo S..FF..... (123.456ms)
_cairo_func void _cairo_tests_report_suite(const _cairo_tests_s* const tests) {
	if ((tests->passed > 0) || (tests->failed > 0) || (tests->skipped > 0)) {
		const char* suite   = NULL;
		double      elapsed = 0.0 ;

		for (size_t index = 0; index < tests->count; ++index) {
			_cairo_test_s* const test = tests->data[index];
			if (NULL == test)     continue;
			if (!test->shall_run) continue;

			if ((NULL == suite) || (strcmp(suite, test->suite) != 0)) {
				if (suite != NULL) {
					(void)printf(" (%s)\n", _cairo_format_time(elapsed));
				}
				suite   = test->suite;
				elapsed = 0.0        ;
				(void)printf("%s ", suite);
			}

			elapsed += test->elapsed;
			const bool passed = _cairo_test_status_pass == test->status;
			const bool failed = _cairo_test_status_fail == test->status;
			(void)printf("%s", passed ? "." : (failed ? "F" : "S"));
			(void)fflush(stdout);
		}

		(void)printf(" (%s)\n", _cairo_format_time(elapsed));
	}
}

/// prints a detailed failure report (via '_cairo_test_report') for every failed
/// test, when there are any.
_cairo_func void _cairo_tests_report_tests(const _cairo_tests_s* const tests) {
	if (tests->failed > 0) {
		for (size_t index = 0; index < tests->count; ++index) {
			_cairo_test_s* const test = tests->data[index];
			if (NULL == test)     continue;
			if (!test->shall_run) continue;

			if (_cairo_test_status_fail == test->status) {
				_cairo_test_report(test, tests->args->verbose);
			}
		}

		(void)printf("\n");
	}
}

/// prints the whole report: per-suite progress, each failure in detail, and the
/// summary line of passed/failed/skipped counts with total time.
_cairo_func void _cairo_tests_report(const _cairo_tests_s* const tests) {
	_cairo_tests_report_suite(tests);
	_cairo_tests_report_tests(tests);
	(void)printf("%zu passed, %zu failed, %zu skipped / %zu (%s)\n",
		tests->passed, tests->failed, tests->skipped,
		tests->passed + tests->failed + tests->skipped,
		_cairo_format_time(tests->elapsed)
	);
}

/// runs the tests according to 'args' and returns a process-ready exit code. if
/// parsing did not end in 'go_on' (e.g. --help or an usage error), returns that
/// status as the code without running. otherwise it prepares, runs, and reports
/// the selected tests 'args.repeat' times, returning 1 if any run had a failure
/// and 0 if all passed. with no tests at all, returns 0.
_cairo_func int cairo_tests_run(const cairo_args_s args) {
	if (args._status != _cairo_args_status_go_on) return (int)args._status;
	_cairo_tests_s tests = _cairo_tests_new(&args);
	if (0 == tests.count) return 0;

	if (args.list) {
		_cairo_tests_prepare(&tests);
		_cairo_tests_list(&tests);
		return 0;
	}

	bool failed = false;
	for (size_t _ = 0; _ < args.repeat; ++_) {
		_cairo_tests_prepare(&tests);
		_cairo_tests_execute(&tests);
		_cairo_tests_report(&tests);
		if (!failed && (tests.failed > 0)) failed = true;
	}
	return failed ? (int)args.ecode : 0;
}

/// helper entry point that runs all tests with default options and no cli args.
/// equivalent to 'cairo_tests_run(cairo_args_default())'.
_cairo_func int cairo_tests_run_default(void) {
	return cairo_tests_run(cairo_args_default());
}

/// note: this ref to a null test is needed to have at least one test in a cairo
/// tests section to not get linker errors.
_cairo_test_ref(_cairo_test_dummy_ref, NULL);

#ifdef cairo_enable_prefixless
#	define test_new          cairo_test_new
#	define test_skip         cairo_test_skip
#	define assert_true       cairo_assert_true
#	define assert_false      cairo_assert_false
#	define assert_eq         cairo_assert_eq
#	define assert_neq        cairo_assert_neq
#	define assert_gt         cairo_assert_gt
#	define assert_ge         cairo_assert_ge
#	define assert_lt         cairo_assert_lt
#	define assert_le         cairo_assert_le
#	define assert_nr         cairo_assert_nr
#	define assert_nreq       cairo_assert_nreq
#	define args_s            cairo_args_s
#	define args_default      cairo_args_default
#	define args_new          cairo_args_new
#	define tests_run         cairo_tests_run
#	define tests_run_default cairo_tests_run_default
#endif

#endif

/// 
/// revision history:
///     vX.X.X (xxxx-xx-xx) add verbosity mechanism and cli --verbose flag.
///     v1.0.0 (2026-07-28) first release.
/// 
/// Copyright (c) 2026 yorissu
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documents, and documentation files (referred
/// to as the "software" from here on), to deal in and with the software without
/// restriction, including, without limitation, the rights to use, copy, modify,
/// merge, publish, distribute, sublicense, and/or sell copies of this software,
/// and to permit persons to whom the software is furnished to do so, subject to
/// the following conditions:
/// The above copyright notice and this permission notice shall be included with
/// any and in all copies or substantial portions of the software.
/// The software is provided "as is", without warranty of any kind, expressed or
/// implied, including but not limited to the warranties of the merchantability,
/// fitness for a particular purpose and noninfringement. In no event, shall the
/// authors or copyright holders be liable for any claim, damages, incidents, or
/// other liability, whether in an action of contract, tort or otherwise arising
/// from, out of or in connection with the software or the use or other dealings
/// in the software.
/// 
