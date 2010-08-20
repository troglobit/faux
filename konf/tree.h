/*
 * conf.h
 */
 /**
\ingroup clish
\defgroup clish_conf config
@{

\brief This class is a config in memory container.

Use it to implement config in memory.

*/
#ifndef _konf_tree_h
#define _konf_tree_h

#include <stdio.h>

#include "lub/types.h"
#include "lub/bintree.h"

typedef struct konf_tree_s konf_tree_t;

/*=====================================
 * CONF INTERFACE
 *===================================== */
/*-----------------
 * meta functions
 *----------------- */
konf_tree_t *konf_tree_new(const char * line, unsigned short priority);
int konf_tree_bt_compare(const void *clientnode, const void *clientkey);
void konf_tree_bt_getkey(const void *clientnode, lub_bintree_key_t * key);
size_t konf_tree_bt_offset(void);

/*-----------------
 * methods
 *----------------- */
void konf_tree_delete(konf_tree_t * instance);
void konf_tree_fprintf(konf_tree_t * instance, FILE * stream,
		const char *pattern,
		int depth, unsigned char prev_pri_hi);
konf_tree_t *konf_tree_new_conf(konf_tree_t * instance,
				const char *line, unsigned short priority);
konf_tree_t *konf_tree_find_conf(konf_tree_t * instance,
				const char *line, unsigned short priority);
void konf_tree_del_pattern(konf_tree_t *this,
				const char *pattern);

/*-----------------
 * attributes 
 *----------------- */
unsigned konf_tree__get_depth(const konf_tree_t * instance);
unsigned short konf_tree__get_priority(const konf_tree_t * instance);
unsigned char konf_tree__get_priority_hi(const konf_tree_t * instance);
unsigned char konf_tree__get_priority_lo(const konf_tree_t * instance);
bool_t konf_tree__get_splitter(const konf_tree_t * instance);
void konf_tree__set_splitter(konf_tree_t *instance, bool_t splitter);

#endif				/* _konf_tree_h */
/** @} clish_conf */