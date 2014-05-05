/*
 *  This file is part of libmempool, a library for memory management
 *
 *  Copyright (C) 2009    <li_guang2009@sohu.com>
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
#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_

/* defaultly support multithread */
#define MULTITHREAD_POOL

#ifdef MULTITHREAD_POOL
#include <pthread.h>
#endif /* MULTITHREAD_POOL */

#include "list.h"

struct memsubpool;

struct memblock
{
    struct memblock *next;
    // void *data;          // no need, can be calculated
};

struct memtrunk
{
    struct memtrunk *prev;
    struct memtrunk *next;

    struct submempool *subpool;
    struct memblock *freelst;           /* free block list */

    unsigned short totalcnt;            /* total memblock count */
    unsigned short freecnt;             /* free memblock count */
    struct memblock *blocks;            /* memblock start address */
    unsigned char *data_begin;
    unsigned char *data_end;
    unsigned char *bitmap;
};

#define SUBMEMPOOL_FLAG_AUTOGROW    0x01
#define SUBMEMPOOL_FLAG_ALIGN4      0x04
#define SUBMEMPOOL_FLAG_ALIGN8      0x08
#define SUBMEMPOOL_FLAG_ALIGN16     0x10
#define SUBMEMPOOL_FLAG_ALIGN32     0x20
#define SUBMEMPOOL_FLAG_ALIGN64     0x40

struct submempool
{
    size_t block_size;                  /* after alignment */
    size_t trunk_size;                  /* for trunk auto growing */

    unsigned long flags;
    size_t totalcnt;                    /* total memblock count */
    size_t freecnt;                     /* free memblock count */

    struct dclinklist_node trunklst;    /* empty first, full last */
};

#define SUBMEMPOOL_MAX_NUM 50
#define TRUNK_MAX_NUM 2000

struct mempool
{
#ifdef MULTITHREAD_POOL
    pthread_mutex_t mutex;
#endif /* MULTITHREAD_POOL */

    size_t subpool_num;
    struct submempool *pools[SUBMEMPOOL_MAX_NUM];

    size_t new_trunks;
    size_t trunk_num;
    struct memtrunk *trunks[TRUNK_MAX_NUM];
};

struct mempool * mempool_create();
void mempool_release(struct mempool *mpool);
struct submempool * submempool_create(struct mempool *mpool, size_t blksiz, size_t trksiz, unsigned long flags);
struct submempool *submempool_get(struct mempool *mpool, size_t blksiz, unsigned long align);
struct memtrunk *submempool_add_trunk(struct mempool* mpool, struct submempool *subpool, void *trunk, size_t size);

void *mempool_alloc(struct mempool* mpool, int size);
int mempool_free(struct mempool* mpool, void *p);

void mempool_dump(struct mempool *mpool);

#endif /* _MEMPOOL_H_ */


