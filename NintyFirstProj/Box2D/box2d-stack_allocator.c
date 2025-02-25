// SPDX-FileCopyrightText: 2023 Erin Catto
// SPDX-License-Identifier: MIT

#include "box2d-stack_allocator.h"

#include "box2d-allocate.h"
#include "box2d-array.h"
#include "box2d-core.h"

#include <stdbool.h>

typedef struct b2StackEntry
{
	char* data;
	const char* name;
	int size;
	bool usedMalloc;
} b2StackEntry;

b2StackAllocator b2CreateStackAllocator( int capacity )
{
	B2_ASSERT( capacity >= 0 );
	b2StackAllocator allocator = { 0 };
	allocator.capacity = capacity;
	allocator.data = b2Alloc( capacity );
	allocator.allocation = 0;
	allocator.maxAllocation = 0;
	allocator.index = 0;
	allocator.entries = b2CreateArray( sizeof( b2StackEntry ), 32 );
	return allocator;
}

void b2DestroyStackAllocator( b2StackAllocator* allocator )
{
	b2DestroyArray( allocator->entries, sizeof( b2StackEntry ) );
	b2Free( allocator->data, allocator->capacity );
}

void* b2AllocateStackItem( b2StackAllocator* alloc, int size, const char* name )
{
	// ensure allocation is 32 byte aligned to support 256-bit SIMD
	int size32 = ( ( size - 1 ) | 0x1F ) + 1;

	b2StackEntry entry;
	entry.size = size32;
	entry.name = name;
	if ( alloc->index + size32 > alloc->capacity )
	{
		// fall back to the heap (undesirable)
		entry.data = b2Alloc( size32 );
		entry.usedMalloc = true;

		B2_ASSERT( ( (uintptr_t)entry.data & 0x1F ) == 0 );
	}
	else
	{
		entry.data = alloc->data + alloc->index;
		entry.usedMalloc = false;
		alloc->index += size32;

		B2_ASSERT( ( (uintptr_t)entry.data & 0x1F ) == 0 );
	}

	alloc->allocation += size32;
	if ( alloc->allocation > alloc->maxAllocation )
	{
		alloc->maxAllocation = alloc->allocation;
	}

	b2Array_Push( alloc->entries, entry );
	return entry.data;
}

void b2FreeStackItem( b2StackAllocator* alloc, void* mem )
{
	int entryCount = b2Array( alloc->entries ).count;
	B2_ASSERT( entryCount > 0 );
	b2StackEntry* entry = alloc->entries + ( entryCount - 1 );
	B2_ASSERT( mem == entry->data );
	if ( entry->usedMalloc )
	{
		b2Free( mem, entry->size );
	}
	else
	{
		alloc->index -= entry->size;
	}
	alloc->allocation -= entry->size;
	b2Array_Pop( alloc->entries );
}

void b2GrowStack( b2StackAllocator* alloc )
{
	// Stack must not be in use
	B2_ASSERT( alloc->allocation == 0 );

	if ( alloc->maxAllocation > alloc->capacity )
	{
		b2Free( alloc->data, alloc->capacity );
		alloc->capacity = alloc->maxAllocation + alloc->maxAllocation / 2;
		alloc->data = b2Alloc( alloc->capacity );
	}
}

int b2GetStackCapacity( b2StackAllocator* alloc )
{
	return alloc->capacity;
}

int b2GetStackAllocation( b2StackAllocator* alloc )
{
	return alloc->allocation;
}

int b2GetMaxStackAllocation( b2StackAllocator* alloc )
{
	return alloc->maxAllocation;
}
