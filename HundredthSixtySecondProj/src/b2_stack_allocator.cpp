// Metadot physics engine is enhanced based on box2d modification
// Metadot code Copyright(c) 2022-2023, KaoruXun All rights reserved.
// Box2d code by Erin Catto licensed under the MIT License
// https://github.com/erincatto/box2d

// MIT License
// Copyright (c) 2022-2023 KaoruXun
// Copyright (c) 2019 Erin Catto

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "b2_stack_allocator.h"

#include "b2_math.h"

b2StackAllocator::b2StackAllocator() {
    m_index = 0;
    m_allocation = 0;
    m_maxAllocation = 0;
    m_entryCount = 0;
}

b2StackAllocator::~b2StackAllocator() {
    b2Assert(m_index == 0);
    b2Assert(m_entryCount == 0);
}

void* b2StackAllocator::Allocate(int32 size) {
    b2Assert(m_entryCount < b2_maxStackEntries);
    const int32 roundedSize = (size + ALIGN_MASK) & ~ALIGN_MASK;
    b2StackEntry* entry = m_entries + m_entryCount;
    entry->size = roundedSize;
    if (m_index + roundedSize > b2_stackSize) {
        entry->data = (char*)b2Alloc(roundedSize);
        entry->usedMalloc = true;
    } else {
        entry->data = m_data + m_index;
        entry->usedMalloc = false;
        m_index += roundedSize;
    }

    m_allocation += roundedSize;
    m_maxAllocation = b2Max(m_maxAllocation, m_allocation);
    ++m_entryCount;

    return entry->data;
}

void* b2StackAllocator::Reallocate(void* p, int32 size) {
    b2Assert(m_entryCount > 0);
    b2StackEntry* entry = m_entries + m_entryCount - 1;
    b2Assert(p == entry->data);

    int32 incrementSize = size - entry->size;
    if (incrementSize > 0) {
        if (entry->usedMalloc) {
            void* data = b2Alloc(size);
            memcpy(data, entry->data, entry->size);
            b2Free(entry->data);
            entry->data = (char*)data;
        } else if (m_index + incrementSize > b2_stackSize) {
            void* data = b2Alloc(size);
            memcpy(data, entry->data, entry->size);
            m_index -= entry->size;
            entry->data = (char*)data;
            entry->usedMalloc = true;
        } else {
            m_index += incrementSize;
            m_allocation += incrementSize;
            m_maxAllocation = b2Max(m_maxAllocation, m_allocation);
        }
        entry->size = size;
    }

    return entry->data;
}

void b2StackAllocator::Free(void* p) {
    b2Assert(m_entryCount > 0);
    b2StackEntry* entry = m_entries + m_entryCount - 1;
    b2Assert(p == entry->data);
    if (entry->usedMalloc) {
        b2Free(p);
    } else {
        m_index -= entry->size;
    }
    m_allocation -= entry->size;
    --m_entryCount;

    p = nullptr;
}

int32 b2StackAllocator::GetMaxAllocation() const { return m_maxAllocation; }
