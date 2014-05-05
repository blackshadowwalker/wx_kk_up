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
 * mempool.c
 */
#ifndef _GNU_SOURCE
// to support recursive mutex
#define _GNU_SOURCE    1
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#include "bsearch.h"
/* pthread.h is included in mempool.h, don't put it here */
#include "mempool.h"

#undef ZLOG_MODULE
#define ZLOG_MODULE ZLOGMOD_MEMPOOL

/* malloc marco, for portation */
#define MEMPOOL_ALLOC(siz) malloc(siz)
#define MEMPOOL_FREE(p) free(p)
#define SUBMEMPOOL_NEW() malloc(sizeof(struct submempool))
#define MEMPOOL_NEW() malloc(sizeof(struct mempool))

#define MIN_BLOCK_SIZ 4

static int subpool_cmpfunc_qsort(const void *node1, const void *node2);
static int subpool_cmpfunc(const void *node, const void *key);

/*
 * set one bit in bits to 1
 * pos start from 0
 */
static void bitmap_mark(int *bits, size_t pos)
{
        int n, nbit;

        n = pos/32;
        nbit = pos%32;
        bits[n] |= 0x01 << nbit;
}

/*
 * set one bit in bits to 0
 */
static void bitmap_unmark(int *bits, size_t pos)
{
        int n, nbit;

        n = pos/32;
        nbit = pos%32;
    bits[n] &= ~(0x01 << nbit);
}

/*
 * get one bit in bits, pos starting from 0
 * return 1 or 0;
 */
static int bitmap_get(int *bits, size_t pos)
{
        int n, nbit;

        n = pos/32;
        nbit = pos%32;
    return (bits[n] & (0x01 << nbit))?1:0;
}

/*
 * create a empty memory pool
 */
struct mempool * mempool_create(void)
{
    struct mempool *mpool;
#ifdef MULTITHREAD_POOL
    pthread_mutexattr_t   attr;
#endif

    mpool = (struct mempool *)MEMPOOL_NEW();

    if(mpool)
    {
        memset(mpool, 0, sizeof(struct mempool));

#ifdef MULTITHREAD_POOL
        /* mutex init, must support recursive lock */
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&mpool->mutex, &attr);
#endif
    }

    return mpool;
}

void mempool_release(struct mempool *mpool)
{
    /* TBD */

#ifdef MULTITHREAD
        /* semarphore init */
#endif
}

/*
 * create a sub-pool in a mempool
 * parameters:
 *   mpool -- mempool pointer
 *   blksiz -- size of memory block
 *   trksiz -- size of trunk
 *   flags -- sub-pool flags
 */
struct submempool * submempool_create(struct mempool *mpool, size_t blksiz, size_t trksiz, unsigned long flags)
{
    struct submempool *subpool;
    unsigned long align, fother;
    int i;

    if(mpool == NULL || blksiz == 0)
        return NULL;

    if(trksiz == 0 && (flags & SUBMEMPOOL_FLAG_AUTOGROW))
    {
        ZLOG_WARN("submempool_create(): AUTOGROW flag set but trunk size configed to zero");
        return NULL;
    }

    /* the minimum block size must be 16 */
    if(blksiz < MIN_BLOCK_SIZ)
    {
        ZLOG_WARN("submempool_create(): adjust the block size to %d", MIN_BLOCK_SIZ);
        blksiz = MIN_BLOCK_SIZ;
    }

    /* adjust the block size to next aligning-border */
    fother = flags & 0x03;
    align = flags & 0xFC;
    switch(align)
    {
        case 0:
            break;

        case SUBMEMPOOL_FLAG_ALIGN4:
        case SUBMEMPOOL_FLAG_ALIGN8:
        case SUBMEMPOOL_FLAG_ALIGN16:
        case SUBMEMPOOL_FLAG_ALIGN32:
        case SUBMEMPOOL_FLAG_ALIGN64:
            blksiz = (blksiz + align - 1) & (~(align-1));
            break;

        default:
            ZLOG_WARN("submempool_create(): unknown alignment flag:0x%x, reset it to 0.", (unsigned int)align);
            align = 0;
            break;
    }

#ifdef MULTITHREAD_POOL
    pthread_mutex_lock(&mpool->mutex);
#endif
    /* check for duplicated block size */
    for(i=0; i<mpool->subpool_num; i++)
    {
        if(blksiz == (mpool->pools[i])->block_size)
        {
            ZLOG_WARN("submempool_create(): duplicate with sub-pool %d", i);
            /* return the duplicated one */
#ifdef MULTITHREAD_POOL
            pthread_mutex_unlock(&mpool->mutex);
#endif
            return mpool->pools[i];
        }
    }

#ifdef MULTITHREAD_POOL
    // don't want involve external calls, unlock mutex
    pthread_mutex_unlock(&mpool->mutex);
#endif
    subpool = SUBMEMPOOL_NEW();

    if(subpool)
    {
        subpool->block_size = blksiz;
        subpool->trunk_size = trksiz;
        subpool->flags = align | fother;
        subpool->totalcnt = 0;
        subpool->freecnt = 0;
        dc_linklist_init(&(subpool->trunklst));

#ifdef MULTITHREAD_POOL
    pthread_mutex_lock(&mpool->mutex);
#endif
        /* update subpool search array */
        mpool->pools[mpool->subpool_num++] = subpool;

        /* re-sort search array */
        qsort(mpool->pools, mpool->subpool_num, sizeof(struct submempool *), subpool_cmpfunc_qsort);

#ifdef MULTITHREAD_POOL
    pthread_mutex_unlock(&mpool->mutex);
#endif
    }

    return subpool;
}

/*
 * find a sub-pool by block size and return it
 */
struct submempool *submempool_get(struct mempool *mpool, size_t blksiz, unsigned long align)
{
    int i;
    if(mpool == NULL || blksiz == 0)
        return NULL;

    /* adjust blksiz */
    align = align & 0xFC;
    switch(align)
    {
        case SUBMEMPOOL_FLAG_ALIGN4:
        case SUBMEMPOOL_FLAG_ALIGN8:
        case SUBMEMPOOL_FLAG_ALIGN16:
        case SUBMEMPOOL_FLAG_ALIGN32:
        case SUBMEMPOOL_FLAG_ALIGN64:
            align --;
            blksiz = (blksiz + align) & (~align);
            break;
    }

#ifdef MULTITHREAD_POOL
    pthread_mutex_lock(&mpool->mutex);
#endif
    /* check for duplicated block size */
    for(i=0; i<mpool->subpool_num; i++)
    {
        if(blksiz == (mpool->pools[i])->block_size)
        {
#ifdef MULTITHREAD_POOL
            pthread_mutex_unlock(&mpool->mutex);
#endif
            return mpool->pools[i];
        }
    }

#ifdef MULTITHREAD_POOL
    pthread_mutex_unlock(&mpool->mutex);
#endif

    return NULL;
}


/*
 * add a trunk for subpool
 * paramters:
 *   mpool -- mempool pointer
 *   subpool -- subpool pointer
 *   trunk -- trunk memory starting address
 *   size -- size of trunk
 *   return -- turnk pointer
 */
struct memtrunk *submempool_add_trunk(struct mempool* mpool, struct submempool *subpool, void *trunk, size_t size)
{
    struct memtrunk *ptrunk;
    int totalcnt;
    unsigned long alignment;
    size_t block_size;
    struct memblock *pblk;

    if(mpool == NULL || subpool == NULL || trunk == NULL)
        return NULL;

    if(mpool->trunk_num >= TRUNK_MAX_NUM)
        return NULL;

#ifdef MULTITHREAD_POOL
    pthread_mutex_lock(&mpool->mutex);
#endif
    alignment = subpool->flags & 0xfc;  /* get alignment */
    if(alignment)
        alignment --;
    block_size = subpool->block_size;   // get block size

    ptrunk = (struct memtrunk *)trunk;  // no alignment, optimize me!!
    ptrunk->prev = NULL;
    ptrunk->next = NULL;
    ptrunk->subpool = subpool;
    ptrunk->freelst = NULL;
    ptrunk->bitmap = (unsigned char *)(ptrunk + 1);

    /* the first try of totalcnt */
    totalcnt =
        8*(size-sizeof(struct memtrunk))/(8*subpool->block_size + 8*sizeof(struct memblock) + 1);
    for(; totalcnt>0; totalcnt --)
    {
        unsigned char *ptmp;

        /* set ->blocks and align ->blocks to 4 bytes border */
        ptmp = ptrunk->bitmap + (totalcnt+7)/8;
        ptmp = (unsigned char *)((int)(ptmp+3) & 0xFFFFFFFC);
        ptrunk->blocks = (struct memblock *)ptmp;

        /* set ->data_begin and align data_begin to align-requirement */
        ptmp = (unsigned char *)(ptrunk->blocks+totalcnt);
        ptrunk->data_begin = (unsigned char *)((int)(ptmp + alignment) & (~alignment));

        ptrunk->data_end = ptrunk->data_begin + totalcnt*block_size;
        if(ptrunk->data_end <= (unsigned char *)trunk + size)
        {
            /* ok hit */
            memset(ptrunk->bitmap, 0, (totalcnt+7)/8);  //clear bitmap
            ptrunk->totalcnt = totalcnt;
            ptrunk->freecnt = totalcnt;
            subpool->totalcnt += totalcnt;
            subpool->freecnt += totalcnt;
            break;
        }
    }

    if(ptrunk->totalcnt <= 0)
    {
#ifdef MULTITHREAD_POOL
        pthread_mutex_unlock(&mpool->mutex);
#endif
        ZLOG_ERR("submempool_add_trunk(): trunk size (%d) is too small", size);
        return NULL;
    }

    //ZLOG_DEBUG("submempool_add_trunk(): new a trunk(%p) with %d blocks", ptrunk, totalcnt);

    /* divided into blocks and insert them into free list */
    pblk = ptrunk->blocks;
    while(totalcnt --)
    {
        pblk->next = ptrunk->freelst;
        ptrunk->freelst = pblk;
        pblk ++;
    }

    /* add trunk to subpool's trunklst */
    dc_linklist_insert(&(subpool->trunklst), (struct dclinklist_node *)ptrunk);

    /* update mpool search array, but sorting is delayed to free time */
    mpool->trunks[mpool->trunk_num++] = ptrunk;
    mpool->new_trunks++;

#ifdef MULTITHREAD_POOL
    pthread_mutex_unlock(&mpool->mutex);
#endif
    return ptrunk;
}

/*
 * subpool compare function used in bsearch_nearest
 * compare the block size of a subpool with the key
 */
static int subpool_cmpfunc(const void *node, const void *key)
{
    struct submempool **pl;
    int size;

    assert(NULL != node);
    assert(NULL != key);

    pl = (struct submempool **)node;
    size = *(int *)key;

    if((*pl)->block_size == size)
        return 0;
    else if((*pl)->block_size < size)
        return -1;
    else
        return 1;
}

/*
 * subpool compare function used in qsort
 * compare the block size of two subpool
 */
static int subpool_cmpfunc_qsort(const void *node1, const void *node2)
{
    struct submempool **p1, **p2;

    assert(NULL != node1);
    assert(NULL != node2);

    p1 = (struct submempool **)node1;
    p2 = (struct submempool **)node2;

    if((*p1)->block_size == (*p2)->block_size)
        return 0;
    else if((*p1)->block_size < (*p2)->block_size)
        return -1;
    else
        return 1;
}


/*
 * mempool_alloc()
 *   fast allocated
 */
void *mempool_alloc(struct mempool* mpool, int size)
{
    int subpool_idx;
    struct submempool* subpool;
    struct memtrunk *ptrunk;
    struct memblock *pblock;
    int index;
    void *newtrunk;

    if(mpool == NULL || size <= 0)
        return NULL;

#ifdef MULTITHREAD_POOL
    pthread_mutex_lock(&mpool->mutex);
#endif
    if((mpool->pools[mpool->subpool_num-1])->block_size < size)
    {
#ifdef MULTITHREAD_POOL
        pthread_mutex_unlock(&mpool->mutex);
#endif
        return NULL;    /* larger than the maximum block size */
    }

    /* find a most suitable sub-pool */
    subpool_idx = bsearch_nearest(mpool->pools, sizeof(void *), mpool->subpool_num, subpool_cmpfunc, &size);

    if(subpool_idx <0 || subpool_idx >= mpool->subpool_num)
    {
        ZLOG_WARN("mempool_alloc(): no suitable sub-pool");
#ifdef MULTITHREAD_POOL
        pthread_mutex_unlock(&mpool->mutex);
#endif
        return NULL;
    }
    else if((mpool->pools[subpool_idx])->block_size < size)
        subpool_idx ++;

    /* check the sub pool's free list */
    subpool = mpool->pools[subpool_idx];
    if((subpool->freecnt <= 0) && (subpool->flags & SUBMEMPOOL_FLAG_AUTOGROW) &&
        (mpool->trunk_num < TRUNK_MAX_NUM))
    {
#ifdef MULTITHREAD_POOL
        /* don't include exteranl call to avoid deadlock */
        pthread_mutex_unlock(&mpool->mutex);
#endif
        /* new trunk needed */
        newtrunk = MEMPOOL_ALLOC(subpool->trunk_size);
        if(NULL == newtrunk)
        {
            ZLOG_WARN("mempool_alloc(): new trunk request to system failed");
            return NULL;
        }
#ifdef MULTITHREAD_POOL
        pthread_mutex_lock(&mpool->mutex);
#endif
        ptrunk = submempool_add_trunk(mpool, subpool, newtrunk, subpool->trunk_size);

#ifdef MULTITHREAD_POOL
        pthread_mutex_unlock(&mpool->mutex);
#endif
        if(ptrunk == NULL)
        {
            MEMPOOL_FREE(newtrunk);
            return NULL;
        }
#ifdef MULTITHREAD_POOL
        pthread_mutex_lock(&mpool->mutex);
#endif
    }

    if(subpool->freecnt <= 0)
    {
        ZLOG_WARN("mempool_alloc(): no more trunks!");
#ifdef MULTITHREAD_POOL
        pthread_mutex_unlock(&mpool->mutex);
#endif
        return NULL;
    }

    /* retrieve the head node and return it */
    ptrunk = (struct memtrunk *)subpool->trunklst.next;
    assert(ptrunk->freecnt > 0);
    pblock = ptrunk->freelst;
    ptrunk->freelst = pblock->next;
    pblock->next = NULL;
    ptrunk->freecnt--;
    subpool->freecnt--;

    /* mark the trunk bitmap */
    index = pblock - ptrunk->blocks;
    assert(index>=0);
    assert(index<ptrunk->totalcnt);
    bitmap_mark((int *)ptrunk->bitmap, index);

    if(ptrunk->freecnt == 0 && subpool->freecnt != 0)
    {
        /* move the trunk to list tail */
        dc_linklist_remove(&(subpool->trunklst), (struct dclinklist_node *)ptrunk);
        dc_linklist_append(&(subpool->trunklst), (struct dclinklist_node *)ptrunk);
    }

#ifdef MULTITHREAD_POOL
    pthread_mutex_unlock(&mpool->mutex);
#endif
    //ZLOG_DEBUG("mempool_alloc(): allocate %dth block with size %d, in trunk(%p) in subpool(%p)", index+1, subpool->block_size, ptrunk, subpool);

    return (void *)(ptrunk->data_begin + index*subpool->block_size);
}


/*
 * trunk compare function used by bsearch
 * compare trunk->data_begin with p
 */
static int trunk_cmpfunc(const void *trunk, const void *p)
{
    struct memtrunk **ptrunk;

    assert(NULL != trunk);
    assert(NULL != p);

    ptrunk = (struct memtrunk **)trunk;

    if((void *)((*ptrunk)->data_end) < p)
        return -1;
    else if((void *)((*ptrunk)->data_begin) <= p)
        return 0;
    else
        return 1;
}


/*
 * trunk compare function used by qsort
 * compare two trunk
 */
static int trunk_cmpfunc_qsort(const void *trunk1, const void *trunk2)
{
    struct memtrunk **p1, **p2;
    assert(NULL != trunk1);
    assert(NULL != trunk2);

    p1 = (struct memtrunk **)trunk1;
    p2 = (struct memtrunk **)trunk2;

    if((*p1)->data_begin < (*p2)->data_begin)
    {
        assert((*p1)->data_end < (*p2)->data_end);
        return -1;
    }
    else if((*p1)->data_begin == (*p2)->data_begin)
    {
        ZLOG_WARN("two trunk have same data area: %p<-->%p", trunk1, trunk2);
        return 0;
    }
    else
    {
        assert((*p1)->data_end > (*p2)->data_end);
        return 1;
    }

    return 0;   // never reach here
}


/*
 * free a memory block back to mempool
 */
int mempool_free(struct mempool* mpool, void *p)
{
    int trunk_idx;
    int block_idx;
    struct memtrunk *ptrunk;
    struct submempool *subpool;
    struct memblock *pblock;

    if(mpool == NULL || p == NULL)
        return -1;

#ifdef MULTITHREAD_POOL
    pthread_mutex_lock(&mpool->mutex);
#endif
    if(mpool->new_trunks)
    {
        /* sort the trunks */
        qsort(mpool->trunks, mpool->trunk_num, sizeof(struct memtrunk *), trunk_cmpfunc_qsort);

        mpool->new_trunks = 0;
    }

    /* find the trunk */
    trunk_idx = bsearch_exact(mpool->trunks, sizeof(void *), mpool->trunk_num, trunk_cmpfunc, p);
    if(trunk_idx < 0 || trunk_idx >= mpool->trunk_num)
    {
#ifdef MULTITHREAD_POOL
        pthread_mutex_unlock(&mpool->mutex);
#endif
        ZLOG_WARN("mempool_free(): no trunk found for address %p", p);
        return -1;
    }

    /* get block index */
    ptrunk = mpool->trunks[trunk_idx];
    subpool = ptrunk->subpool;
    block_idx = ((unsigned char *)p - ptrunk->data_begin)/subpool->block_size;
    if(block_idx < 0 || block_idx >= ptrunk->totalcnt)
    {
#ifdef MULTITHREAD_POOL
        pthread_mutex_unlock(&mpool->mutex);
#endif
        ZLOG_WARN("mempool_free(): block index %d out of range", block_idx);
        return -1;
    }

    /* check pointer's validity */
    if(ptrunk->data_begin + block_idx * subpool->block_size != p)
    {
#ifdef MULTITHREAD_POOL
        pthread_mutex_unlock(&mpool->mutex);
#endif
        ZLOG_WARN("mempool_free(): free addr invalid %p, block_idx=%d",
            p, block_idx);
        return -1;
    }

    if(bitmap_get((int *)ptrunk->bitmap, block_idx) != 1)
    {
#ifdef MULTITHREAD_POOL
        pthread_mutex_unlock(&mpool->mutex);
#endif
        ZLOG_WARN("mempool_free(): bitmap is 0, block_idx=%d!",
            block_idx);
        return -1;
    }

    /* ok, check passed, add the block to free list */
    //ZLOG_DEBUG("mempool_free(); free address %p: %dth block with size %d in trunk(%p) in subpool(%p)", p, block_idx+1, subpool->block_size, ptrunk, subpool);
    bitmap_unmark((int *)ptrunk->bitmap, block_idx);
    pblock = &(ptrunk->blocks[block_idx]);

    pblock->next = ptrunk->freelst;
    ptrunk->freelst = pblock;
    ptrunk->freecnt++;
    subpool->freecnt++;

    if(ptrunk->freecnt == 1)
    {
        /* move the ptrunk to list head */
        dc_linklist_remove(&(subpool->trunklst), (struct dclinklist_node *)ptrunk);
        dc_linklist_insert(&(subpool->trunklst), (struct dclinklist_node *)ptrunk);
    }

#ifdef MULTITHREAD_POOL
    pthread_mutex_unlock(&mpool->mutex);
#endif
    return 0;
}


/* dump utils */
void mempool_dump(struct mempool *mpool)
{
    int npool;
    int ntrunk;

    if(mpool == NULL) return;

#ifdef MULTITHREAD_POOL
    pthread_mutex_lock(&mpool->mutex);
#endif
    ZLOG_INFO("mempool_dump(): %d subpools %d trunks %d new-trunks",
        mpool->subpool_num, mpool->trunk_num, mpool->new_trunks);
    ZLOG_INFO("mempool_dump(): trunk array:");
    for(ntrunk=0; ntrunk<mpool->trunk_num; ntrunk++)
        ZLOG_INFO("  %d -- %p", ntrunk, mpool->trunks[ntrunk]);

    for(npool = 0; npool < mpool->subpool_num; npool ++)
    {
        struct submempool *subpool;
        struct memtrunk *ptrunk;

        subpool = mpool->pools[npool];
        ZLOG_INFO("****sub-pool%d: %p****", npool+1, subpool);
        ZLOG_INFO("    block size (%d), trunk size (%d)",
            subpool->block_size, subpool->trunk_size);
        ZLOG_INFO("    flags (0x%x), totalcnt (%d), freecnt (%d)",
            (unsigned int)subpool->flags, subpool->totalcnt, subpool->freecnt);

        ntrunk = 1;
        ptrunk = (struct memtrunk *)(subpool->trunklst.next);
        while(&(subpool->trunklst) != (struct dclinklist_node *)ptrunk)
        {
            ZLOG_INFO("======trunk%d: %p======", ntrunk, ptrunk);
            ZLOG_INFO("      totalcnt (%d), freecnt (%d)",
                ptrunk->totalcnt, ptrunk->freecnt);
            ZLOG_INFO("      data (%p--%p)",
                ptrunk->data_begin, ptrunk->data_end);

            /* dump bitmap */

            /* dump free list */

            ptrunk = ptrunk->next;
            ntrunk ++;
        }
    }
#ifdef MULTITHREAD_POOL
    pthread_mutex_unlock(&mpool->mutex);
#endif
}


