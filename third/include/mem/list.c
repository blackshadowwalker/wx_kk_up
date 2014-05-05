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
 * list.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list.h>
#include <assert.h>
#include "list.h"

/*
 * init a double circle linked list
 */
void dc_linklist_init(struct dclinklist_node *list)
{
  assert(NULL !=list);

  list->prev = list;
  list->next = list;
}


/*
 * insert a node after list head
 */
void dc_linklist_insert(struct dclinklist_node *list, struct dclinklist_node *node)
{
  assert(NULL !=list);
  assert(NULL !=node);

  node->prev = list;
  node->next = list->next;
  list->next->prev = node;
  list->next = node;
}

/*
 * remove a node from list
 */
void dc_linklist_remove(struct dclinklist_node *list, struct dclinklist_node *node)
{
  assert(NULL != node);
  assert(NULL != node->prev);
  assert(NULL != node->next);

  node->prev->next = node->next;
  node->next->prev = node->prev;
}

/*
 * append a node to the end of list
 */
void dc_linklist_append(struct dclinklist_node *list, struct dclinklist_node *node)
{
  assert(NULL != list);
  assert(NULL != node);

  node->prev = list->prev;
  node->next = list;
  list->prev->next = node;
  list->prev = node;
}

/*
 * append a node to the end of list
 */
void dc_linklist_append_list(struct dclinklist_node *list, struct dclinklist_node *newlist)
{
  struct dclinklist_node *last;

  assert(NULL != list);
  assert(NULL != newlist);

  last = newlist->prev;
  newlist->prev = list->prev;
  last->next = list;
  list->prev->next = newlist;
  list->prev =last;
}

/************************************************************
*
*
*   sorted double link list library
*
*
*
***************************************************************/
/* Allocates |size| bytes of space using |malloc()|.
   Returns a null pointer if allocation fails. */
void *
dl_malloc (struct libdl_allocator *allocator, size_t size)
{
  assert (allocator != NULL && size > 0);
  return malloc (size);
}

/* Frees |block|. */
void
dl_free (struct libdl_allocator *allocator, void *block)
{
  assert (NULL != allocator && NULL != block );
  free (block);
}

/* Default memory allocator that uses |malloc()| and |free()|. */
struct libdl_allocator dl_allocator_default =
  {
    dl_malloc,
    dl_free
  };


/* Add a new node to the list tail.  */
static void
dl_add (struct dl_list *list,struct dl_node *node)
{
  node->next = NULL;
  node->prev = list->tail;
  if (list->tail)
    list->tail->next = node;
  else
    list->head = node;
  list->tail = node;
  list->dl_count++;
}


/* Add a new node just before the point.  */
static void
dl_add_before (struct dl_list *list,struct  dl_node *point,struct  dl_node *node)
{
  node->next = point;
  node->prev = point->prev;
  if (point->prev)
    point->prev->next = node;
  else
    list->head = node;
  point->prev = node;
  list->dl_count++;
}


/* Delete a node from the list and return the data in the node. */
void* dl_node_delete (struct dl_list *list, dl_node_t* node)
{
  void * data;
  
  if (node->next)
    node->next->prev = node->prev;
  else
    list->tail = node->prev;
  if (node->prev)
    node->prev->next = node->next;
  else
    list->head = node->next;
  node->next = node->prev = NULL;
  //release the node
  data = node->data;
  list->dl_alloc->libdl_free (list->dl_alloc, node);
  list->dl_count--;
  return data;
}

/* Creates and returns a new list
   with comparison function |compare| using parameter |param|
   and memory allocator |allocator|.
   Returns |NULL| if memory allocation failed. */
struct dl_list*
dl_create (dl_comparison_func *compare, void *param, size_t datasize,
            struct libdl_allocator *allocator)
{
  struct dl_list *list;

  assert (compare != NULL);

  if (allocator == NULL)
    allocator = &dl_allocator_default;

  list = allocator->libdl_malloc (allocator, sizeof *list);
  if (list == NULL)
    return NULL;

  list->head = NULL;
  list->tail = NULL;  
  list->dl_compare = compare;
  list->dl_param = param;
  list->dl_alloc = allocator;
  list->dl_count = 0;
  list->datasize = datasize;

  return list;
}

/* Inserts |item| into |list| and returns a pointer to |item|'s address.
   If a duplicate item is found in the list,
   returns a pointer to the duplicate without inserting |item|.
   Returns |NULL| in case of memory allocation failure. */
void **
dl_insert (struct dl_list *list, void *item)
{
  dl_node_t* tt;
  dl_node_t* node;
  
  for (tt = list->head; tt; tt = tt->next)
  {
    int cmp = list->dl_compare(tt->data, item,NULL);
    if(!cmp) return &tt->data;
    else if (cmp > 0) break;
  }

  node = list->dl_alloc->libdl_malloc (list->dl_alloc, sizeof *node);
  if (node == NULL)
    return NULL;
  node->data =item;
    
  if (tt)
    dl_add_before (list, tt, node);
  else
    dl_add (list, node);

  return &node->data;
}
void **
dl_insert_tail(struct dl_list *list, void *item)
{
  dl_node_t* node;
  
  node = list->dl_alloc->libdl_malloc (list->dl_alloc, sizeof *node);
  if (node == NULL)
    return NULL;
  node->data =item;

  dl_add (list, node);

  return &node->data;
}

void **
dl_insert_head(struct dl_list *list, void *item)
{
  dl_node_t* node;
  
  node = list->dl_alloc->libdl_malloc (list->dl_alloc, sizeof *node);
  if (node == NULL)
    return NULL;
  node->data =item;

  if (list->head)
    dl_add_before (list, list->head, node);
  else
    dl_add (list, node);

  return &node->data;
}

/* Deletes from |list| and returns an item matching |item|.
   Returns a null pointer if no matching item found. */
void * dl_delete (struct dl_list *list, const void *item)
{
  dl_node_t* tt;
   
  for (tt = list->head; tt; tt = tt->next)
    if (list->dl_compare(tt->data, item,NULL) == 0)
      break;

  if(NULL == tt) return NULL;
  else
  {
      return dl_node_delete(list,tt);
  }
}


/* Frees storage allocated for |list|.
   If |destroy != NULL|, applies it to each data item in inorder. */
void
dl_destroy (struct dl_list *list, dl_item_func *destroy)
{
  struct dl_node *p;

  assert (list != NULL);
  for (p = list->head; p != NULL; p = p->next)
  {
        if (destroy != NULL && p->data != NULL)
        {
          destroy (p->data, list->dl_param);
          list->dl_alloc->libdl_free (list->dl_alloc, p);
        }
  }

  list->dl_alloc->libdl_free (list->dl_alloc, list);
}

