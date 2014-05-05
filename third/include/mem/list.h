/*
 *  This file is part of libmempool, a library for memory management
 *
 *  Copyright (C) 2009  <li_guang2009@sohu.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 *   list.h
 */

#ifndef __LIST_H__
#define __LIST_H__
#include <stddef.h>

/*********************************************************************************
 * data and structure for function version double linked list
 *********************************************************************************/
/* Function types. */
typedef int dl_comparison_func (const void *dl_a, const void *dl_b,
                                 void *dl_param);
typedef void dl_item_func (void *dl_item, void *dl_param);
//typedef void *dl_copy_func (void *dl_item, void *dl_param);

#ifndef LIBDL_ALLOCATOR
#define LIBDL_ALLOCATOR
/* Memory allocator. */
struct libdl_allocator
  {
    void *(*libdl_malloc) (struct libdl_allocator *, size_t libdl_size);
    void (*libdl_free) (struct libdl_allocator *, void *libdl_block);
  };
#endif

/* Default memory allocator. */
extern struct libdl_allocator dl_allocator_default;
void *dl_malloc (struct libdl_allocator *, size_t);
void dl_free (struct libdl_allocator *, void *);


/*an double link list node*/
typedef struct dl_node{
    struct dl_node * prev; /*pointer to previous node*/
    struct dl_node * next ; /*pointer to next node*/
    void * data;                /*pointer to data. */
}dl_node_t;

/*double link list structure*/
typedef struct dl_list{
    struct dl_node * head; /*pointer to list header*/
    struct dl_node * tail;    /*pointer to list tail*/
    dl_comparison_func *dl_compare;   /* Comparison function. */
    void *dl_param;                    /* Extra argument to |dl_compare|. */
    struct libdl_allocator *dl_alloc; /* Memory allocator. */
    size_t dl_count;                   /* Number of items in list. */
    size_t datasize; /* sizeof(*data) */
}dl_list_t;

dl_list_t *dl_create (dl_comparison_func *, void *,size_t datasize,struct libdl_allocator *);
void dl_destroy (struct dl_list *, dl_item_func *);
void **dl_insert (struct dl_list *table, void *item);
void ** dl_insert_tail(struct dl_list *table, void *item);
void ** dl_insert_head(struct dl_list *table, void *item);
//void *dl_find (const struct dl_list *tree, const void *item);
void *dl_delete (struct dl_list *tree, const void *item);
/* Delete a node from the list and return the data in the node. */
void* dl_node_delete (struct dl_list *list, dl_node_t* node);

struct dclinklist_node
{
    struct dclinklist_node *prev;
    struct dclinklist_node *next;
};

void dc_linklist_init(struct dclinklist_node *list);
void dc_linklist_insert(struct dclinklist_node *list, struct dclinklist_node *node);
void dc_linklist_remove(struct dclinklist_node *list, struct dclinklist_node *node);
void dc_linklist_append(struct dclinklist_node *list, struct dclinklist_node *node);
void dc_linklist_append_list(struct dclinklist_node *list, struct dclinklist_node *newlist);

#endif /* __D_LIST_H__ */
