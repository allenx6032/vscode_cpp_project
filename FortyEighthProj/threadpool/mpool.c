/*
 *                 GNU GENERAL PUBLIC LICENSE
 *                  Version 3, 29 June 2007
 *                 Copyright (C) 2016 XD.Yang
 *   Everyone is permitted to copy and distribute verbatim copies
 *   of this license document, but changing it is not allowed.
 */

#include <stdio.h>
#include <stdlib.h>

#include "mpool.h"

#define     Inc_Ptr( PTR,N )            ((char*)PTR+N)
#define     Dec_Ptr( PTR,N )            ((char*)PTR-N)
#define     nextof(Ptr,BSZ)             (*(void**)Inc_Ptr(Ptr,BSZ))

#define     NOUSE

Block*  pool_create(int bsz, int count)
{
    Block   *res = (Block*)malloc(sizeof(Block));
    int     realsize = bsz + sizeof( void* );
    void    *_Mem = (void*)calloc(count,realsize);

    int i;
    res->_slots = Linear_New();
    Linear(res->_slots,10);
    res->_first = _Mem;
    res->_cap = count;
    res->_size = count;
    res->_bsz = bsz;
    for ( i = 0 ; i < count-1; ++i)
    {
       *(void**)Inc_Ptr(_Mem , i*realsize + bsz)= Inc_Ptr(_Mem,(i+1)*realsize);
    }
    Line_Append(res->_slots,_Mem);
    return res;
}
void    extend(Block *blk,int sz,int bsz)
{
    void *_Mem = NULL;
    int i = 0,realsize = bsz + sizeof(void*);

    _Mem = calloc(sz,bsz+ sizeof(void*));

    Line_Append(blk->_slots,_Mem);
    blk->_first = _Mem;
    for ( ; i<sz-1; ++i )
    {
        *(void**)Inc_Ptr(_Mem , i*realsize + bsz) = Inc_Ptr(_Mem , (i+1) * realsize);
    }
}

void*   m_malloc(Block* blk)
{
    void *res = NULL;
    res = blk->_first;
    if ( res == NULL )
    {
        int sz = blk->_cap * 1.5;
        extend(blk,sz,blk->_bsz);
        blk->_size = sz;
        blk->_cap += sz;
        res = blk->_first;
    }
    blk->_first = nextof(blk->_first,blk->_bsz);
    --blk->_size;
    return res;
}

void m_free(Block* blk, void **ptr)
{
    if (*ptr == NULL) return;

    *(char**)((char*)(*ptr) + blk->_bsz ) = blk->_first;
    blk->_first = (*ptr);
    *ptr = NULL;
}

void    pool_release(Block *blk)
{
    int i = 0,sz = blk->_slots->_size;

    for( ; i<sz; ++i )
        free(Line_Get(blk->_slots,i));
    free(blk->_slots->_store);
    free(blk->_slots);
}
////////////////////////////////////Don't Use the Code Below////////////////////////////////////////////

NOUSE MPool   *mpool_create(int bsz, int count)
{
    MPool   *pool = (MPool*)malloc(sizeof(MPool));
    int     i = 0;

    pool->_store = (char **)malloc(sizeof(char*)*count);
    pool->_freeIndex = 0;
    pool->_bsz = bsz;
    pool->_slots = Linear_New();
    pool->_cap = count;

    Linear(pool->_slots,10);
    pool->_mem = malloc(bsz * count);
    Line_Append(pool->_slots,pool->_mem);

    for ( ; i < count; ++i)
    {
        pool->_store[i] = (char*)pool->_mem + i * bsz;
    }

    return pool;
}
NOUSE void   mpool_realse(MPool *pool)
{
    int i;

    for ( i = 0 ; i < pool->_slots->_size; ++i)
        free(Line_Get(pool->_slots,i));
    free(pool->_store);
}

NOUSE void    _extend(MPool *pool)
{
    int incsize = pool->_cap * 1.5, i = pool->_cap,j = 0;
    void *incmem = malloc(pool->_bsz * incsize);

    Line_Append(pool->_slots,incmem);
    pool->_store    = (char**)realloc(pool->_store,(pool->_cap + incsize)*sizeof(char*));
    assert(pool->_store != NULL);

    for ( ; j < incsize; ++j )
    {
        pool->_store[i + j] = (char*)incmem + j * pool->_bsz;
    }
    pool->_freeIndex = pool->_cap;
    pool->_cap += incsize;
}

NOUSE void *malloc2(MPool *pool)
{
    void *res = NULL;

    if ( pool->_freeIndex >= pool->_cap )
    {
        _extend(pool);
    }

    res = pool->_store[ pool->_freeIndex++ ];
    return res;
}

NOUSE void free2(MPool *pool, void *ptr)
{
    //STOP HERE
}
