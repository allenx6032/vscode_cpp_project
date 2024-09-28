/*
 *                 GNU GENERAL PUBLIC LICENSE
 *                  Version 3, 29 June 2007
 *                 Copyright (C) 2016 XD.Yang
 *   Everyone is permitted to copy and distribute verbatim copies
 *   of this license document, but changing it is not allowed.
 */
#ifndef M_VECTOR_H
#define M_VECTOR_H

#include <stdlib.h>
#include <assert.h>


typedef struct
{
    void    **_store;
    int     _size;
    int     _cap;
}LineTable;

#define     Linear_New() (LineTable*)malloc(sizeof(LineTable))

#define     Linear(LT,SZ)  \
{\
    LT->_cap = SZ; LT->_size = 0;LT->_store = (void**)calloc(SZ,sizeof(void*));\
}

#define     Line_Append(LT,V)\
{\
    if ( LT->_size >= LT->_cap )\
        _Extend(LT);\
    LT->_store[ LT->_size++] = V;\
}

#define     _Extend(LT) \
{\
    int _cap = LT->_cap * 3;\
    LT->_store = (void**)realloc(LT->_store,sizeof(void**)*_cap);\
    assert(LT->_store != NULL);\
    LT->_cap = _cap;\
}

#define     Line_Free(LT)   \
{free(LT->_store);}

#define     Line_Get(LT,I)      (LT->_store[I])

#define     Line_Set(LT,I,V)    (LT->_store[I] = V)

#endif // M_VECTOR_H
