
#include "da.h"

#include "cairo.h"

#include <stddef.h>
 
typedef struct {
	int*   data    ;
	size_t capacity;
	size_t count   ;
} da_int_s;
 
typedef struct {
	int    id   ;
	float  value;
} da_item_s;
 
typedef struct {
	da_item_s* data    ;
	size_t     capacity;
	size_t     count   ;
} da_items_s;
 
typedef struct {
	int**  data    ;
	size_t capacity;
	size_t count   ;
} da_ptrs_s;
 
cairo_test_new(da, da_push_empty_test) {
	da_int_s da = {0};

	cairo_expect_eq(da.data    , NULL     );
	cairo_expect_eq(da.capacity, (size_t)0);
	cairo_expect_eq(da.count   , (size_t)0);
 
	da_push(&da, 42);
	cairo_expect_neq(da.data    , NULL     );
	cairo_expect_eq( da.capacity, (size_t)4);
	cairo_expect_eq( da.count   , (size_t)1);
	cairo_expect_eq( da.data[0] , 42       );
 
	cairo_test_defered da_free(&da);
	cairo_assert_eq(da.data    , NULL     );
	cairo_assert_eq(da.capacity, (size_t)0);
	cairo_assert_eq(da.count   , (size_t)0);
}
 
cairo_test_new(da, da_push_growth_test) {
	da_int_s da = {0};
 
	da_push(&da, 10);
	cairo_expect_eq(da.capacity, (size_t)4);
	cairo_expect_eq(da.count   , (size_t)1);
 
	da_push(&da, 20);
	da_push(&da, 30);
	da_push(&da, 40);
	cairo_expect_eq(da.capacity, (size_t)4);
	cairo_expect_eq(da.count   , (size_t)4);
 
	da_push(&da, 50);
	cairo_expect_eq(da.capacity, (size_t)8);
	cairo_expect_eq(da.count   , (size_t)5);
 
	cairo_test_defered da_free(&da);
	cairo_assert_eq(da.data    , NULL     );
	cairo_assert_eq(da.capacity, (size_t)0);
	cairo_assert_eq(da.count   , (size_t)0);
}
 
cairo_test_new(da, da_push_values_test) {
	da_int_s da = {0};
 
	for (int index = 0; index < 100; ++index) {
		da_push(&da, index * index);
	}
 
	cairo_expect_eq(da.count   , (size_t)100);
	cairo_expect_eq(da.capacity, (size_t)128);
	cairo_expect_ge(da.capacity, da.count   );
 
	for (int index = 0; index < 100; ++index) {
		cairo_expect_eq(da.data[index], index * index);
	}
 
	cairo_test_defered da_free(&da);
	cairo_assert_eq(da.data    , NULL     );
	cairo_assert_eq(da.capacity, (size_t)0);
	cairo_assert_eq(da.count   , (size_t)0);
}
 
cairo_test_new(da, da_push_struct_test) {
	da_items_s da = {0};
 
	da_push(&da, ((da_item_s) { .id = 1, .value = 1.5f, }));
	da_push(&da, ((da_item_s) { .id = 2, .value = 2.5f, }));
 
	cairo_expect_eq(  da.count        , (size_t)2);
	cairo_expect_eq(  da.data[0].id   , 1        );
	cairo_expect_nreq(da.data[0].value, 1.5f     );
	cairo_expect_eq(  da.data[1].id   , 2        );
	cairo_expect_nreq(da.data[1].value, 2.5f     );
 
	cairo_test_defered da_free(&da);
	cairo_assert_eq(da.data    , NULL     );
	cairo_assert_eq(da.capacity, (size_t)0);
	cairo_assert_eq(da.count   , (size_t)0);
}
 
cairo_test_new(da, da_push_pointer_test) {
	int a = 1, b = 2, c = 3;
	da_ptrs_s da = {0};
 
	da_push(&da, &a);
	da_push(&da, &b);
	da_push(&da, &c);
 
	cairo_expect_eq(da.count   , (size_t)3);
	cairo_expect_eq(da.data[0] , &a       );
	cairo_expect_eq(da.data[1] , &b       );
	cairo_expect_eq(da.data[2] , &c       );
	cairo_expect_eq(*da.data[0], 1        );
	cairo_expect_eq(*da.data[2], 3        );
 
	cairo_test_defered da_free(&da);
	cairo_assert_eq(da.data    , NULL     );
	cairo_assert_eq(da.capacity, (size_t)0);
	cairo_assert_eq(da.count   , (size_t)0);
}
 
cairo_test_new(da, da_free_reuse_test) {
	da_int_s da = {0};
 
	da_push(&da, 42);
	cairo_expect_eq(da.count, (size_t)1);
 
	da_free(&da);
	cairo_expect_eq(da.data    , NULL     );
	cairo_expect_eq(da.capacity, (size_t)0);
	cairo_expect_eq(da.count   , (size_t)0);
 
	da_push(&da, 7);
	cairo_expect_eq(da.count  , (size_t)1);
	cairo_expect_eq(da.data[0], 7        );
 
	cairo_test_defered da_free(&da);
	cairo_assert_eq(da.data    , NULL     );
	cairo_assert_eq(da.capacity, (size_t)0);
	cairo_assert_eq(da.count   , (size_t)0);
}
