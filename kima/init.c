#include "init.h"

void kima_init() {
	kima_rbtree_init(
		&kima_vpgdesc_query_tree,
		kima_vpgdesc_nodecmp,
		kima_vpgdesc_nodefree);
	kima_rbtree_init(
		&kima_ublk_query_tree,
		kima_ublk_nodecmp,
		kima_ublk_nodefree);
}

void kima_deinit() {

}
