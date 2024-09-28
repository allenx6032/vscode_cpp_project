/*
 *                 GNU GENERAL PUBLIC LICENSE
 *                  Version 3, 29 June 2007
 *                 Copyright (C) 2016 XD.Yang
 *   Everyone is permitted to copy and distribute verbatim copies
 *   of this license document, but changing it is not allowed.
 */

#ifndef MPOOL_H
#define MPOOL_H

#ifdef  __cplusplus
extern "C"{
#endif


#include <string.h>
#include <stdlib.h>

#include "m_vector.h"
//////////////////////////////////Linked-List///////////////////////////////////

#define                 MemPool     Block

typedef struct
{
    void        *_first;
    int         _cap;
    int         _bsz;
    int         _size;
    LineTable   *_slots;
}Block;

Block*  pool_create(int bsz, int count);
void*   m_malloc(Block* blk);
void    m_free(Block* blk, void **ptr);
void    pool_release(Block *blk);



//////////////////////////////////Array///////////////////////////////////

typedef struct
{
    void        *_mem;           //实际分配的存储区
    char        **_store;
    int         _freeIndex;
    int         _bsz;         //块的尺寸
    int         _cap;         //一个区内的总块数
    LineTable   *_slots;    //内存区块指针
}MPool;


MPool   *mpool_create(int bsz, int count);
void    *malloc2(MPool *pool);
void    free2(MPool *pool, void *ptr);
void    mpool_realse(MPool *pool);

/////////////////////////////////////////////////////////////////////
#ifdef  __cplusplus
}
#endif

#endif // MPOOL_H
