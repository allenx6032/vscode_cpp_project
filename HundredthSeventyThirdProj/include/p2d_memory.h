#ifndef UNITY_BUILD
 #pragma once
 #include <string.h>
 #include <stdlib.h>
 #include <assert.h>
 #include "p2d_common.h"
#endif

#define ARENA_SIZE_IN_BYTES Megabytes(100)
#define LOG_SIZE_IN_BYTES Megabytes(90)

//NOTE(pipecaniza): right now the arena is not capable to free resources(linear allocator) to avoid fragmentation
struct
{
    uint8* memoryArena;
    uint32 bufferSize;
} typedef Arena;

function Arena
MakeArena()
{
    Arena result = {};
    result.memoryArena = (uint8*)calloc(1, ARENA_SIZE_IN_BYTES);
    return result;
}

function uint8*
MakeLog()
{
    return (uint8*)calloc(1, LOG_SIZE_IN_BYTES);
}

function uint8*
PushToMemory(Arena* arena, void* ptr, uint32 sizeInBytes)
{
    assert(arena->bufferSize + sizeInBytes < ARENA_SIZE_IN_BYTES);

    uint8* bytePtr = (uint8*)ptr;
    uint8* baseAddress = arena->memoryArena + arena->bufferSize;
    for (uint32 i = 0; i < sizeInBytes; ++i) {
        uint8* currentByte = bytePtr + i;
        arena->memoryArena[arena->bufferSize++] = *currentByte;
    }
    return baseAddress;
}


function void
PushToMemoryAtLocation(Arena* arena, uint8* arenaPointer, void* ptr, uint32 sizeInBytes)
{
    // NOTE(pipecaniza): Check that ArenaPointer is inside the arena memory
    assert((arenaPointer > arena->memoryArena && arenaPointer < arena->memoryArena + ARENA_SIZE_IN_BYTES) ||
           (arenaPointer < arena->memoryArena && arenaPointer > arena->memoryArena + ARENA_SIZE_IN_BYTES));
    assert(arena->bufferSize + sizeInBytes < ARENA_SIZE_IN_BYTES);

    uint8* BytePtr = (uint8*)ptr;
    uint8* BaseAddress = arenaPointer;
    for (uint32 i = 0; i < sizeInBytes; ++i) {
        uint8* CurrentByte = BytePtr + i;
        *(BaseAddress + i) = *CurrentByte;
    }
}


function uint8*
ReserveMemory(Arena* arena, uint32 sizeInBytes)
{
    assert(arena->bufferSize + sizeInBytes < ARENA_SIZE_IN_BYTES);
    uint8* baseAddress = arena->memoryArena + arena->bufferSize;
    arena->bufferSize += sizeInBytes;
    return baseAddress;
}


inline function String
AllocateString(Arena* arena, char* data)
{
    // Add +1 to store '\0'
    uint8* Address = PushToMemory(arena, data, strlen(data ) + 1);
    return BundleString(Address);
}

function void
ResetArena(Arena* arena) {
    memset(arena->memoryArena, 0, ARENA_SIZE_IN_BYTES);
    arena->bufferSize = 0;
}
