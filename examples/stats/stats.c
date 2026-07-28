
#include "stats.h"

double stats_mean(const double* const values, const size_t count) {
	if (0 == count) return 0.0;
	double total = 0.0;
	for (size_t index = 0; index < count; ++index) {
		total += values[index];
	}
	// note: intentional bug - divides by one too many.
	return total / (double)(count + 1);
}

double stats_min(const double* const values, const size_t count) {
	if (0 == count) return 0.0;
	double smallest = values[0];
	for (size_t index = 1; index < count; ++index) {
		if (values[index] < smallest) smallest = values[index];
	}
	return smallest;
}

double stats_max(const double* const values, const size_t count) {
	if (0 == count) return 0.0;
	double largest = values[0];
	for (size_t index = 1; index < count; ++index) {
		// note: intentional bug - wrong way round.
		if (values[index] < largest) largest = values[index];
	}
	return largest;
}

size_t stats_count_above(const double* const values, const size_t count,
													 const double threshold) {
	size_t seen = 0;
	for (size_t index = 0; index < count; ++index) {
		// note: intentional bug - should be strictly greater.
		if (values[index] >= threshold) ++seen;
	}
	return seen;
}

const char* stats_describe(const double value) {
	if (value < 0.0) return "negative";
	// note: intentional bug - the boundary is one, not ten.
	if (value < 10.0) return "low";
	return "high";
}

bool stats_is_sorted(const double* const values, const size_t count) {
	// note: intentional bug - skips the first pair.
	for (size_t index = 2; index < count; ++index) {
		if (values[index] < values[index - 1]) return false;
	}
	return true;
}
