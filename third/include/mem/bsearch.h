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
#ifndef _BSEARCH_EXT_H_
#define _BSEARCH_EXT_H_

int bsearch_nearest(const void *search_array, unsigned int step, unsigned int size, int (*cmpfunc)(const void *, const void *), const void *search_node);
int bsearch_exact(const void *search_array, unsigned int step, unsigned int size, int (*cmpfunc)(const void *, const void *), const void *search_node);

#endif /* _BSEARCH_EXT_H_ */

