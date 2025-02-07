#ifndef UNITY_BUILD
 #pragma once
 #include "p2d_globals.h"
 #include <stdio.h>
 #include <string.h>
 #include <stdbool.h>
#endif

typedef struct 
{
    uint8 *data;
    int32 size;
} Buffer;
typedef Buffer String;

#define BundleStringC(s) string_Make((uint8 *)s, strlen(s))
#define BundleString(s) string_Make(s, strlen((char *)s))
inline function String
string_Make(uint8 *address, int32 size)
{
    String newString;
    newString.data = address;
    newString.size = size;
    return newString;
}

inline function void
string_ToChar(String *s, char* c)
{
    snprintf(c, s->size + 1, "%s", s->data);
}

inline internal uint32
common_GenerateHash(void *memoryBlock, uint32 sizeInBytes)
{
    uint8 *Ptr = (uint8 *) memoryBlock;
    uint32 Result = 0;
    for(int i = 0; i < sizeInBytes; ++i) {
		Result = Ptr[i] + (Result * 31);
	}
    return Result;
}

inline function bool
utils_IsLittleEndian()
{
    int32 One = 1;
    return (*((uint8*)&One) == 1);
}

#define ChangeEndianness16(n) n<<8 | n >>8;
#define ChangeEndianness32(n) ((n>>24)&0xff) | ((n<<8)&0xff0000) | ((n>>8)&0xff00) | ((n<<24)&0xff000000)

#define UNLIKELY(x) __builtin_expect((bool)x, 0)
#define Max(a,b) a < b ? b : a
#define Mod(a,b) a >= b ? a % b : a
#define UnlikelyMod(a,b) UNLIKELY((a >= b)) ? a % b : a
