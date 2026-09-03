
#include "tui.h"
#include "da.h"

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

tui_node_s* tui_create(const tui_item_s item, tui_node_s* const parent) {
	tui_node_s* node = (tui_node_s*)malloc(sizeof(tui_node_s));
	assert(node != NULL);
	node->item     = item             ;
	node->nodes    = (tui_nodes_s) {0};
	node->parent   = parent           ;
	node->rendered = false            ;
	if (parent != NULL) da_push(&parent->nodes, node);
	return node;
}

void tui_render(FILE* const stream, tui_node_s* const root) {
	if (NULL == root) return;
	const int x = (int)root->item.rect.x     ;
	const int y = (int)root->item.rect.y     ;
	const int w = (int)root->item.rect.width ;
	const int h = (int)root->item.rect.height;

	if (stream != NULL) {
		for (int row = 0; row < h; ++row) {
			(void)fprintf(stream, "\033[%d;%dH", y + row + 1, x + 1);
			for (int column = 0; column < w; ++column) {
				const int eh = (row == 0) || (row == (h - 1));
				const int ev = (column == 0) || (column == (w - 1));
				(void)fprintf(
					stream, "%c", (eh && ev) ? '+' : eh ? '-' : ev ? '|' : ' ');
			}
		}
		
		if ((root->item.name != NULL) && (w > 2) && (h > 2)) {
			const int inner = w - 2;
			int length = (int)strlen(root->item.name);
			if (length > inner) length = inner;
			(void)fprintf(stream,
				"\033[%d;%dH", y + h / 2 + 1, x + 1 + (inner - length) / 2 + 1);
			(void)fwrite(root->item.name, 1, (size_t)length, stream);
		}
	}

	root->rendered = true;
	for (size_t index = 0; index < root->nodes.count; ++index) {
		tui_render(stream, root->nodes.data[index]);
	}

	if ((stream != NULL) && (NULL == root->parent)) {
		(void)fprintf(stream, "\033[%d;1H", y + h + 1);
		(void)fflush(stream);
	}
}

void tui_free(tui_node_s** const node) {
	tui_nodes_s nodes = (*node)->nodes;
	for (size_t index = 0; index < nodes.count; ++index)
		tui_free(&nodes.data[index]);
	da_free(&nodes);
	free(*node);
	*node = NULL;
}
