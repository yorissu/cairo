
#include "tui.h"

#include "cairo.h"

#include <unistd.h>

#define def_item(_name, _x, _y, _w, _h) (tui_item_s) {                         \
		.name = _name,                                                         \
		.rect = (tui_rect_s) {                                                 \
			.x      = _x,                                                      \
			.y      = _y,                                                      \
			.width  = _w,                                                      \
			.height = _h,                                                      \
		},                                                                     \
	}                                                                          \

cairo_test_new(tui, create_root_test) {
	tui_node_s* root = tui_create(def_item("root", 0, 0, 100, 100), NULL);

	cairo_expect_streq(root->item.name       , "root"   );
	cairo_expect_nreq( root->item.rect.x     , 0        );
	cairo_expect_nreq( root->item.rect.y     , 0        );
	cairo_expect_nreq( root->item.rect.width , 100      );
	cairo_expect_nreq( root->item.rect.height, 100      );
	cairo_expect_eq(   root->nodes.count     , (size_t)0);
	cairo_expect_eq(   root->parent          , NULL     );
	cairo_expect_false(root->rendered                   );

	cairo_test_defered tui_free(&root);
	cairo_assert_eq(root, NULL);
}

cairo_test_new(tui, render_root_test) {
	tui_node_s* root = tui_create(def_item("root", 0, 0, 100, 100), NULL);

	cairo_expect_streq(root->item.name       , "root"   );
	cairo_expect_nreq( root->item.rect.x     , 0        );
	cairo_expect_nreq( root->item.rect.y     , 0        );
	cairo_expect_nreq( root->item.rect.width , 100      );
	cairo_expect_nreq( root->item.rect.height, 100      );
	cairo_expect_eq(   root->nodes.count     , (size_t)0);
	cairo_expect_eq(   root->parent          , NULL     );
	cairo_expect_false(root->rendered                   );

	tui_render(NULL, root);
	cairo_expect_true(root->rendered);

	cairo_test_defered tui_free(&root);
	cairo_assert_eq(root, NULL);
}

cairo_test_new(tui, create_and_render_tree_test) {
	////////////////////////////////////////////////////////////////////////////
	//                                                                        //
	//                                   a                                    //
	//                                  / \                                   //
	//                                 b   c                                  //
	//                                /   / \                                 //
	//                               d   e   f                                //
	//                                                                        //
	//                                   |                                    //
	//                                   V                                    //
	//                                                                        //
	//      +----------------------------------------------------------+      //
	//      || +---------------+ | | +-------------------+----------+ ||      //
	//      || |       d       | | | |         e         |    f     | ||      //
	//      || +---------------+ | | +-------------------+----------+ ||      //
	//      ||                   | |                                  ||      //
	//      ||                   | |                                  ||      //
	//      ||                   | |                                  ||      //
	//      ||                   | |                                  ||      //
	//      ||                   | |                                  ||      //
	//      ||         b         | |                c                 ||      //
	//      ||                   | |                                  ||      //
	//      ||                   | |                                  ||      //
	//      ||                   | |                                  ||      //
	//      ||                   | |                                  ||      //
	//      ||                   | |                                  ||      //
	//      ||                   | |                                  ||      //
	//      ||                   | |                                  ||      //
	//      |+-------------------+ +----------------------------------+|      //
	//      +----------------------------------------------------------+      //
	//                                                                        //
	////////////////////////////////////////////////////////////////////////////

	tui_node_s* a = tui_create(def_item("a", 0, 0, 60, 20), NULL);
		tui_node_s* b = tui_create(def_item("b", 1, 1, 21, 18), a);
			tui_node_s* d = tui_create(def_item("d", 3, 2, 17, 3), b);
		tui_node_s* c = tui_create(def_item("c", 23, 1, 36, 18), a);
			tui_node_s* e = tui_create(def_item("e", 25, 2, 21, 3), c);
			tui_node_s* f = tui_create(def_item("f", 45, 2, 12, 3), c);
	tui_node_s* tree = a;

	cairo_expect_streq(a->item.name       , "a"      );
	cairo_expect_nreq( a->item.rect.x     , 0        );
	cairo_expect_nreq( a->item.rect.y     , 0        );
	cairo_expect_nreq( a->item.rect.width , 60       );
	cairo_expect_nreq( a->item.rect.height, 20       );
	cairo_expect_eq(   a->nodes.count     , (size_t)2);
	cairo_expect_eq(   a->parent          , NULL     );
	cairo_expect_false(a->rendered                   );

	cairo_expect_streq(b->item.name       , "b"      );
	cairo_expect_nreq( b->item.rect.x     , 1        );
	cairo_expect_nreq( b->item.rect.y     , 1        );
	cairo_expect_nreq( b->item.rect.width , 21       );
	cairo_expect_nreq( b->item.rect.height, 18       );
	cairo_expect_eq(   b->nodes.count     , (size_t)1);
	cairo_expect_eq(   b->parent          , a        );
	cairo_expect_false(b->rendered                   );

	cairo_expect_streq(c->item.name       , "c"      );
	cairo_expect_nreq( c->item.rect.x     , 23       );
	cairo_expect_nreq( c->item.rect.y     , 1        );
	cairo_expect_nreq( c->item.rect.width , 36       );
	cairo_expect_nreq( c->item.rect.height, 18       );
	cairo_expect_eq(   c->nodes.count     , (size_t)2);
	cairo_expect_eq(   c->parent          , a        );
	cairo_expect_false(c->rendered                   );

	cairo_expect_streq(d->item.name       , "d"      );
	cairo_expect_nreq( d->item.rect.x     , 3        );
	cairo_expect_nreq( d->item.rect.y     , 2        );
	cairo_expect_nreq( d->item.rect.width , 17       );
	cairo_expect_nreq( d->item.rect.height, 3        );
	cairo_expect_eq(   d->nodes.count     , (size_t)0);
	cairo_expect_eq(   d->parent          , b        );
	cairo_expect_false(d->rendered                   );

	cairo_expect_streq(e->item.name       , "e"      );
	cairo_expect_nreq( e->item.rect.x     , 25       );
	cairo_expect_nreq( e->item.rect.y     , 2        );
	cairo_expect_nreq( e->item.rect.width , 21       );
	cairo_expect_nreq( e->item.rect.height, 3        );
	cairo_expect_eq(   e->nodes.count     , (size_t)0);
	cairo_expect_eq(   e->parent          , c        );
	cairo_expect_false(e->rendered                   );

	cairo_expect_streq(f->item.name       , "f"      );
	cairo_expect_nreq( f->item.rect.x     , 45       );
	cairo_expect_nreq( f->item.rect.y     , 2        );
	cairo_expect_nreq( f->item.rect.width , 12       );
	cairo_expect_nreq( f->item.rect.height, 3        );
	cairo_expect_eq(   f->nodes.count     , (size_t)0);
	cairo_expect_eq(   f->parent          , c        );
	cairo_expect_false(f->rendered                   );

	tui_render(NULL, tree);
	cairo_expect_true(a->rendered);
	cairo_expect_true(b->rendered);
	cairo_expect_true(d->rendered);
	cairo_expect_true(c->rendered);
	cairo_expect_true(e->rendered);
	cairo_expect_true(f->rendered);

	cairo_test_defered tui_free(&tree);
	cairo_assert_eq(tree, NULL);
}
