
#ifndef __stats_h__
#define __stats_h__

#include <stdbool.h>
#include <stddef.h>

double stats_mean(const double* values, size_t count);

double stats_min(const double* values, size_t count);

double stats_max(const double* values, size_t count);

size_t stats_count_above(const double* values, size_t count, double threshold);

const char* stats_describe(double value);

bool stats_is_sorted(const double* values, size_t count);

#endif
