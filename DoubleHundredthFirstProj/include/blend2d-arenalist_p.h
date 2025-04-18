// This file is part of Blend2D project <https://blend2d.com>
//
// See blend2d.h or LICENSE.md for license and copyright information
// SPDX-License-Identifier: Zlib

#ifndef BLEND2D_SUPPORT_ARENALIST_P_H_INCLUDED
#define BLEND2D_SUPPORT_ARENALIST_P_H_INCLUDED

#include "blend2d-api-internal_p.h"

//! \cond INTERNAL
//! \addtogroup blend2d_internal
//! \{

namespace bl {

//! \name Arena Allocated ArenaList
//! \{

//! Zone-allocated double-linked list node.
template<typename NodeT>
class ArenaListNode {
public:
  union {
    NodeT* _listNodes[2];
    struct {
      NodeT* _listPrev;
      NodeT* _listNext;
    };
  };

  BL_NONCOPYABLE(ArenaListNode)

  BL_INLINE ArenaListNode() noexcept
    : _listNodes { nullptr, nullptr } {}

  BL_INLINE ArenaListNode(ArenaListNode&& other) noexcept
    : _listNodes { other._listNodes[0], other._listNodes[1] } {}

  BL_INLINE bool hasPrev() const noexcept { return _listPrev != nullptr; }
  BL_INLINE bool hasNext() const noexcept { return _listNext != nullptr; }

  BL_INLINE NodeT* prev() const noexcept { return _listPrev; }
  BL_INLINE NodeT* next() const noexcept { return _listNext; }
};

//! Zone-allocated double-linked list container.
template <typename NodeT>
class ArenaList {
public:
  BL_NONCOPYABLE(ArenaList)

  NodeT* _nodes[2];

  //! \name Construction & Destruction
  //! \{

  BL_INLINE ArenaList() noexcept
    : _nodes { nullptr, nullptr } {}

  BL_INLINE ArenaList(ArenaList&& other) noexcept
    : _nodes { other._nodes[0], other._nodes[1] } {}

  //! \}

  //! \name Common Functionality
  //! \{

  BL_INLINE void swap(ArenaList& other) noexcept {
    BLInternal::swap(_nodes[0], other._nodes[0]);
    BLInternal::swap(_nodes[1], other._nodes[1]);
  }

  BL_INLINE void reset() noexcept {
    _nodes[0] = nullptr;
    _nodes[1] = nullptr;
  }

  BL_INLINE void reset(NodeT* node) noexcept {
    node->_listNodes[0] = nullptr;
    node->_listNodes[1] = nullptr;
    _nodes[0] = node;
    _nodes[1] = node;
  }

  //! \}

  //! \name ArenaList Functionality
  //! \{

  // Can be used to both prepend and append.
  BL_INLINE void _addNode(NodeT* node, size_t dir) noexcept {
    NodeT* prev = _nodes[dir];

    node->_listNodes[!dir] = prev;
    _nodes[dir] = node;
    if (prev)
      prev->_listNodes[dir] = node;
    else
      _nodes[!dir] = node;
  }

  // Can be used to both prepend and append.
  BL_INLINE void _insertNode(NodeT* ref, NodeT* node, size_t dir) noexcept {
    BL_ASSERT(ref != nullptr);

    NodeT* prev = ref;
    NodeT* next = ref->_listNodes[dir];

    prev->_listNodes[dir] = node;
    if (next)
      next->_listNodes[!dir] = node;
    else
      _nodes[dir] = node;

    node->_listNodes[!dir] = prev;
    node->_listNodes[ dir] = next;
  }

  BL_INLINE void append(NodeT* node) noexcept { _addNode(node, 1); }
  BL_INLINE void prepend(NodeT* node) noexcept { _addNode(node, 0); }

  BL_INLINE void insertAfter(NodeT* ref, NodeT* node) noexcept { _insertNode(ref, node, 1); }
  BL_INLINE void insertBefore(NodeT* ref, NodeT* node) noexcept { _insertNode(ref, node, 0); }

  BL_INLINE NodeT* unlink(NodeT* node) noexcept {
    NodeT* prev = node->prev();
    NodeT* next = node->next();

    if (prev) { prev->_listNext = next; } else { _nodes[0] = next; }
    if (next) { next->_listPrev = prev; } else { _nodes[1] = prev; }

    node->_listPrev = nullptr;
    node->_listNext = nullptr;

    return node;
  }

  BL_INLINE NodeT* popFirst() noexcept {
    NodeT* node = _nodes[0];
    BL_ASSERT(node != nullptr);

    NodeT* next = node->next();
    _nodes[0] = next;

    if (next) {
      next->_listPrev = nullptr;
      node->_listNext = nullptr;
    }
    else {
      _nodes[1] = nullptr;
    }

    return node;
  }

  BL_INLINE NodeT* pop() noexcept {
    NodeT* node = _nodes[1];
    BL_ASSERT(node != nullptr);

    NodeT* prev = node->prev();
    _nodes[1] = prev;

    if (prev) {
      prev->_listNext = nullptr;
      node->_listPrev = nullptr;
    }
    else {
      _nodes[0] = nullptr;
    }

    return node;
  }

  //! \}

  //! \name Accessors
  //! \{

  BL_INLINE bool empty() const noexcept { return _nodes[0] == nullptr; }
  BL_INLINE NodeT* first() const noexcept { return _nodes[0]; }
  BL_INLINE NodeT* last() const noexcept { return _nodes[1]; }

  //! \}
};

//! \}

} // {bl}

//! \}
//! \endcond

#endif // BLEND2D_SUPPORT_ARENALIST_P_H_INCLUDED
