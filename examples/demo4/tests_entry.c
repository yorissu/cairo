
/// ===========================================================================
///  ██████╗ █████╗ ██╗██████╗  ██████╗     ██╗    demo4
/// ██╔════╝██╔══██╗██║██╔══██╗██╔═══██╗   ██╔╝    the guided tour
/// ██║     ███████║██║██████╔╝██║   ██║  ██╔╝      a demo, a manual, and a
/// ██║     ██╔══██║██║██╔══██╗██║   ██║ ██╔╝       functional test in one file
/// ╚██████╗██║  ██║██║██║  ██║╚██████╔╝██╔╝
///  ╚═════╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝ ╚═════╝ ╚═╝
/// ---------------------------------------------------------------------------
/// WHAT THIS FILE IS
///
/// This example is meant to be read top-to-bottom. It demonstrates every
/// public feature of cairo.h three ways at once:
///
///   * as a DEMO      - run it and watch each feature in action;
///   * as a MANUAL    - the comments explain what each feature is for;
///   * as a TEST      - the 'demo4' and 'demo4_short' suites are genuine
///                      passing tests, so a clean run also proves the
///                      framework itself still works.
///
/// HOW TO RUN
///
///   Guided tour (no arguments):
///       ./demo4_tests
///
///   Interactive mode (any arguments): the arguments are forwarded straight
///   to cairo, so you can experiment with the real CLI, e.g.
///       ./demo4_tests --list
///       ./demo4_tests --pattern 'demo4.assert*' --verbose
///       ./demo4_tests --help
///
/// LAYOUT OF THIS EXAMPLE
///
///   tests_entry.c   (this file) - the tour driver + the real 'demo4' suite
///                                 + an intentionally-failing 'broken' suite
///                                 used only to show failure reporting.
///   prefixless.c                - a second .c file proving cross-file test
///                                 registration and the prefixless aliases.
///
/// A NOTE ON THE 'broken' SUITE
///
/// Several tests below fail ON PURPOSE. They are collected under the 'broken'
/// suite and are EXCLUDED from the functional run, then shown separately so
/// you can see exactly what a failure report looks like. The process exit code
/// reflects only the clean run, so CI stays green.
/// ===========================================================================

#include "cairo.h"

/// ===========================================================================
/// PART 1 - THE REAL SUITE: 'demo4'
///
/// These tests all PASS. Together they exercise the entire public assertion
/// surface, so a green run of this suite is a functional test of cairo itself.
/// ===========================================================================

/// cairo_test_new(suite, name) { body } defines and auto-registers a test.
/// Inside the body a hidden handle named '_test' carries pass/fail state; the
/// assert macros use it for you, so you never touch it directly.

cairo_test_new(demo4, assert_true) {
	/// cairo_assert_true(x) - passes when x is truthy.
	cairo_assert_true(1);
	cairo_assert_true(true);
	cairo_assert_true(!0);
	cairo_assert_true("non-null pointer");
}

cairo_test_new(demo4, assert_false) {
	/// cairo_assert_false(x) - passes when x is falsy.
	cairo_assert_false(0);
	cairo_assert_false(false);
	cairo_assert_false(!1);
	cairo_assert_false(NULL);
}

cairo_test_new(demo4, assert_eq) {
	/// cairo_assert_eq(a, b) - passes when a == b. Works on any type the
	/// comparison operator accepts: integers, characters, pointers, ...
	cairo_assert_eq(2 + 2, 4);
	cairo_assert_eq('a', 'a');
	cairo_assert_eq(NULL, NULL);
	/// For strings, compare with strcmp - '==' would compare addresses.
	cairo_assert_eq(strcmp("cairo", "cairo"), 0);
}

cairo_test_new(demo4, assert_neq) {
	/// cairo_assert_neq(a, b) - passes when a != b.
	cairo_assert_neq(2 + 2, 5);
	cairo_assert_neq('a', 'b');
	cairo_assert_neq(strcmp("cairo", "cairos"), 0);
}

cairo_test_new(demo4, assert_gt) {
	/// cairo_assert_gt(a, b) - passes when a > b.
	cairo_assert_gt(2, 1);
	cairo_assert_gt('b', 'a');
	cairo_assert_gt(1.5, 0.5); /// '>' on reals is fine, see the nr note below.
}

cairo_test_new(demo4, assert_ge) {
	/// cairo_assert_ge(a, b) - passes when a >= b.
	cairo_assert_ge(2, 1);
	cairo_assert_ge(2, 2);
}

cairo_test_new(demo4, assert_lt) {
	/// cairo_assert_lt(a, b) - passes when a < b.
	cairo_assert_lt(1, 2);
	cairo_assert_lt('a', 'b');
	cairo_assert_lt(0.5, 1.5);
}

cairo_test_new(demo4, assert_le) {
	/// cairo_assert_le(a, b) - passes when a <= b.
	cairo_assert_le(1, 2);
	cairo_assert_le(2, 2);
}

cairo_test_new(demo4, assert_nr) {
	/// cairo_assert_nr(a, b, eps) - passes when |a - b| <= eps.
	/// USE THIS (or _nreq) FOR FLOATS. cairo_assert_eq on floating-point
	/// compares bit-for-bit and will surprise you, so prefer a tolerance.
	cairo_assert_nr(0.1 + 0.2, 0.3, 1e-9);
	cairo_assert_nr(1.0f, 1.0f, 1e-6);
}

cairo_test_new(demo4, assert_nreq) {
	/// cairo_assert_nreq(a, b) - like _nr with a built-in epsilon of 1e-6.
	cairo_assert_nreq(0.1 + 0.2, 0.3);
}

cairo_test_new(demo4, skipping) {
	/// cairo_test_skip() marks the test skipped and returns immediately.
	/// Anything after it does not run. Skips show up as 'S' in the report.
	cairo_test_skip();
	cairo_assert_true(false); /// never reached.
}

cairo_test_new(demo4, empty_is_a_pass) {
	/// A test with no assertions simply passes. The hidden '_test' handle is
	/// marked [[maybe_unused]] by cairo, so empty bodies compile cleanly even
	/// under -Wunused-parameter.
}

cairo_test_new(demo4, multiline_expression) {
	/// Assertion arguments may span multiple lines. cairo captures the whole
	/// parenthesised expression - useful to know for the verbose report later.
	cairo_assert_eq(
		(
			5 + 9 * 7
		),
		(
			60 + 8 + 0
		)
	);
}

/// ===========================================================================
/// PART 2 - THE 'xfail' SUITE (fails on purpose)
///
/// Every test here fails deliberately so the tour can show off two things:
///
///   * FAILURE REPORTING - the file:line, the stringified expression, and (in
///     verbose mode) the reprinted source with a '^~~~' caret.
///   * VALUE FORMATTING - when an assertion fails, cairo prints the actual
///     left/right values. It picks the right formatter for the operand type at
///     compile time via _Generic, with no help from you.
///
/// KEY DETAIL: a failing assertion STOPS ITS TEST immediately. So to show the
/// formatter for every type, each type gets its OWN one-line test below (the
/// 'fmt_*' gallery). The left/right values only appear in VERBOSE mode.
///
/// The whole suite is named 'xfail' so the functional run can drop it with a
/// single 'xfail.*' exclude. Browse the gallery yourself with:
///     ./demo4_tests -p 'xfail.fmt_*' -v
/// ===========================================================================

/// --- the value-formatter gallery: one failing assert per supported type ----

cairo_test_new(xfail, fmt_char)    { cairo_assert_eq((char)'a', (char)'z'); }                 /// glyph:  a / z
cairo_test_new(xfail, fmt_schar)   { cairo_assert_eq((signed char)-3, (signed char)4); }      /// -3 / 4
cairo_test_new(xfail, fmt_uchar)   { cairo_assert_eq((unsigned char)200, (unsigned char)7); } /// 200 / 7
cairo_test_new(xfail, fmt_bool)    { const bool yes = true, no = false; cairo_assert_eq(yes, no); } /// true / false
cairo_test_new(xfail, fmt_short)   { cairo_assert_eq((short)1, (short)2); }
cairo_test_new(xfail, fmt_ushort)  { cairo_assert_eq((unsigned short)1, (unsigned short)2); }
cairo_test_new(xfail, fmt_int)     { cairo_assert_eq(2 + 2, 5); }
cairo_test_new(xfail, fmt_uint)    { cairo_assert_eq(1u, 2u); }
cairo_test_new(xfail, fmt_long)    { cairo_assert_eq(1L, 2L); }
cairo_test_new(xfail, fmt_ulong)   { cairo_assert_eq(1UL, 2UL); }
cairo_test_new(xfail, fmt_llong)   { cairo_assert_eq(1LL, 2LL); }
cairo_test_new(xfail, fmt_ullong)  { cairo_assert_eq(1ULL, 2ULL); }
cairo_test_new(xfail, fmt_size)    { cairo_assert_eq((size_t)1, (size_t)2); }                 /// size_t -> unsigned

/// reals use the tolerance comparison, so the reported operator is '~='.
/// (cairo_assert_eq on floats would trip -Wfloat-equal - always use _nr/_nreq.)
cairo_test_new(xfail, fmt_float)   { cairo_assert_nr(1.5f, 2.5f, 1e-6); }   /// %g  -> 1.5 / 2.5
cairo_test_new(xfail, fmt_double)  { cairo_assert_nr(1.25, 2.5, 1e-6); }    /// %g
cairo_test_new(xfail, fmt_ldouble) { cairo_assert_nr(1.5L, 2.5L, 1e-6); }   /// %Lg

cairo_test_new(xfail, fmt_string) {
	/// char* / const char* print quoted. (Assign to variables first so we are
	/// not comparing string-literal addresses, which -Waddress dislikes.)
	const char* const lhs = "expected";
	const char* const rhs = "actual";
	cairo_assert_eq(lhs, rhs);            /// "expected" / "actual"
}

cairo_test_new(xfail, fmt_null) {
	const char* const value = NULL;
	cairo_assert_neq(value, value);      /// a NULL string prints as: null
}

cairo_test_new(xfail, fmt_pointer) {
	/// Any other pointer type falls through to the raw-address formatter.
	int object = 0;
	void* const here    = &object;
	void* const nowhere = NULL;
	cairo_assert_eq(here, nowhere);      /// 0x... / null
}

/// --- reporting mechanics ----------------------------------------------------

cairo_test_new(xfail, unary) {
	/// Unary assertions carry no left/right values - the report shows only the
	/// stringified expression, with no '= left/right' lines.
	cairo_assert_true(1 == 2);
}

cairo_test_new(xfail, multiline) {
	/// In verbose mode the caret underlines the failing expression even when it
	/// spans several lines.
	cairo_assert_eq(
		(1 + 1),
		(3 + 4)
	);
}

/// ===========================================================================
/// PART 3 - THE TOUR DRIVER
/// ===========================================================================

/// small helper to print a titled banner between sections.
static void section(const char* const title) {
	(void)printf(
		"\n"
		"================================================================\n"
		"  %s\n"
		"================================================================\n",
		title
	);
}

/// builds an argv[]/argc pair from string literals for the simulated CLI runs
/// below. argv[0] stands in for the program name (used in --help / errors).
/// the locals are underscored so they never shadow main's own argc/argv.
#define with_argv(...)                                                         \
	const char* _argv[] = { __VA_ARGS__ };                                     \
	const int   _argc   = (int)(sizeof(_argv) / sizeof(*_argv))

int main(const int argc, const char* argv[]) {
	/// ------------------------------------------------------------------
	/// INTERACTIVE MODE: if the reader passed any real arguments, hand them
	/// straight to cairo and behave like an ordinary test runner. This is
	/// also the shortest possible cairo entry point:
	///     return cairo_tests_run(cairo_args_new(argc, argv));
	/// ------------------------------------------------------------------
	if (argc > 1) {
		return cairo_tests_run(cairo_args_new(argc, argv));
	}

	(void)printf(
		"cairo demo4 - guided tour\n"
		"run me with any argument (e.g. --help, --list, -p '<glob>') to use\n"
		"the real command line instead of this tour.\n"
	);

	/// ------------------------------------------------------------------
	/// 1. THE FUNCTIONAL RUN
	///
	/// cairo_args_default() gives you a ready-to-use options struct; tweak
	/// its fields directly - there is no hidden global config. Here we run
	/// everything EXCEPT the intentionally-broken suite. Its return value is
	/// the process exit code we ultimately propagate.
	/// ------------------------------------------------------------------
	section("1. the real suite (all passing) - this is the functional test");
	int result;
	{
		cairo_args_s args = cairo_args_default(); /// pattern "*", no exclude.
		args.exclude = "xfail.*";                 /// keep the demo failures out.
		result = cairo_tests_run(args);
		(void)printf("exit code: %d\n", result);
	}

	/// ------------------------------------------------------------------
	/// 2. ASSERTION CHEAT-SHEET (printed reference, no run)
	/// ------------------------------------------------------------------
	section("2. assertion reference");
	(void)printf(
		"  cairo_assert_true(x)        x is truthy\n"
		"  cairo_assert_false(x)       x is falsy\n"
		"  cairo_assert_eq(a, b)       a == b\n"
		"  cairo_assert_neq(a, b)      a != b\n"
		"  cairo_assert_gt(a, b)       a >  b\n"
		"  cairo_assert_ge(a, b)       a >= b\n"
		"  cairo_assert_lt(a, b)       a <  b\n"
		"  cairo_assert_le(a, b)       a <= b\n"
		"  cairo_assert_nr(a, b, eps)  |a - b| <= eps      (use for floats)\n"
		"  cairo_assert_nreq(a, b)     |a - b| <= 1e-6     (use for floats)\n"
		"  cairo_test_skip()           mark test skipped, stop running it\n"
		"\n"
		"  a failing assert stops its test but never the whole run.\n"
	);

	/// ------------------------------------------------------------------
	/// 3. FILTERING WITH --pattern (globs: '*' any run, '?' one char)
	///
	/// The pattern is matched against each test's 'suite.name'.
	/// ------------------------------------------------------------------
	section("3. include filter: pattern = 'demo4.assert_*'");
	{
		cairo_args_s args = cairo_args_default();
		args.pattern = "demo4.assert_*";
		(void)cairo_tests_run(args);
	}

	section("3b. include filter with '?': pattern = 'demo4.assert_?e*'");
	{
		cairo_args_s args = cairo_args_default();
		args.pattern = "demo4.assert_?e*"; /// matches assert_eq / assert_neq / assert_le ...
		(void)cairo_tests_run(args);
	}

	/// ------------------------------------------------------------------
	/// 4. EXCLUDING WITH --exclude
	/// ------------------------------------------------------------------
	section("4. exclude filter: everything except 'demo4.assert_*'");
	{
		cairo_args_s args = cairo_args_default();
		args.exclude = "demo4.assert_*";
		args.pattern = "demo4.*"; /// stay inside the demo4 suite for a tidy view.
		(void)cairo_tests_run(args);
	}

	/// ------------------------------------------------------------------
	/// 5. PATTERN + EXCLUDE TOGETHER (exclude wins on overlap)
	/// ------------------------------------------------------------------
	section("5. pattern 'demo4.assert_*' minus exclude 'demo4.assert_n*'");
	{
		cairo_args_s args = cairo_args_default();
		args.pattern = "demo4.assert_*";
		args.exclude = "demo4.assert_n*"; /// drops assert_nr / assert_neq / assert_nreq.
		(void)cairo_tests_run(args);
	}

	/// ------------------------------------------------------------------
	/// 6. REPEATING A RUN (--repeat n) - runs the whole selection n times
	/// ------------------------------------------------------------------
	section("6. repeat the selection 3 times");
	{
		cairo_args_s args = cairo_args_default();
		args.pattern = "demo4.assert_eq";
		args.repeat  = 3;
		(void)cairo_tests_run(args);
	}

	/// ------------------------------------------------------------------
	/// 7. LISTING TESTS (--list) - prints the selection and exits, runs none
	/// ------------------------------------------------------------------
	section("7. list the selected tests without running them");
	{
		cairo_args_s args = cairo_args_default();
		args.list = true; /// respects pattern/exclude too.
		(void)cairo_tests_run(args);
	}

	/// ------------------------------------------------------------------
	/// 8. VERBOSE VALUE-FORMATTER GALLERY (--verbose)
	///
	/// The 'xfail' suite fails on purpose. In verbose mode cairo reprints the
	/// source line, underlines the failing expression with '^~~~', and prints
	/// the formatted left/right values. The 'fmt_*' tests below walk every
	/// supported operand type so you can see exactly how each one renders. The
	/// exit code is deliberately ignored so it never affects the tour result.
	/// ------------------------------------------------------------------
	section("8. verbose per-type value formatting (failures are INTENTIONAL)");
	{
		cairo_args_s args = cairo_args_default();
		args.pattern = "xfail.fmt_*";
		args.verbose = true;
		(void)cairo_tests_run(args);
	}

	/// ------------------------------------------------------------------
	/// 9. VERBOSE REPORTING MECHANICS - unary (no values) and multi-line caret
	/// ------------------------------------------------------------------
	section("9. verbose reporting mechanics (unary + multi-line caret)");
	{
		cairo_args_s args = cairo_args_default();
		args.pattern = "xfail.*";
		args.exclude = "xfail.fmt_*"; /// leaves 'unary' and 'multiline'.
		args.verbose = true;
		(void)cairo_tests_run(args);
	}

	/// ------------------------------------------------------------------
	/// 10. COMPACT FAILURE REPORTS (verbose off) - one line per failure
	/// ------------------------------------------------------------------
	section("10. the same failures, compact (verbose off)");
	{
		cairo_args_s args = cairo_args_default();
		args.pattern = "xfail.*";
		args.verbose = false;
		(void)cairo_tests_run(args);
	}

	/// ------------------------------------------------------------------
	/// 11. CUSTOM EXIT CODE (--ecode n) - what a failing run returns
	/// ------------------------------------------------------------------
	section("11. custom exit code on failure (ecode = 42)");
	{
		cairo_args_s args = cairo_args_default();
		args.pattern = "xfail.fmt_int";
		args.ecode   = 42;
		const int code = cairo_tests_run(args);
		(void)printf("a failing run returned exit code: %d\n", code);
	}

	/// ------------------------------------------------------------------
	/// 12. PARSING A COMMAND LINE (cairo_args_new)
	///
	/// cairo_args_new(argc, argv) turns real argv into a cairo_args_s and
	/// reports how parsing ended via its (private) status. cairo_tests_run
	/// honours that status: on --help or a usage error it prints and returns
	/// WITHOUT running any tests. Below we feed it synthetic argument vectors.
	/// ------------------------------------------------------------------
	section("12a. cairo_args_new parses --help, then cairo_tests_run exits");
	{
		with_argv("demo4_tests", "--help");
		const int code = cairo_tests_run(cairo_args_new(_argc, _argv));
		(void)printf("(--help ran no tests; returned %d)\n", code);
	}

	section("12b. a usage error is reported and no tests run");
	{
		with_argv("demo4_tests", "--repeat", "not-a-number");
		const int code = cairo_tests_run(cairo_args_new(_argc, _argv));
		(void)printf("(bad argument; returned %d)\n", code);
	}

	section("12c. an unknown option is rejected");
	{
		with_argv("demo4_tests", "--frobnicate");
		const int code = cairo_tests_run(cairo_args_new(_argc, _argv));
		(void)printf("(unknown option; returned %d)\n", code);
	}

	/// ------------------------------------------------------------------
	/// 13. CROSS-FILE + PREFIXLESS (see prefixless.c)
	///
	/// The 'demo4_short' suite lives in prefixless.c, a separate .c file with
	/// no main(). It uses the prefixless aliases (test_new, assert_eq, ...)
	/// enabled by '#define cairo_enable_prefixless'. It appears here purely
	/// because defining a test registers it into cairo's linker section - no
	/// include, no registration call, no shared header needed.
	/// ------------------------------------------------------------------
	section("13. tests auto-collected from prefixless.c ('demo4_short' suite)");
	{
		cairo_args_s args = cairo_args_default();
		args.pattern = "demo4_short.*";
		(void)cairo_tests_run(args);
	}

	/// ------------------------------------------------------------------
	/// 14. COMPILE-TIME KNOBS (recap - these are set before #include)
	/// ------------------------------------------------------------------
	section("14. compile-time options");
	(void)printf(
		"  #define cairo_enable_prefixless   drop the 'cairo_' prefix on macros\n"
		"                                    (demonstrated in prefixless.c)\n"
		"  #define cairo_tab_width <n>       tab width for verbose source lines\n"
		"                                    (default 4; set in this file)\n"
		"\n"
		"  CLI flags (also settable as cairo_args_s fields):\n"
		"    -p/--pattern  -e/--exclude  -r/--repeat  -v/--verbose\n"
		"    -x/--ecode    -l/--list     -h/--help\n"
	);

	section("end of tour");
	(void)printf(
		"the exit code below is from step 1 (the clean functional run).\n"
		"re-run with arguments to drive cairo yourself, e.g.:\n"
		"    ./demo4_tests -p 'xfail.*' -v\n"
	);

	/// Propagate ONLY the functional run's result, so intentional demo
	/// failures never turn the build red.
	return result;
}
