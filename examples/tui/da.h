
#ifndef __da_h__
#define __da_h__

#include <assert.h>
#include <stdlib.h>

#define da_push(_da, _value) do {                                              \
		if ((_da)->count >= (_da)->capacity) {                                 \
			(_da)->capacity = (                                                \
				((_da)->capacity <= 0) ? 4 : ((_da)->capacity * 2)             \
			);                                                                 \
			                                                                   \
			(_da)->data = realloc((_da)->data,                                 \
				(_da)->capacity * sizeof(*(_da)->data)                         \
			);                                                                 \
			                                                                   \
			assert((_da)->data != NULL);                                       \
		}                                                                      \
		                                                                       \
		(_da)->data[(_da)->count++] = (_value);                                \
	} while (0)

#define da_free(_da) do {                                                      \
		free((_da)->data);                                                     \
		(_da)->data     = NULL;                                                \
		(_da)->capacity = 0   ;                                                \
		(_da)->count    = 0   ;                                                \
	} while (0)

#endif
