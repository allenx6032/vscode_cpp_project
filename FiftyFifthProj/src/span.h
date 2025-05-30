// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_CONTAINERS_SPAN_H_
#define BASE_CONTAINERS_SPAN_H_

#include <stddef.h>
#include <stdint.h>

#include <algorithm>
#include <array>
#include <iterator>
#include <string_view>
#include <type_traits>
#include <utility>

#include "logging.h"
#include "stl_util.h"

namespace base {

// [views.constants]
constexpr size_t dynamic_extent = static_cast<size_t>(-1);

template <typename T, size_t Extent = dynamic_extent>
class span;

namespace internal {

template <typename T>
struct IsSpanImpl : std::false_type {};

template <typename T, size_t Extent>
struct IsSpanImpl<span<T, Extent>> : std::true_type {};

template <typename T>
using IsSpan = IsSpanImpl<std::decay_t<T>>;

template <typename T>
struct IsStdArrayImpl : std::false_type {};

template <typename T, size_t N>
struct IsStdArrayImpl<std::array<T, N>> : std::true_type {};

template <typename T>
using IsStdArray = IsStdArrayImpl<std::decay_t<T>>;

template <typename T>
using IsCArray = std::is_array<std::remove_reference_t<T>>;

template <typename From, typename To>
using IsLegalDataConversion = std::is_convertible<From (*)[], To (*)[]>;

template <typename Container, typename T>
using ContainerHasConvertibleData = IsLegalDataConversion<
    std::remove_pointer_t<decltype(std::data(std::declval<Container>()))>,
    T>;

template <typename Container>
using ContainerHasIntegralSize =
    std::is_integral<decltype(std::size(std::declval<Container>()))>;

template <typename From, size_t FromExtent, typename To, size_t ToExtent>
using EnableIfLegalSpanConversion =
    std::enable_if_t<(ToExtent == dynamic_extent || ToExtent == FromExtent) &&
                     IsLegalDataConversion<From, To>::value>;

// SFINAE check if Array can be converted to a span<T>.
template <typename Array, size_t N, typename T, size_t Extent>
using EnableIfSpanCompatibleArray =
    std::enable_if_t<(Extent == dynamic_extent || Extent == N) &&
                     ContainerHasConvertibleData<Array, T>::value>;

// SFINAE check if Container can be converted to a span<T>.
template <typename Container, typename T>
using EnableIfSpanCompatibleContainer =
    std::enable_if_t<!internal::IsSpan<Container>::value &&
                     !internal::IsStdArray<Container>::value &&
                     !internal::IsCArray<Container>::value &&
                     ContainerHasConvertibleData<Container, T>::value &&
                     ContainerHasIntegralSize<Container>::value>;

}  // namespace internal

// A span is a value type that represents an array of elements of type T. Since
// it only consists of a pointer to memory with an associated size, it is very
// light-weight. It is cheap to construct, copy, move and use spans, so that
// users are encouraged to use it as a pass-by-value parameter. A span does not
// own the underlying memory, so care must be taken to ensure that a span does
// not outlive the backing store.
//
// span is somewhat analogous to std::string_view, but with arbitrary element
// types, allowing mutation if T is non-const.
//
// span is implicitly convertible from C++ arrays, as well as most [1]
// container-like types that provide a data() and size() method (such as
// std::vector<T>). A mutable span<T> can also be implicitly converted to an
// immutable span<const T>.
//
// Consider using a span for functions that take a data pointer and size
// parameter: it allows the function to still act on an array-like type, while
// allowing the caller code to be a bit more concise.
//
// For read-only data access pass a span<const T>: the caller can supply either
// a span<const T> or a span<T>, while the callee will have a read-only view.
// For read-write access a mutable span<T> is required.
//
// Without span:
//   Read-Only:
//     // std::string HexEncode(const uint8_t* data, size_t size);
//     std::vector<uint8_t> data_buffer = GenerateData();
//     std::string r = HexEncode(data_buffer.data(), data_buffer.size());
//
//  Mutable:
//     // ssize_t SafeSNPrintf(char* buf, size_t N, const char* fmt, Args...);
//     char str_buffer[100];
//     SafeSNPrintf(str_buffer, sizeof(str_buffer), "Pi ~= %lf", 3.14);
//
// With span:
//   Read-Only:
//     // std::string HexEncode(base::span<const uint8_t> data);
//     std::vector<uint8_t> data_buffer = GenerateData();
//     std::string r = HexEncode(data_buffer);
//
//  Mutable:
//     // ssize_t SafeSNPrintf(base::span<char>, const char* fmt, Args...);
//     char str_buffer[100];
//     SafeSNPrintf(str_buffer, "Pi ~= %lf", 3.14);
//
// Spans with "const" and pointers
// -------------------------------
//
// Const and pointers can get confusing. Here are vectors of pointers and their
// corresponding spans:
//
//   const std::vector<int*>        =>  base::span<int* const>
//   std::vector<const int*>        =>  base::span<const int*>
//   const std::vector<const int*>  =>  base::span<const int* const>
//
// Differences from the working group proposal
// -------------------------------------------
//
// https://wg21.link/P0122 is the latest working group proposal, Chromium
// currently implements R7. Differences between the proposal and the
// implementation are documented in subsections below.
//
// Differences from [span.objectrep]:
// - as_bytes() and as_writable_bytes() return spans of uint8_t instead of
//   std::byte
//
// Differences in constants and types:
// - index_type is aliased to size_t
//
// Differences from [span.sub]:
// - using size_t instead of ptrdiff_t for indexing
//
// Differences from [span.obs]:
// - using size_t instead of ptrdiff_t to represent size()
//
// Differences from [span.elem]:
// - using size_t instead of ptrdiff_t for indexing
//
// Furthermore, all constructors and methods are marked noexcept due to the lack
// of exceptions in Chromium.
//
// Due to the lack of class template argument deduction guides in C++14
// appropriate make_span() utility functions are provided.

// [span], class template span
template <typename T, size_t Extent>
class span {
 public:
  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using index_type = size_t;
  using difference_type = ptrdiff_t;
  using pointer = T*;
  using reference = T&;
  using iterator = T*;
  using const_iterator = const T*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  static constexpr index_type extent = Extent;

  // [span.cons], span constructors, copy, assignment, and destructor
  constexpr span() noexcept : data_(nullptr), size_(0) {
    static_assert(Extent == dynamic_extent || Extent == 0, "Invalid Extent");
  }

  constexpr span(T* data, size_t size) noexcept : data_(data), size_(size) {
    CHECK(Extent == dynamic_extent || Extent == size);
  }

  // Artificially templatized to break ambiguity for span(ptr, 0).
  template <typename = void>
  constexpr span(T* begin, T* end) noexcept : span(begin, end - begin) {
    // Note: CHECK_LE is not constexpr, hence regular CHECK must be used.
    CHECK(begin <= end);
  }

  template <
      size_t N,
      typename = internal::EnableIfSpanCompatibleArray<T (&)[N], N, T, Extent>>
  constexpr span(T (&array)[N]) noexcept : span(std::data(array), N) {}

  template <
      size_t N,
      typename = internal::
          EnableIfSpanCompatibleArray<std::array<value_type, N>&, N, T, Extent>>
  constexpr span(std::array<value_type, N>& array) noexcept
      : span(std::data(array), N) {}

  template <size_t N,
            typename = internal::EnableIfSpanCompatibleArray<
                const std::array<value_type, N>&,
                N,
                T,
                Extent>>
  constexpr span(const std::array<value_type, N>& array) noexcept
      : span(std::data(array), N) {}

  // Conversion from a container that has compatible std::data() and integral
  // std::size().
  template <typename Container,
            typename = internal::EnableIfSpanCompatibleContainer<Container&, T>>
  constexpr span(Container& container) noexcept
      : span(std::data(container), std::size(container)) {}

  template <
      typename Container,
      typename = internal::EnableIfSpanCompatibleContainer<const Container&, T>>
  span(const Container& container) noexcept
      : span(std::data(container), std::size(container)) {}

  constexpr span(const span& other) noexcept = default;

  // Conversions from spans of compatible types and extents: this allows a
  // span<T> to be seamlessly used as a span<const T>, but not the other way
  // around. If extent is not dynamic, OtherExtent has to be equal to Extent.
  template <
      typename U,
      size_t OtherExtent,
      typename =
          internal::EnableIfLegalSpanConversion<U, OtherExtent, T, Extent>>
  constexpr span(const span<U, OtherExtent>& other)
      : span(other.data(), other.size()) {}

  constexpr span& operator=(const span& other) noexcept = default;
  ~span() noexcept = default;

  // [span.sub], span subviews
  template <size_t Count>
  constexpr span<T, Count> first() const noexcept {
    static_assert(Extent == dynamic_extent || Count <= Extent,
                  "Count must not exceed Extent");
    CHECK(Extent != dynamic_extent || Count <= size());
    return {data(), Count};
  }

  template <size_t Count>
  constexpr span<T, Count> last() const noexcept {
    static_assert(Extent == dynamic_extent || Count <= Extent,
                  "Count must not exceed Extent");
    CHECK(Extent != dynamic_extent || Count <= size());
    return {data() + (size() - Count), Count};
  }

  template <size_t Offset, size_t Count = dynamic_extent>
  constexpr span<T,
                 (Count != dynamic_extent
                      ? Count
                      : (Extent != dynamic_extent ? Extent - Offset
                                                  : dynamic_extent))>
  subspan() const noexcept {
    static_assert(Extent == dynamic_extent || Offset <= Extent,
                  "Offset must not exceed Extent");
    static_assert(Extent == dynamic_extent || Count == dynamic_extent ||
                      Count <= Extent - Offset,
                  "Count must not exceed Extent - Offset");
    CHECK(Extent != dynamic_extent || Offset <= size());
    CHECK(Extent != dynamic_extent || Count == dynamic_extent ||
          Count <= size() - Offset);
    return {data() + Offset, Count != dynamic_extent ? Count : size() - Offset};
  }

  constexpr span<T, dynamic_extent> first(size_t count) const noexcept {
    // Note: CHECK_LE is not constexpr, hence regular CHECK must be used.
    CHECK(count <= size());
    return {data(), count};
  }

  constexpr span<T, dynamic_extent> last(size_t count) const noexcept {
    // Note: CHECK_LE is not constexpr, hence regular CHECK must be used.
    CHECK(count <= size());
    return {data() + (size() - count), count};
  }

  constexpr span<T, dynamic_extent> subspan(
      size_t offset,
      size_t count = dynamic_extent) const noexcept {
    // Note: CHECK_LE is not constexpr, hence regular CHECK must be used.
    CHECK(offset <= size());
    CHECK(count == dynamic_extent || count <= size() - offset);
    return {data() + offset, count != dynamic_extent ? count : size() - offset};
  }

  // [span.obs], span observers
  constexpr size_t size() const noexcept { return size_; }
  constexpr size_t size_bytes() const noexcept { return size() * sizeof(T); }
  constexpr bool empty() const noexcept { return size() == 0; }

  // [span.elem], span element access
  constexpr T& operator[](size_t idx) const noexcept {
    // Note: CHECK_LT is not constexpr, hence regular CHECK must be used.
    CHECK(idx < size());
    return *(data() + idx);
  }

  constexpr T& operator()(size_t idx) const noexcept {
    // Note: CHECK_LT is not constexpr, hence regular CHECK must be used.
    CHECK(idx < size());
    return *(data() + idx);
  }

  constexpr T* data() const noexcept { return data_; }

  // [span.iter], span iterator support
  constexpr iterator begin() const noexcept { return data(); }
  constexpr iterator end() const noexcept { return data() + size(); }

  constexpr const_iterator cbegin() const noexcept { return begin(); }
  constexpr const_iterator cend() const noexcept { return end(); }

  constexpr reverse_iterator rbegin() const noexcept {
    return reverse_iterator(end());
  }
  constexpr reverse_iterator rend() const noexcept {
    return reverse_iterator(begin());
  }

  constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }
  constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(cbegin());
  }

 private:
  T* data_;
  size_t size_;
};

// span<T, Extent>::extent can not be declared inline prior to C++17, hence this
// definition is required.
template <class T, size_t Extent>
constexpr size_t span<T, Extent>::extent;

// [span.comparison], span comparison operators
// Relational operators. Equality is a element-wise comparison.
template <typename T, size_t X, typename U, size_t Y>
constexpr bool operator==(span<T, X> lhs, span<U, Y> rhs) noexcept {
  return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename T, size_t X, typename U, size_t Y>
constexpr bool operator!=(span<T, X> lhs, span<U, Y> rhs) noexcept {
  return !(lhs == rhs);
}

template <typename T, size_t X, typename U, size_t Y>
constexpr bool operator<(span<T, X> lhs, span<U, Y> rhs) noexcept {
  return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(),
                                      rhs.cend());
}

template <typename T, size_t X, typename U, size_t Y>
constexpr bool operator<=(span<T, X> lhs, span<U, Y> rhs) noexcept {
  return !(rhs < lhs);
}

template <typename T, size_t X, typename U, size_t Y>
constexpr bool operator>(span<T, X> lhs, span<U, Y> rhs) noexcept {
  return rhs < lhs;
}

template <typename T, size_t X, typename U, size_t Y>
constexpr bool operator>=(span<T, X> lhs, span<U, Y> rhs) noexcept {
  return !(lhs < rhs);
}

// [span.objectrep], views of object representation
template <typename T, size_t X>
span<const uint8_t, (X == dynamic_extent ? dynamic_extent : sizeof(T) * X)>
as_bytes(span<T, X> s) noexcept {
  return {reinterpret_cast<const uint8_t*>(s.data()), s.size_bytes()};
}

template <typename T,
          size_t X,
          typename = std::enable_if_t<!std::is_const<T>::value>>
span<uint8_t, (X == dynamic_extent ? dynamic_extent : sizeof(T) * X)>
as_writable_bytes(span<T, X> s) noexcept {
  return {reinterpret_cast<uint8_t*>(s.data()), s.size_bytes()};
}

// Type-deducing helpers for constructing a span.
template <typename T>
constexpr span<T> make_span(T* data, size_t size) noexcept {
  return {data, size};
}

template <typename T>
constexpr span<T> make_span(T* begin, T* end) noexcept {
  return {begin, end};
}

template <typename T, size_t N>
constexpr span<T, N> make_span(T (&array)[N]) noexcept {
  return array;
}

template <typename T, size_t N>
constexpr span<T, N> make_span(std::array<T, N>& array) noexcept {
  return array;
}

template <typename T, size_t N>
constexpr span<const T, N> make_span(const std::array<T, N>& array) noexcept {
  return array;
}

template <typename Container,
          typename T = typename Container::value_type,
          typename = internal::EnableIfSpanCompatibleContainer<Container&, T>>
constexpr span<T> make_span(Container& container) noexcept {
  return container;
}

template <
    typename Container,
    typename T = const typename Container::value_type,
    typename = internal::EnableIfSpanCompatibleContainer<const Container&, T>>
constexpr span<T> make_span(const Container& container) noexcept {
  return container;
}

template <typename T, size_t X>
constexpr span<T, X> make_span(const span<T, X>& span) noexcept {
  return span;
}

}  // namespace base

#endif  // BASE_CONTAINERS_SPAN_H_
