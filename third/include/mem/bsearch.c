/*
 *  This file is part of libmempool, a library for memory management
 *
 *  Copyright (C) 2009   <li_guang2009@sohu.com>
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
 * bsearch.c
 */

//#define BSEARCH_UNITEST

#include <stdio.h>

#include "bsearch.h"

int bsearch_exact(const void *search_array, size_t step, size_t size, int (*cmpfunc)(const void *, const void *), const void *search_node)
{
    int low, high, mid;
    int ret;
    unsigned char *sbase;

    /* parameter check */
    if(search_array == NULL || size == 0 || step == 0 || cmpfunc == NULL || search_node == NULL)
        return -1;

    sbase = (unsigned char *)search_array;
    low = 0;
    high = size - 1;

    while(low <= high)
    {
        mid = (low + high)/2;

#ifdef BSEARCH_UNITEST
        printf("low=%d, mid=%d, high=%d\n", low, mid, high);
#endif

        ret = cmpfunc(&sbase[mid*step], search_node);

        if(ret < 0)
            low = mid + 1;
        else if(ret > 0)
            high = mid - 1;
        else
            return mid;
    }

    return -1;
}

/*
 * binary search in array
 * search_array: sorted array, from small to big
 * cmpfunc: equal=0, less<0, greater>0
 * return: the index that most closest to search node
 */
int bsearch_nearest(const void *search_array, size_t step, size_t size, int (*cmpfunc)(const void *, const void *), const void *search_node)
{
    int low, high, mid;
    int ret;
    unsigned char *sbase;

    /* parameter check */
    if(search_array == NULL || size == 0 || step == 0 || cmpfunc == NULL || search_node == NULL)
        return -1;

    sbase = (unsigned char *)search_array;
    low = 0;
    high = size - 1;
    mid =0;

    while(low <= high)
    {
        mid = (low + high)/2;

#ifdef BSEARCH_UNITEST
        printf("low=%d, mid=%d, high=%d\n", low, mid, high);
#endif

        ret = cmpfunc(&sbase[mid*step], search_node);

        if(ret < 0)
            low = mid + 1;
        else if(ret > 0)
            high = mid - 1;
        else
            return mid;
    }

    return mid;
}



