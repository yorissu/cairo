
#ifndef __tui_h__
#define __tui_h__

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct {
	float x     ;
	float y     ;
	float width ;
	float height;
} tui_rect_s;

typedef struct {
	const char* name;
	tui_rect_s  rect;
} tui_item_s;

typedef struct tui_node_s tui_node_s;

typedef struct {
	tui_node_s** data    ;
	size_t       capacity;
	size_t       count   ;
} tui_nodes_s;

struct tui_node_s {
	tui_item_s  item    ;
	tui_nodes_s nodes   ;
	tui_node_s* parent  ;
	bool        rendered;
};

tui_node_s* tui_create(const tui_item_s item, tui_node_s* const parent);

void tui_render(FILE* const stream, tui_node_s* const root);

void tui_free(tui_node_s** const node);

#endif
