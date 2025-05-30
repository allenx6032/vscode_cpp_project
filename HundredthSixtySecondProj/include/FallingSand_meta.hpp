// Copyright(c) 2022-2023, KaoruXun

// This source file include
// https://github.com/blackmatov/meta.hpp (MIT) by Matvey Cherevko

#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <climits>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <exception>
#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include <version>

#include "FallingSand_exception.hpp"

#if !defined(META_HPP_NO_RTTI) && !defined(__cpp_rtti)
#define META_HPP_NO_RTTI
#endif

#if !defined(META_HPP_FWD)
#define META_HPP_FWD(v) std::forward<decltype(v)>(v)
#endif

#if !defined(META_HPP_ASSERT)
#define META_HPP_ASSERT(...) assert(__VA_ARGS__)  // NOLINT
#endif

#if !defined(META_HPP_DEV_ASSERT)
#if defined(META_HPP_SANITIZERS)
#define META_HPP_DEV_ASSERT(...) META_HPP_ASSERT(__VA_ARGS__)
#else
#define META_HPP_DEV_ASSERT(...) (void)0
#endif
#endif

//
//
//

#define META_HPP_CLANG_COMPILER_ID 1
#define META_HPP_GCC_COMPILER_ID 2
#define META_HPP_MSVC_COMPILER_ID 3
#define META_HPP_UNKNOWN_COMPILER_ID 4

#if defined(__clang__)
#define META_HPP_COMPILER_ID META_HPP_CLANG_COMPILER_ID
#elif defined(__GNUC__)
#define META_HPP_COMPILER_ID META_HPP_GCC_COMPILER_ID
#elif defined(_MSC_VER)
#define META_HPP_COMPILER_ID META_HPP_MSVC_COMPILER_ID
#else
#define META_HPP_COMPILER_ID META_HPP_UNKNOWN_COMPILER_ID
#endif

//
//
//

#if META_HPP_COMPILER_ID == META_HPP_CLANG_COMPILER_ID
#define META_HPP_CLANG_PRAGMA_TO_STR(x) _Pragma(#x)
#define META_HPP_CLANG_IGNORE_WARNING(w) META_HPP_CLANG_PRAGMA_TO_STR(clang diagnostic ignored w)
#define META_HPP_CLANG_IGNORE_WARNINGS_PUSH() _Pragma("clang diagnostic push")
#define META_HPP_CLANG_IGNORE_WARNINGS_POP() _Pragma("clang diagnostic pop")
#else
#define META_HPP_CLANG_PRAGMA_TO_STR(x)
#define META_HPP_CLANG_IGNORE_WARNING(w)
#define META_HPP_CLANG_IGNORE_WARNINGS_PUSH()
#define META_HPP_CLANG_IGNORE_WARNINGS_POP()
#endif

#if META_HPP_COMPILER_ID == META_HPP_GCC_COMPILER_ID
#define META_HPP_GCC_PRAGMA_TO_STR(x) _Pragma(#x)
#define META_HPP_GCC_IGNORE_WARNING(w) META_HPP_GCC_PRAGMA_TO_STR(GCC diagnostic ignored w)
#define META_HPP_GCC_IGNORE_WARNINGS_PUSH() _Pragma("GCC diagnostic push")
#define META_HPP_GCC_IGNORE_WARNINGS_POP() _Pragma("GCC diagnostic pop")
#else
#define META_HPP_GCC_PRAGMA_TO_STR(x)
#define META_HPP_GCC_IGNORE_WARNING(w)
#define META_HPP_GCC_IGNORE_WARNINGS_PUSH()
#define META_HPP_GCC_IGNORE_WARNINGS_POP()
#endif

#if META_HPP_COMPILER_ID == META_HPP_MSVC_COMPILER_ID
#define META_HPP_MSVC_IGNORE_WARNING(w) __pragma(warning(disable : w))
#define META_HPP_MSVC_IGNORE_WARNINGS_PUSH() __pragma(warning(push))
#define META_HPP_MSVC_IGNORE_WARNINGS_POP() __pragma(warning(pop))
#else
#define META_HPP_MSVC_IGNORE_WARNING(w)
#define META_HPP_MSVC_IGNORE_WARNINGS_PUSH()
#define META_HPP_MSVC_IGNORE_WARNINGS_POP()
#endif

//
//
//

#define META_HPP_IGNORE_OVERRIDE_WARNINGS_PUSH()                     \
    META_HPP_CLANG_IGNORE_WARNINGS_PUSH()                            \
    META_HPP_CLANG_IGNORE_WARNING("-Wunknown-warning-option")        \
    META_HPP_CLANG_IGNORE_WARNING("-Winconsistent-missing-override") \
    META_HPP_CLANG_IGNORE_WARNING("-Wsuggest-override")

#define META_HPP_IGNORE_OVERRIDE_WARNINGS_POP() META_HPP_CLANG_IGNORE_WARNINGS_POP()

namespace ME::meta::r::detail {
template <typename Enum>
class bitflags final {
    static_assert(std::is_enum_v<Enum>);

public:
    using enum_type = Enum;
    using underlying_type = std::underlying_type_t<Enum>;

    bitflags() = default;
    bitflags(const bitflags&) = default;
    bitflags& operator=(const bitflags&) = default;
    bitflags(bitflags&&) noexcept = default;
    bitflags& operator=(bitflags&&) noexcept = default;
    ~bitflags() = default;

    constexpr bitflags(enum_type flags) : flags_(static_cast<underlying_type>(flags)) {}

    constexpr explicit bitflags(underlying_type flags) : flags_(flags) {}

    constexpr void swap(bitflags& other) noexcept {
        using std::swap;
        swap(flags_, other.flags_);
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept { return !!flags_; }

    [[nodiscard]] constexpr underlying_type as_raw() const noexcept { return flags_; }

    [[nodiscard]] constexpr enum_type as_enum() const noexcept { return static_cast<enum_type>(flags_); }

    [[nodiscard]] constexpr bool has(bitflags flags) const noexcept { return flags.flags_ == (flags_ & flags.flags_); }

    constexpr bitflags& set(bitflags flags) noexcept {
        flags_ |= flags.flags_;
        return *this;
    }

    constexpr bitflags& toggle(bitflags flags) noexcept {
        flags_ ^= flags.flags_;
        return *this;
    }

    constexpr bitflags& clear(bitflags flags) noexcept {
        flags_ &= ~flags.flags_;
        return *this;
    }

private:
    underlying_type flags_{};
};

template <typename Enum>
constexpr void swap(bitflags<Enum>& l, bitflags<Enum>& r) noexcept {
    l.swap(r);
}
}  // namespace ME::meta::r::detail

namespace std {
template <typename Enum>
struct hash<ME::meta::r::detail::bitflags<Enum>> {
    size_t operator()(ME::meta::r::detail::bitflags<Enum> bf) const noexcept { return hash<Enum>{}(bf.as_enum()); }
};
}  // namespace std

namespace ME::meta::r::detail {
#define META_HPP_DEFINE_BINARY_OPERATOR(op)                                                 \
    template <typename Enum>                                                                \
    constexpr bool operator op(Enum l, bitflags<Enum> r) noexcept {                         \
        return l op r.as_enum();                                                            \
    }                                                                                       \
    template <typename Enum>                                                                \
    constexpr bool operator op(bitflags<Enum> l, Enum r) noexcept {                         \
        return l.as_enum() op r;                                                            \
    }                                                                                       \
    template <typename Enum>                                                                \
    constexpr bool operator op(std::underlying_type_t<Enum> l, bitflags<Enum> r) noexcept { \
        return l op r.as_raw();                                                             \
    }                                                                                       \
    template <typename Enum>                                                                \
    constexpr bool operator op(bitflags<Enum> l, std::underlying_type_t<Enum> r) noexcept { \
        return l.as_raw() op r;                                                             \
    }                                                                                       \
    template <typename Enum>                                                                \
    constexpr bool operator op(bitflags<Enum> l, bitflags<Enum> r) noexcept {               \
        return l.as_raw() op r.as_raw();                                                    \
    }
META_HPP_DEFINE_BINARY_OPERATOR(<)
META_HPP_DEFINE_BINARY_OPERATOR(>)
META_HPP_DEFINE_BINARY_OPERATOR(<=)
META_HPP_DEFINE_BINARY_OPERATOR(>=)
META_HPP_DEFINE_BINARY_OPERATOR(==)
META_HPP_DEFINE_BINARY_OPERATOR(!=)
#undef META_HPP_DEFINE_BINARY_OPERATOR
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename Enum>
constexpr bitflags<Enum> operator~(bitflags<Enum> l) noexcept {
    return static_cast<Enum>(~l.as_raw());
}

#define META_HPP_DEFINE_BINARY_OPERATOR(op)                                                  \
    template <typename Enum>                                                                 \
    constexpr bitflags<Enum> operator op(Enum l, bitflags<Enum> r) noexcept {                \
        return bitflags{l} op r;                                                             \
    }                                                                                        \
    template <typename Enum>                                                                 \
    constexpr bitflags<Enum> operator op(bitflags<Enum> l, Enum r) noexcept {                \
        return l op bitflags<Enum>{r};                                                       \
    }                                                                                        \
    template <typename Enum>                                                                 \
    constexpr bitflags<Enum> operator op(bitflags<Enum> l, bitflags<Enum> r) noexcept {      \
        return static_cast<Enum>(l.as_raw() op r.as_raw());                                  \
    }                                                                                        \
    template <typename Enum>                                                                 \
    constexpr bitflags<Enum>& operator op##=(bitflags<Enum>& l, Enum r) noexcept {           \
        return l = l op bitflags<Enum>{r};                                                   \
    }                                                                                        \
    template <typename Enum>                                                                 \
    constexpr bitflags<Enum>& operator op##=(bitflags<Enum>& l, bitflags<Enum> r) noexcept { \
        return l = l op r;                                                                   \
    }
META_HPP_DEFINE_BINARY_OPERATOR(|)
META_HPP_DEFINE_BINARY_OPERATOR(&)
META_HPP_DEFINE_BINARY_OPERATOR(^)
#undef META_HPP_DEFINE_BINARY_OPERATOR
}  // namespace ME::meta::r::detail

//
// META_HPP_BITFLAGS_OPERATORS_DECL
//

#define META_HPP_BITFLAGS_OPERATORS_DECL(Enum)                                                                                                        \
    constexpr ::ME::meta::r::detail::bitflags<Enum> operator~[[maybe_unused]] (Enum l) noexcept { return ~::ME::meta::r::detail::bitflags<Enum>(l); } \
    constexpr ::ME::meta::r::detail::bitflags<Enum> operator| [[maybe_unused]] (Enum l, Enum r) noexcept {                                            \
        return ::ME::meta::r::detail::bitflags<Enum>(l) | ::ME::meta::r::detail::bitflags<Enum>(r);                                                   \
    }                                                                                                                                                 \
    constexpr ::ME::meta::r::detail::bitflags<Enum> operator& [[maybe_unused]] (Enum l, Enum r) noexcept {                                            \
        return ::ME::meta::r::detail::bitflags<Enum>(l) & ::ME::meta::r::detail::bitflags<Enum>(r);                                                   \
    }                                                                                                                                                 \
    constexpr ::ME::meta::r::detail::bitflags<Enum> operator^ [[maybe_unused]] (Enum l, Enum r) noexcept { return ::ME::meta::r::detail::bitflags<Enum>(l) ^ ::ME::meta::r::detail::bitflags<Enum>(r); }

namespace ME::meta::r::detail {
template <typename From>
struct cv_traits {
    static constexpr bool is_const = std::is_const_v<std::remove_reference_t<From>>;
    static constexpr bool is_volatile = std::is_volatile_v<std::remove_reference_t<From>>;

    template <bool yesno, template <typename> typename Q, typename V>
    using apply_t_if = std::conditional_t<yesno, Q<V>, V>;

    // clang-format off
        template < typename To >
        using add_to =
            apply_t_if<is_const, std::add_const_t,
            apply_t_if<is_volatile, std::add_volatile_t,
            To>>;
    // clang-format on

    template <typename To>
    using copy_to = add_to<std::remove_cv_t<To>>;
};

template <typename From, typename To>
struct add_cv {
    using type = typename cv_traits<From>::template add_to<To>;
};

template <typename From, typename To>
struct copy_cv {
    using type = typename cv_traits<From>::template copy_to<To>;
};

template <typename From, typename To>
using add_cv_t = typename add_cv<From, To>::type;

template <typename From, typename To>
using copy_cv_t = typename copy_cv<From, To>::type;
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
[[noreturn]] inline void throw_exception(::ME::exception::error_code err) { throw ::ME::exception::exception_meta{err}; }
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <typename From>
struct cvref_traits {
    static constexpr bool is_lvalue = std::is_lvalue_reference_v<From>;
    static constexpr bool is_rvalue = std::is_rvalue_reference_v<From>;
    static constexpr bool is_const = std::is_const_v<std::remove_reference_t<From>>;
    static constexpr bool is_volatile = std::is_volatile_v<std::remove_reference_t<From>>;

    template <bool yesno, template <typename> typename Q, typename V>
    using apply_t_if = std::conditional_t<yesno, Q<V>, V>;

    // clang-format off
        template < typename To >
        using add_to =
            apply_t_if<is_lvalue, std::add_lvalue_reference_t,
            apply_t_if<is_rvalue, std::add_rvalue_reference_t,
            apply_t_if<is_const, std::add_const_t,
            apply_t_if<is_volatile, std::add_volatile_t,
            To>>>>;
    // clang-format on

    template <typename To>
    using copy_to = add_to<std::remove_cvref_t<To>>;
};

template <typename From, typename To>
struct add_cvref {
    using type = typename cvref_traits<From>::template add_to<To>;
};

template <typename From, typename To>
struct copy_cvref {
    using type = typename cvref_traits<From>::template copy_to<To>;
};

template <typename From, typename To>
using add_cvref_t = typename add_cvref<From, To>::type;

template <typename From, typename To>
using copy_cvref_t = typename copy_cvref<From, To>::type;

}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename Function>
class fixed_function;

template <typename R, typename... Args>
class fixed_function<R(Args...)> final {
public:
    using result_type = R;

    fixed_function() = default;

    ~fixed_function() { reset(); }

    fixed_function(const fixed_function& other) = delete;
    fixed_function& operator=(const fixed_function& other) = delete;

    fixed_function(fixed_function&& other) noexcept {
        if (other.vtable_) {
            other.vtable_->move(other, *this);
        }
    }

    fixed_function& operator=(fixed_function&& other) noexcept {
        if (this != &other) {
            fixed_function{std::move(other)}.swap(*this);
        }
        return *this;
    }

    template <                                             //
            typename F,                                    //
            typename Fp = std::decay_t<F>,                 //
            typename = std::enable_if_t<                   //
                    !std::is_same_v<Fp, fixed_function>>>  //
    fixed_function(F&& fun) {
        vtable_t::construct(*this, std::forward<F>(fun));
    }

    template <                                             //
            typename F,                                    //
            typename Fp = std::decay_t<F>,                 //
            typename = std::enable_if_t<                   //
                    !std::is_same_v<Fp, fixed_function>>>  //
    fixed_function& operator=(F&& fun) {
        fixed_function{std::forward<F>(fun)}.swap(*this);
        return *this;
    }

    [[nodiscard]] bool is_valid() const noexcept { return vtable_ != nullptr; }

    [[nodiscard]] explicit operator bool() const noexcept { return is_valid(); }

    R operator()(Args... args) const {
        META_HPP_ASSERT(vtable_ && "bad function call");
        return vtable_->call(*this, std::forward<Args>(args)...);
    }

    void reset() noexcept {
        if (vtable_) {
            vtable_->destroy(*this);
        }
    }

    void swap(fixed_function& other) noexcept { vtable_t::swap(*this, other); }

private:
    struct vtable_t;
    vtable_t* vtable_{};

private:
    struct buffer_t final {
        // NOLINTNEXTLINE(*-avoid-c-arrays)
        alignas(std::max_align_t) std::byte data[sizeof(void*) * 3];
    } buffer_{};
};

template <typename Function>
void swap(fixed_function<Function>& l, fixed_function<Function>& r) noexcept {
    l.swap(r);
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename R, typename... Args>
struct fixed_function<R(Args...)>::vtable_t final {
    // NOLINTBEGIN(*-avoid-const-or-ref-data-members)

    R (*const call)(const fixed_function& self, Args... args);
    void (*const move)(fixed_function& from, fixed_function& to) noexcept;
    void (*const destroy)(fixed_function& self);

    // NOLINTEND(*-avoid-const-or-ref-data-members)

    template <typename T>
    static T* buffer_cast(buffer_t& buffer) noexcept {
        // NOLINTNEXTLINE(*-reinterpret-cast)
        return std::launder(reinterpret_cast<T*>(buffer.data));
    }

    template <typename T>
    static const T* buffer_cast(const buffer_t& buffer) noexcept {
        // NOLINTNEXTLINE(*-reinterpret-cast)
        return std::launder(reinterpret_cast<const T*>(buffer.data));
    }

    template <typename Fp>
    static vtable_t* get() {
        static_assert(std::is_same_v<Fp, std::decay_t<Fp>>);

        static vtable_t table{
                .call{[](const fixed_function& self, Args... args) -> R {
                    META_HPP_DEV_ASSERT(self);

                    const Fp& src = *buffer_cast<Fp>(self.buffer_);
                    return std::invoke(src, std::forward<Args>(args)...);
                }},

                .move{[](fixed_function& from, fixed_function& to) noexcept {
                    META_HPP_DEV_ASSERT(!to);
                    META_HPP_DEV_ASSERT(from);

                    Fp& src = *buffer_cast<Fp>(from.buffer_);
                    std::construct_at(buffer_cast<Fp>(to.buffer_), std::move(src));
                    std::destroy_at(&src);

                    to.vtable_ = from.vtable_;
                    from.vtable_ = nullptr;
                }},

                .destroy{[](fixed_function& self) {
                    META_HPP_DEV_ASSERT(self);

                    Fp& src = *buffer_cast<Fp>(self.buffer_);
                    std::destroy_at(&src);

                    self.vtable_ = nullptr;
                }},
        };
        return &table;
    }

    template <typename F, typename Fp = std::decay_t<F>>
    static void construct(fixed_function& dst, F&& fun) {
        META_HPP_DEV_ASSERT(!dst);

        static_assert(sizeof(Fp) <= sizeof(buffer_t));
        static_assert(alignof(buffer_t) % alignof(Fp) == 0);
        static_assert(std::is_invocable_r_v<R, Fp, Args...>);
        static_assert(std::is_nothrow_move_constructible_v<Fp>);

        std::construct_at(buffer_cast<Fp>(dst.buffer_), std::forward<F>(fun));

        dst.vtable_ = vtable_t::get<Fp>();
    }

    static void swap(fixed_function& l, fixed_function& r) noexcept {
        if ((&l == &r) || (!l && !r)) {
            return;
        }

        if (l && r) {
            fixed_function temp;
            r.vtable_->move(r, temp);
            l.vtable_->move(l, r);
            temp.vtable_->move(temp, l);
        } else {
            if (l) {
                l.vtable_->move(l, r);
            } else {
                r.vtable_->move(r, l);
            }
        }
    }
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
namespace impl {
template <typename F>
struct strip_signature_impl;

template <typename R, typename C, typename... Args>
struct strip_signature_impl<R (C::*)(Args...)> {
    using type = R(Args...);
};

template <typename R, typename C, typename... Args>
struct strip_signature_impl<R (C::*)(Args...) const> {
    using type = R(Args...);
};

template <typename R, typename C, typename... Args>
struct strip_signature_impl<R (C::*)(Args...)&> {
    using type = R(Args...);
};

template <typename R, typename C, typename... Args>
struct strip_signature_impl<R (C::*)(Args...) const&> {
    using type = R(Args...);
};

template <typename R, typename C, typename... Args>
struct strip_signature_impl<R (C::*)(Args...) noexcept> {
    using type = R(Args...);
};

template <typename R, typename C, typename... Args>
struct strip_signature_impl<R (C::*)(Args...) const noexcept> {
    using type = R(Args...);
};

template <typename R, typename C, typename... Args>
struct strip_signature_impl<R (C::*)(Args...) & noexcept> {
    using type = R(Args...);
};

template <typename R, typename C, typename... Args>
struct strip_signature_impl<R (C::*)(Args...) const & noexcept> {
    using type = R(Args...);
};

template <typename F>
using strip_signature_impl_t = typename strip_signature_impl<F>::type;
}  // namespace impl

template <typename R, typename... Args>
fixed_function(R (*)(Args...)) -> fixed_function<R(Args...)>;

template <typename F, typename S = impl::strip_signature_impl_t<decltype(&F::operator())>>
fixed_function(F) -> fixed_function<S>;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
// REFERENCE:
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

template <std::size_t SizeBits = CHAR_BIT * sizeof(std::size_t)>
struct fnv1a_hash_traits;

template <>
struct fnv1a_hash_traits<32> {  // NOLINT(*-magic-numbers)
    using underlying_type = std::uint32_t;
    static inline constexpr underlying_type prime{16777619U};
    static inline constexpr underlying_type offset_basis{2166136261U};
};

template <>
struct fnv1a_hash_traits<64> {  // NOLINT(*-magic-numbers)
    using underlying_type = std::uint64_t;
    static inline constexpr underlying_type prime{1099511628211U};
    static inline constexpr underlying_type offset_basis{14695981039346656037U};
};

template <typename T>
    requires(std::is_same_v<T, std::byte>) || (std::is_integral_v<T> && sizeof(T) == 1)
constexpr std::size_t fnv1a_hash(const T* mem, std::size_t size) noexcept {
    using traits = fnv1a_hash_traits<>;
    std::size_t hash{traits::offset_basis};
    for (T byte : std::span(mem, size)) {
        hash ^= static_cast<std::size_t>(byte);
        hash *= traits::prime;
    }
    return hash;
}

inline std::size_t fnv1a_hash(const void* mem, std::size_t size) noexcept { return fnv1a_hash(static_cast<const std::byte*>(mem), size); }
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
struct hash_combiner {
    template <typename T>
    [[nodiscard]] std::size_t operator()(const T& x) noexcept {
        return std::hash<T>{}(x);
    }

    template <typename T>
    [[nodiscard]] std::size_t operator()(std::size_t seed, const T& x) noexcept {
        // NOLINTNEXTLINE(*-magic-numbers)
        return (seed ^= std::hash<T>{}(x) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
    }
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
class hashed_string final {
public:
    hashed_string() = default;
    ~hashed_string() = default;

    hashed_string(hashed_string&&) = default;
    hashed_string(const hashed_string&) = default;

    hashed_string& operator=(hashed_string&&) = default;
    hashed_string& operator=(const hashed_string&) = default;

    constexpr hashed_string(std::string_view str) noexcept : hash_{fnv1a_hash(str.data(), str.size())} {}

    constexpr void swap(hashed_string& other) noexcept { std::swap(hash_, other.hash_); }

    [[nodiscard]] constexpr std::size_t get_hash() const noexcept { return hash_; }

    [[nodiscard]] constexpr bool operator==(hashed_string other) const noexcept { return hash_ == other.hash_; }

    [[nodiscard]] constexpr std::strong_ordering operator<=>(hashed_string other) const noexcept { return hash_ <=> other.hash_; }

private:
    std::size_t hash_{fnv1a_hash("", 0)};
};

constexpr void swap(hashed_string& l, hashed_string& r) noexcept { l.swap(r); }

[[nodiscard]] constexpr bool operator==(hashed_string l, std::string_view r) noexcept { return l == hashed_string{r}; }

[[nodiscard]] constexpr std::strong_ordering operator<=>(hashed_string l, std::string_view r) noexcept { return l <=> hashed_string{r}; }
}  // namespace ME::meta::r::detail

namespace std {
template <>
struct hash<ME::meta::r::detail::hashed_string> {
    size_t operator()(ME::meta::r::detail::hashed_string hs) const noexcept { return hs.get_hash(); }
};
}  // namespace std

namespace ME::meta::r::detail {
template <typename Value, typename Allocator>
    requires std::is_move_constructible_v<Value> && std::is_move_assignable_v<Value>
typename std::vector<Value, Allocator>::iterator insert_or_assign(  //
        std::vector<Value, Allocator>& vector, typename std::vector<Value, Allocator>::value_type&& value) {
    if (auto&& position{std::find(vector.begin(), vector.end(), value)}; position != vector.end()) {
        *position = std::move(value);
        return position;
    }
    return vector.insert(vector.end(), std::move(value));
}

template <typename Value, typename Allocator>
    requires std::is_copy_constructible_v<Value> && std::is_copy_assignable_v<Value>
typename std::vector<Value, Allocator>::iterator insert_or_assign(  //
        std::vector<Value, Allocator>& vector, const typename std::vector<Value, Allocator>::value_type& value) {
    if (auto&& position{std::find(vector.begin(), vector.end(), value)}; position != vector.end()) {
        *position = value;
        return position;
    }
    return vector.insert(vector.end(), value);
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename Key, typename Compare, typename Allocator>
void insert_or_assign(  //
        std::set<Key, Compare, Allocator>& set, std::set<Key, Compare, Allocator>& value) {
    set.swap(value);
    set.merge(value);
}

template <typename Key, typename Compare, typename Allocator>
void insert_or_assign(  //
        std::set<Key, Compare, Allocator>& set, std::set<Key, Compare, Allocator>&& value) {
    set.swap(value);
    set.merge(std::move(value));
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename Key, typename Value, typename Compare, typename Allocator>
void insert_or_assign(  //
        std::map<Key, Value, Compare, Allocator>& map, std::map<Key, Value, Compare, Allocator>& value) {
    map.swap(value);
    map.merge(value);
}

template <typename Key, typename Value, typename Compare, typename Allocator>
void insert_or_assign(  //
        std::map<Key, Value, Compare, Allocator>& map, std::map<Key, Value, Compare, Allocator>&& value) {
    map.swap(value);
    map.merge(std::move(value));
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename Derived>
class intrusive_ref_counter;

template <typename Derived>
void intrusive_ptr_add_ref(const intrusive_ref_counter<Derived>* ptr);

template <typename Derived>
void intrusive_ptr_release(const intrusive_ref_counter<Derived>* ptr);

template <typename Derived>
class intrusive_ref_counter {
public:
    intrusive_ref_counter() = default;

    intrusive_ref_counter(intrusive_ref_counter&&) noexcept {}

    intrusive_ref_counter(const intrusive_ref_counter&) noexcept {}

    intrusive_ref_counter& operator=(intrusive_ref_counter&&) noexcept { return *this; }

    intrusive_ref_counter& operator=(const intrusive_ref_counter&) noexcept { return *this; }

    [[nodiscard]] std::size_t get_use_count() const noexcept { return counter_.load(std::memory_order_acquire); }

protected:
    ~intrusive_ref_counter() = default;

private:
    mutable std::atomic_size_t counter_{};
    friend void intrusive_ptr_add_ref<Derived>(const intrusive_ref_counter* ptr);
    friend void intrusive_ptr_release<Derived>(const intrusive_ref_counter* ptr);
};

template <typename Derived>
void intrusive_ptr_add_ref(const intrusive_ref_counter<Derived>* ptr) {
    ptr->counter_.fetch_add(1, std::memory_order_acq_rel);
}

template <typename Derived>
void intrusive_ptr_release(const intrusive_ref_counter<Derived>* ptr) {
    if (ptr->counter_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
        // NOLINTNEXTLINE(*-owning-memory)
        delete static_cast<const Derived*>(ptr);
    }
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename T>
class intrusive_ptr final {
public:
    intrusive_ptr() = default;

    ~intrusive_ptr() {
        if (ptr_ != nullptr) {
            intrusive_ptr_release(ptr_);
        }
    }

    intrusive_ptr(T* ptr, bool add_ref = true) : ptr_{ptr} {
        if (ptr_ != nullptr && add_ref) {
            intrusive_ptr_add_ref(ptr_);
        }
    }

    intrusive_ptr(intrusive_ptr&& other) noexcept : ptr_{other.ptr_} { other.ptr_ = nullptr; }

    intrusive_ptr(const intrusive_ptr& other) : ptr_{other.ptr_} {
        if (ptr_ != nullptr) {
            intrusive_ptr_add_ref(ptr_);
        }
    }

    intrusive_ptr& operator=(T* ptr) noexcept {
        intrusive_ptr{ptr}.swap(*this);
        return *this;
    }

    intrusive_ptr& operator=(intrusive_ptr&& other) noexcept {
        intrusive_ptr{std::move(other)}.swap(*this);
        return *this;
    }

    intrusive_ptr& operator=(const intrusive_ptr& other) {
        intrusive_ptr{other}.swap(*this);
        return *this;
    }

    void reset() { intrusive_ptr{}.swap(*this); }

    void reset(T* ptr) { intrusive_ptr{ptr}.swap(*this); }

    void reset(T* ptr, bool add_ref) { intrusive_ptr{ptr, add_ref}.swap(*this); }

    T* release() noexcept { return std::exchange(ptr_, nullptr); }

    [[nodiscard]] T* get() const noexcept { return ptr_; }

    [[nodiscard]] T& operator*() const noexcept { return *ptr_; }

    [[nodiscard]] T* operator->() const noexcept { return ptr_; }

    [[nodiscard]] explicit operator bool() const noexcept { return ptr_ != nullptr; }

    void swap(intrusive_ptr& other) noexcept { ptr_ = std::exchange(other.ptr_, ptr_); }

    [[nodiscard]] std::size_t get_hash() const noexcept { return hash_combiner{}(ptr_); }

    [[nodiscard]] bool operator==(const intrusive_ptr& other) const noexcept { return ptr_ == other.ptr_; }

    [[nodiscard]] std::strong_ordering operator<=>(const intrusive_ptr& other) const noexcept { return ptr_ <=> other.ptr_; }

private:
    T* ptr_{};
};

template <typename T, typename... Args>
intrusive_ptr<T> make_intrusive(Args&&... args) {
    // NOLINTNEXTLINE(*-owning-memory)
    return new T(std::forward<Args>(args)...);
}

template <typename T>
void swap(intrusive_ptr<T>& l, intrusive_ptr<T>& r) noexcept {
    return l.swap(r);
}

template <typename T>
[[nodiscard]] bool operator==(const intrusive_ptr<T>& l, const T* r) noexcept {
    return l.get() == r;
}

template <typename T>
[[nodiscard]] std::strong_ordering operator<=>(const intrusive_ptr<T>& l, const T* r) noexcept {
    return l.get() <=> r;
}

template <typename T>
[[nodiscard]] bool operator==(const intrusive_ptr<T>& l, std::nullptr_t) noexcept {
    return l.get() == nullptr;
}

template <typename T>
[[nodiscard]] std::strong_ordering operator<=>(const intrusive_ptr<T>& l, std::nullptr_t) noexcept {
    return l.get() <=> nullptr;
}
}  // namespace ME::meta::r::detail

namespace std {
template <typename T>
struct hash<ME::meta::r::detail::intrusive_ptr<T>> {
    size_t operator()(const ME::meta::r::detail::intrusive_ptr<T>& ip) const noexcept { return ip.get_hash(); }
};
}  // namespace std

namespace ME::meta::r::detail {
template <typename SortedContainerL, typename SortedContainerR, typename Compare>
bool is_disjoint(const SortedContainerL& l, const SortedContainerR& r, Compare compare) {
    using std::begin;
    using std::end;

    for (auto iter_l{begin(l)}, iter_r{begin(r)}; iter_l != end(l) && iter_r != end(r);) {
        if (compare(*iter_l, *iter_r)) {
            ++iter_l;
        } else if (compare(*iter_r, *iter_l)) {
            ++iter_r;
        } else {
            return false;
        }
    }

    return true;
}

template <typename SortedContainerL, typename SortedContainerR>
bool is_disjoint(const SortedContainerL& l, const SortedContainerR& r) {
    return is_disjoint(l, r, std::less<>{});
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename T>
struct is_in_place_type : std::false_type {};

template <typename U>
struct is_in_place_type<std::in_place_type_t<U>> : std::true_type {};

template <typename T>
inline constexpr bool is_in_place_type_v = is_in_place_type<T>::value;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
namespace impl {
template <typename From, typename To>
constexpr bool is_virtual_base_of_impl(...) noexcept {
    return false;
}

template <typename From, typename To, decltype(static_cast<const volatile To*>(std::declval<const volatile From*>())) = nullptr>
constexpr bool is_virtual_base_of_impl(int) noexcept {
    return true;
}
}  // namespace impl

// clang-format off

    template < typename Base, typename Derived >
    struct is_virtual_base_of : std::integral_constant<bool,
          std::is_base_of_v<Base, Derived> &&
          impl::is_virtual_base_of_impl<Derived, Base>(0) &&
          !impl::is_virtual_base_of_impl<Base, Derived>(0)> {};

// clang-format on

template <typename Base, typename Derived>
inline constexpr bool is_virtual_base_of_v = is_virtual_base_of<Base, Derived>::value;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
class memory_buffer final {
public:
    memory_buffer() = default;

    memory_buffer(const memory_buffer&) = delete;
    memory_buffer& operator=(const memory_buffer&) = delete;

    memory_buffer(memory_buffer&& other) noexcept : data_{other.data_}, size_{other.size_}, align_{other.align_} {
        other.data_ = nullptr;
        other.size_ = 0;
        other.align_ = std::align_val_t{};
    }

    memory_buffer& operator=(memory_buffer&& other) noexcept {
        if (this != &other) {
            memory_buffer{std::move(other)}.swap(*this);
        }
        return *this;
    }

    explicit memory_buffer(std::size_t size, std::align_val_t align) : data_{::operator new(size, align)}, size_{size}, align_{align} {}

    explicit memory_buffer(const void* mem, std::size_t size, std::align_val_t align) : memory_buffer{size, align} {
        if (mem != nullptr && size > 0) {
            std::memcpy(data_, mem, size);
        }
    }

    ~memory_buffer() noexcept { reset(); }

    [[nodiscard]] bool is_valid() const noexcept { return data_ != nullptr; }

    [[nodiscard]] explicit operator bool() const noexcept { return is_valid(); }

    void reset() noexcept {
        if (data_ != nullptr) {
            ::operator delete(data_, align_);
            data_ = nullptr;
            size_ = 0;
            align_ = std::align_val_t{};
        }
    }

    [[nodiscard]] void* get_data() noexcept { return data_; }

    [[nodiscard]] const void* get_data() const noexcept { return data_; }

    [[nodiscard]] std::size_t get_size() const noexcept { return size_; }

    [[nodiscard]] std::align_val_t get_align() const noexcept { return align_; }

    void swap(memory_buffer& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(align_, other.align_);
    }

    [[nodiscard]] std::size_t get_hash() const noexcept { return fnv1a_hash(data_, size_); }

    [[nodiscard]] bool operator==(const memory_buffer& other) const noexcept {
        return (size_ == other.size_)  //
               && (size_ == 0 || std::memcmp(data_, other.data_, size_) == 0);
    }

    [[nodiscard]] std::strong_ordering operator<=>(const memory_buffer& other) const noexcept {
        if (const std::strong_ordering cmp{size_ <=> other.size_}; cmp != std::strong_ordering::equal) {
            return cmp;
        }
        return (size_ == 0 ? 0 : std::memcmp(data_, other.data_, size_)) <=> 0;
    }

private:
    void* data_{};
    std::size_t size_{};
    std::align_val_t align_{};
};

inline void swap(memory_buffer& l, memory_buffer& r) noexcept { l.swap(r); }
}  // namespace ME::meta::r::detail

namespace std {
template <>
struct hash<ME::meta::r::detail::memory_buffer> {
    size_t operator()(const ME::meta::r::detail::memory_buffer& mb) const noexcept { return mb.get_hash(); }
};
}  // namespace std

namespace ME::meta::r::detail {
class noncopyable {
protected:
    noncopyable() = default;
    ~noncopyable() = default;

public:
    noncopyable(noncopyable&&) = delete;
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(noncopyable&&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
class nonesuch {
public:
    nonesuch() = delete;
    ~nonesuch() = delete;
    nonesuch(nonesuch&&) = delete;
    nonesuch(const nonesuch&) = delete;
    nonesuch& operator=(nonesuch&&) = delete;
    nonesuch& operator=(const nonesuch&) = delete;
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename C, typename R, typename... Args>
constexpr auto select_const(R (C::*func)(Args...) const) -> decltype(func) {
    return func;
}

template <typename C, typename R, typename... Args>
constexpr auto select_const(R (C::*func)(Args...) const noexcept) -> decltype(func) {
    return func;
}

template <typename C, typename R, typename... Args>
constexpr auto select_non_const(R (C::*func)(Args...)) -> decltype(func) {
    return func;
}

template <typename C, typename R, typename... Args>
constexpr auto select_non_const(R (C::*func)(Args...) noexcept) -> decltype(func) {
    return func;
}

template <typename Signature>
constexpr auto select_overload(Signature* func) noexcept -> decltype(func) {
    return func;
}

template <typename Signature, typename C>
constexpr auto select_overload(Signature C::*func) noexcept -> decltype(func) {
    return func;
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename Enum>
[[nodiscard]] constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
    return static_cast<std::underlying_type_t<Enum>>(e);
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
enum class type_kind : std::uint32_t {
    array_,
    class_,
    constructor_,
    destructor_,
    enum_,
    function_,
    member_,
    method_,
    nullptr_,
    number_,
    pointer_,
    reference_,
    void_,
};
}

namespace ME::meta::r::detail {
template <typename T>
concept array_kind = std::is_array_v<T>;

template <typename T>
concept class_kind = std::is_class_v<T>;

template <typename T>
concept enum_kind = std::is_enum_v<T>;

template <typename T>
concept function_kind = std::is_function_v<T>;

template <typename T>
concept member_pointer_kind = std::is_member_object_pointer_v<T>;

template <typename T>
concept method_pointer_kind = std::is_member_function_pointer_v<T>;

template <typename T>
concept nullptr_kind = std::is_null_pointer_v<T>;

template <typename T>
concept number_kind = std::is_arithmetic_v<T>;

template <typename T>
concept pointer_kind = std::is_pointer_v<T>;

template <typename T>
concept reference_kind = std::is_reference_v<T>;

template <typename T>
concept void_kind = std::is_void_v<T>;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename T>
concept non_pointer_kind = !
std::is_pointer_v<T>;

template <typename T>
concept function_pointer_kind = std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>;

template <typename T>
concept non_function_pointer_kind = std::is_pointer_v<T> && !
std::is_function_v<std::remove_pointer_t<T>>;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename... Types>
struct type_list {};

template <std::size_t I>
using size_constant = std::integral_constant<std::size_t, I>;

template <std::size_t I>
using index_constant = std::integral_constant<std::size_t, I>;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <std::size_t Index, typename TypeList>
struct type_list_at;

template <std::size_t Index, typename... Types>
struct type_list_at<Index, type_list<Types...>> {
    using type = std::tuple_element_t<Index, std::tuple<Types...>>;
};

template <std::size_t Index, typename TypeList>
using type_list_at_t = typename type_list_at<Index, TypeList>::type;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename TypeList>
struct type_list_arity;

template <typename... Types>
struct type_list_arity<type_list<Types...>> : size_constant<sizeof...(Types)> {};

template <typename TypeList>
inline constexpr std::size_t type_list_arity_v = type_list_arity<TypeList>::value;
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
using ::ME::exception::error_code;
using ::ME::exception::exception_meta;
using ::ME::exception::get_error_code_message;

using detail::hashed_string;
using detail::memory_buffer;

using detail::select_const;
using detail::select_non_const;
using detail::select_overload;

using detail::type_kind;
using detail::type_list;
}  // namespace ME::meta::r

namespace ME::meta::r {
class uerror;
class uresult;

class uvalue;

namespace detail {
class uarg_base;
class uarg;

class uinst_base;
class uinst;
}  // namespace detail

namespace detail {
template <typename T>
concept uvalue_family                       //
        = std::is_same_v<T, uarg_base>      //
          || std::is_same_v<T, uarg>        //
          || std::is_same_v<T, uinst_base>  //
          || std::is_same_v<T, uinst>       //
          || std::is_same_v<T, uerror>      //
          || std::is_same_v<T, uresult>     //
          || std::is_same_v<T, uvalue>;     //

template <typename T>
concept non_uvalue_family = !
uvalue_family<T>;
}  // namespace detail
}  // namespace ME::meta::r

namespace ME::meta::r {
class argument;
class constructor;
class destructor;
class evalue;
class function;
class member;
class method;
class scope;
class variable;

namespace detail {
struct argument_state;
struct constructor_state;
struct destructor_state;
struct evalue_state;
struct function_state;
struct member_state;
struct method_state;
struct scope_state;
struct variable_state;

using argument_state_ptr = intrusive_ptr<argument_state>;
using constructor_state_ptr = intrusive_ptr<constructor_state>;
using destructor_state_ptr = intrusive_ptr<destructor_state>;
using evalue_state_ptr = intrusive_ptr<evalue_state>;
using function_state_ptr = intrusive_ptr<function_state>;
using member_state_ptr = intrusive_ptr<member_state>;
using method_state_ptr = intrusive_ptr<method_state>;
using scope_state_ptr = intrusive_ptr<scope_state>;
using variable_state_ptr = intrusive_ptr<variable_state>;
}  // namespace detail

namespace detail {
template <typename T>
concept state_family                         //
        = std::is_same_v<T, argument>        //
          || std::is_same_v<T, constructor>  //
          || std::is_same_v<T, destructor>   //
          || std::is_same_v<T, evalue>       //
          || std::is_same_v<T, function>     //
          || std::is_same_v<T, member>       //
          || std::is_same_v<T, method>       //
          || std::is_same_v<T, scope>        //
          || std::is_same_v<T, variable>;    //
}
}  // namespace ME::meta::r

namespace ME::meta::r {
class any_type;
class array_type;
class class_type;
class constructor_type;
class destructor_type;
class enum_type;
class function_type;
class member_type;
class method_type;
class nullptr_type;
class number_type;
class pointer_type;
class reference_type;
class void_type;

namespace detail {
struct type_data_base;
struct array_type_data;
struct class_type_data;
struct constructor_type_data;
struct destructor_type_data;
struct enum_type_data;
struct function_type_data;
struct member_type_data;
struct method_type_data;
struct nullptr_type_data;
struct number_type_data;
struct pointer_type_data;
struct reference_type_data;
struct void_type_data;
}  // namespace detail

namespace detail {
template <typename T>
concept type_family                               //
        = std::is_same_v<T, any_type>             //
          || std::is_same_v<T, array_type>        //
          || std::is_same_v<T, class_type>        //
          || std::is_same_v<T, constructor_type>  //
          || std::is_same_v<T, destructor_type>   //
          || std::is_same_v<T, enum_type>         //
          || std::is_same_v<T, function_type>     //
          || std::is_same_v<T, member_type>       //
          || std::is_same_v<T, method_type>       //
          || std::is_same_v<T, nullptr_type>      //
          || std::is_same_v<T, number_type>       //
          || std::is_same_v<T, pointer_type>      //
          || std::is_same_v<T, reference_type>    //
          || std::is_same_v<T, void_type>;        //
}
}  // namespace ME::meta::r

namespace ME::meta::r {
class argument_index;
class constructor_index;
class destructor_index;
class evalue_index;
class function_index;
class member_index;
class method_index;
class scope_index;
class variable_index;

namespace detail {
template <typename T>
concept index_family                               //
        = std::is_same_v<T, argument_index>        //
          || std::is_same_v<T, constructor_index>  //
          || std::is_same_v<T, destructor_index>   //
          || std::is_same_v<T, evalue_index>       //
          || std::is_same_v<T, function_index>     //
          || std::is_same_v<T, member_index>       //
          || std::is_same_v<T, method_index>       //
          || std::is_same_v<T, scope_index>        //
          || std::is_same_v<T, variable_index>;    //
}
}  // namespace ME::meta::r

namespace ME::meta::r {
using string_ilist = std::initializer_list<std::string_view>;

using metadata_map = std::map<std::string, uvalue, std::less<>>;
using typedef_map = std::map<std::string, any_type, std::less<>>;

using any_type_list = std::vector<any_type>;
using class_list = std::vector<class_type>;
using enum_list = std::vector<enum_type>;

using argument_list = std::vector<argument>;
using constructor_list = std::vector<constructor>;
using destructor_list = std::vector<destructor>;
using evalue_list = std::vector<evalue>;
using function_list = std::vector<function>;
using member_list = std::vector<member>;
using method_list = std::vector<method>;
using variable_list = std::vector<variable>;
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
enum class array_flags : std::uint32_t {
    is_bounded = 1 << 0,
    is_unbounded = 1 << 1,
};

META_HPP_BITFLAGS_OPERATORS_DECL(array_flags)
using array_bitflags = bitflags<array_flags>;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <array_kind Array>
struct array_traits {
    static constexpr std::size_t extent{std::extent_v<Array>};

    using data_type = std::remove_extent_t<Array>;

    [[nodiscard]] static constexpr array_bitflags make_flags() noexcept {
        array_bitflags flags{};

        if constexpr (std::is_bounded_array_v<Array>) {
            flags.set(array_flags::is_bounded);
        }

        if constexpr (std::is_unbounded_array_v<Array>) {
            flags.set(array_flags::is_unbounded);
        }

        return flags;
    }
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
enum class class_flags : std::uint32_t {
    is_empty = 1 << 0,
    is_final = 1 << 1,
    is_abstract = 1 << 2,
    is_polymorphic = 1 << 3,
    is_template_instantiation = 1 << 4,
};

META_HPP_BITFLAGS_OPERATORS_DECL(class_flags)
using class_bitflags = bitflags<class_flags>;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
namespace impl {
template <class_kind Class>
struct class_traits_impl {
    static constexpr std::size_t arity{0};

    using argument_types = type_list<>;

    [[nodiscard]] static constexpr class_bitflags make_flags() noexcept { return {}; }
};

template <template <typename...> typename Class, typename... Args>
struct class_traits_impl<Class<Args...>> {
    static constexpr std::size_t arity{sizeof...(Args)};

    using argument_types = type_list<Args...>;

    [[nodiscard]] static constexpr class_bitflags make_flags() noexcept { return class_flags::is_template_instantiation; }
};
}  // namespace impl

template <class_kind Class>
struct class_traits : impl::class_traits_impl<Class> {
    static constexpr std::size_t size{sizeof(Class)};
    static constexpr std::size_t align{alignof(Class)};

    [[nodiscard]] static constexpr class_bitflags make_flags() noexcept {
        class_bitflags flags{};

        if constexpr (std::is_empty_v<Class>) {
            flags.set(class_flags::is_empty);
        }

        if constexpr (std::is_final_v<Class>) {
            flags.set(class_flags::is_final);
        }

        if constexpr (std::is_abstract_v<Class>) {
            flags.set(class_flags::is_abstract);
        }

        if constexpr (std::is_polymorphic_v<Class>) {
            flags.set(class_flags::is_polymorphic);
        }

        return flags | impl::class_traits_impl<Class>::make_flags();
    }
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
enum class constructor_flags : std::uint32_t {
    is_noexcept = 1 << 0,
};

META_HPP_BITFLAGS_OPERATORS_DECL(constructor_flags)
using constructor_bitflags = bitflags<constructor_flags>;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <class_kind Class, typename... Args>
struct constructor_traits {
    static constexpr std::size_t arity{sizeof...(Args)};

    using class_type = Class;
    using argument_types = type_list<Args...>;

    [[nodiscard]] static constexpr constructor_bitflags make_flags() noexcept {
        constructor_bitflags flags{};

        if constexpr (std::is_nothrow_constructible_v<Class, Args...>) {
            flags.set(constructor_flags::is_noexcept);
        }

        return flags;
    }
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
enum class destructor_flags : std::uint32_t {
    is_noexcept = 1 << 0,
    is_virtual = 1 << 1,
};

META_HPP_BITFLAGS_OPERATORS_DECL(destructor_flags)
using destructor_bitflags = bitflags<destructor_flags>;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <class_kind Class>
struct destructor_traits {
    using class_type = Class;

    [[nodiscard]] static constexpr destructor_bitflags make_flags() noexcept {
        destructor_bitflags flags{};

        if constexpr (std::is_nothrow_destructible_v<Class>) {
            flags.set(destructor_flags::is_noexcept);
        }

        if constexpr (std::has_virtual_destructor_v<Class>) {
            flags.set(destructor_flags::is_virtual);
        }

        return flags;
    }
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
enum class enum_flags : std::uint32_t {
    is_scoped = 1 << 0,
};

META_HPP_BITFLAGS_OPERATORS_DECL(enum_flags)
using enum_bitflags = bitflags<enum_flags>;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <enum_kind Enum>
struct enum_traits {
    using underlying_type = std::underlying_type_t<Enum>;

    [[nodiscard]] static constexpr enum_bitflags make_flags() noexcept {
        enum_bitflags flags{};

        if constexpr (!std::is_convertible_v<Enum, underlying_type>) {
            flags.set(enum_flags::is_scoped);
        }

        return flags;
    }
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
enum class function_flags : std::uint32_t {
    is_noexcept = 1 << 0,
};

META_HPP_BITFLAGS_OPERATORS_DECL(function_flags)
using function_bitflags = bitflags<function_flags>;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <function_kind Function>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<R(Args...)> {
    static constexpr std::size_t arity{sizeof...(Args)};

    using return_type = R;
    using argument_types = type_list<Args...>;

    [[nodiscard]] static constexpr function_bitflags make_flags() noexcept { return {}; }
};

template <typename R, typename... Args>
struct function_traits<R(Args...) noexcept> : function_traits<R(Args...)> {
    [[nodiscard]] static constexpr function_bitflags make_flags() noexcept { return function_flags::is_noexcept; }
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
enum class member_flags : std::uint32_t {
    is_readonly = 1 << 0,
};

META_HPP_BITFLAGS_OPERATORS_DECL(member_flags)
using member_bitflags = bitflags<member_flags>;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <member_pointer_kind Member>
struct member_traits;

template <typename V, typename C>
struct member_traits<V C::*> {
    using class_type = C;
    using value_type = V;

    [[nodiscard]] static constexpr member_bitflags make_flags() noexcept {
        member_bitflags flags{};

        if constexpr (std::is_const_v<value_type>) {
            flags.set(member_flags::is_readonly);
        }

        return flags;
    }
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
enum class method_flags : std::uint32_t {
    is_const = 1 << 0,
    is_noexcept = 1 << 1,
    is_lvalue_qualified = 1 << 2,
    is_rvalue_qualified = 1 << 3,
};

META_HPP_BITFLAGS_OPERATORS_DECL(method_flags)
using method_bitflags = bitflags<method_flags>;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <method_pointer_kind Method>
struct method_traits;

template <typename R, typename C, typename... Args>
struct method_traits<R (C::*)(Args...)> {
    static constexpr std::size_t arity{sizeof...(Args)};

    using class_type = C;
    using return_type = R;
    using qualified_type = C;
    using argument_types = type_list<Args...>;

    [[nodiscard]] static constexpr method_bitflags make_flags() noexcept { return {}; }
};

template <typename R, typename C, typename... Args>
struct method_traits<R (C::*)(Args...) const> : method_traits<R (C::*)(Args...)> {
    using qualified_type = const C;

    [[nodiscard]] static constexpr method_bitflags make_flags() noexcept { return method_flags::is_const; }
};

template <typename R, typename C, typename... Args>
struct method_traits<R (C::*)(Args...) noexcept> : method_traits<R (C::*)(Args...)> {
    using qualified_type = C;

    [[nodiscard]] static constexpr method_bitflags make_flags() noexcept { return method_flags::is_noexcept; }
};

template <typename R, typename C, typename... Args>
struct method_traits<R (C::*)(Args...) const noexcept> : method_traits<R (C::*)(Args...)> {
    using qualified_type = const C;

    [[nodiscard]] static constexpr method_bitflags make_flags() noexcept { return method_flags::is_const | method_flags::is_noexcept; }
};

template <typename R, typename C, typename... Args>
struct method_traits<R (C::*)(Args...)&> : method_traits<R (C::*)(Args...)> {
    using qualified_type = C&;

    [[nodiscard]] static constexpr method_bitflags make_flags() noexcept { return method_flags::is_lvalue_qualified; }
};

template <typename R, typename C, typename... Args>
struct method_traits<R (C::*)(Args...) & noexcept> : method_traits<R (C::*)(Args...)> {
    using qualified_type = C&;

    [[nodiscard]] static constexpr method_bitflags make_flags() noexcept { return method_flags::is_noexcept | method_flags::is_lvalue_qualified; }
};

template <typename R, typename C, typename... Args>
struct method_traits<R (C::*)(Args...) const&> : method_traits<R (C::*)(Args...)> {
    using qualified_type = const C&;

    [[nodiscard]] static constexpr method_bitflags make_flags() noexcept { return method_flags::is_const | method_flags::is_lvalue_qualified; }
};

template <typename R, typename C, typename... Args>
struct method_traits<R (C::*)(Args...) const & noexcept> : method_traits<R (C::*)(Args...)> {
    using qualified_type = const C&;

    [[nodiscard]] static constexpr method_bitflags make_flags() noexcept { return method_flags::is_const | method_flags::is_noexcept | method_flags::is_lvalue_qualified; }
};

template <typename R, typename C, typename... Args>
struct method_traits<R (C::*)(Args...) &&> : method_traits<R (C::*)(Args...)> {
    using qualified_type = C&&;

    [[nodiscard]] static constexpr method_bitflags make_flags() noexcept { return method_flags::is_rvalue_qualified; }
};

template <typename R, typename C, typename... Args>
struct method_traits<R (C::*)(Args...) && noexcept> : method_traits<R (C::*)(Args...)> {
    using qualified_type = C&&;

    [[nodiscard]] static constexpr method_bitflags make_flags() noexcept { return method_flags::is_noexcept | method_flags::is_rvalue_qualified; }
};

template <typename R, typename C, typename... Args>
struct method_traits<R (C::*)(Args...) const&&> : method_traits<R (C::*)(Args...)> {
    using qualified_type = const C&&;

    [[nodiscard]] static constexpr method_bitflags make_flags() noexcept { return method_flags::is_const | method_flags::is_rvalue_qualified; }
};

template <typename R, typename C, typename... Args>
struct method_traits<R (C::*)(Args...) const && noexcept> : method_traits<R (C::*)(Args...)> {
    using qualified_type = const C&&;

    [[nodiscard]] static constexpr method_bitflags make_flags() noexcept { return method_flags::is_const | method_flags::is_noexcept | method_flags::is_rvalue_qualified; }
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
enum class number_flags : std::uint32_t {
    is_signed = 1 << 0,
    is_unsigned = 1 << 1,
    is_integral = 1 << 2,
    is_floating_point = 1 << 3,
};

META_HPP_BITFLAGS_OPERATORS_DECL(number_flags)
using number_bitflags = bitflags<number_flags>;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <number_kind Number>
struct number_traits {
    static constexpr std::size_t size{sizeof(Number)};
    static constexpr std::size_t align{alignof(Number)};

    [[nodiscard]] static constexpr number_bitflags make_flags() noexcept {
        number_bitflags flags{};

        if constexpr (std::is_signed_v<Number>) {
            flags.set(number_flags::is_signed);
        }

        if constexpr (std::is_unsigned_v<Number>) {
            flags.set(number_flags::is_unsigned);
        }

        if constexpr (std::is_integral_v<Number>) {
            flags.set(number_flags::is_integral);
        }

        if constexpr (std::is_floating_point_v<Number>) {
            flags.set(number_flags::is_floating_point);
        }

        return flags;
    }
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
enum class pointer_flags : std::uint32_t {
    is_readonly = 1 << 0,
};

META_HPP_BITFLAGS_OPERATORS_DECL(pointer_flags)
using pointer_bitflags = bitflags<pointer_flags>;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <pointer_kind Pointer>
struct pointer_traits {
    using data_type = std::remove_pointer_t<Pointer>;

    [[nodiscard]] static constexpr pointer_bitflags make_flags() noexcept {
        pointer_bitflags flags{};

        if constexpr (std::is_const_v<data_type>) {
            flags.set(pointer_flags::is_readonly);
        }

        return flags;
    }
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
enum class reference_flags : std::uint32_t {
    is_readonly = 1 << 0,
    is_lvalue = 1 << 1,
    is_rvalue = 1 << 2,
};

META_HPP_BITFLAGS_OPERATORS_DECL(reference_flags)
using reference_bitflags = bitflags<reference_flags>;
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <reference_kind Reference>
struct reference_traits {
    using data_type = std::remove_reference_t<Reference>;

    [[nodiscard]] static constexpr reference_bitflags make_flags() noexcept {
        reference_bitflags flags{};

        if constexpr (std::is_const_v<data_type>) {
            flags.set(reference_flags::is_readonly);
        }

        if constexpr (std::is_lvalue_reference_v<Reference>) {
            flags.set(reference_flags::is_lvalue);
        }

        if constexpr (std::is_rvalue_reference_v<Reference>) {
            flags.set(reference_flags::is_rvalue);
        }

        return flags;
    }
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <type_family Type>
struct type_traits;

template <type_family Type>
[[nodiscard]] typename type_traits<Type>::data_ptr type_access(const Type& type) {
    return type.data_;
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <>
struct type_traits<any_type> {
    using data_ptr = type_data_base*;
};

template <>
struct type_traits<array_type> {
    using data_ptr = array_type_data*;
    inline static constexpr type_kind kind{type_kind::array_};
};

template <>
struct type_traits<class_type> {
    using data_ptr = class_type_data*;
    inline static constexpr type_kind kind{type_kind::class_};
};

template <>
struct type_traits<constructor_type> {
    using data_ptr = constructor_type_data*;
    inline static constexpr type_kind kind{type_kind::constructor_};
};

template <>
struct type_traits<destructor_type> {
    using data_ptr = destructor_type_data*;
    inline static constexpr type_kind kind{type_kind::destructor_};
};

template <>
struct type_traits<enum_type> {
    using data_ptr = enum_type_data*;
    inline static constexpr type_kind kind{type_kind::enum_};
};

template <>
struct type_traits<function_type> {
    using data_ptr = function_type_data*;
    inline static constexpr type_kind kind{type_kind::function_};
};

template <>
struct type_traits<member_type> {
    using data_ptr = member_type_data*;
    inline static constexpr type_kind kind{type_kind::member_};
};

template <>
struct type_traits<method_type> {
    using data_ptr = method_type_data*;
    inline static constexpr type_kind kind{type_kind::method_};
};

template <>
struct type_traits<nullptr_type> {
    using data_ptr = nullptr_type_data*;
    inline static constexpr type_kind kind{type_kind::nullptr_};
};

template <>
struct type_traits<number_type> {
    using data_ptr = number_type_data*;
    inline static constexpr type_kind kind{type_kind::number_};
};

template <>
struct type_traits<pointer_type> {
    using data_ptr = pointer_type_data*;
    inline static constexpr type_kind kind{type_kind::pointer_};
};

template <>
struct type_traits<reference_type> {
    using data_ptr = reference_type_data*;
    inline static constexpr type_kind kind{type_kind::reference_};
};

template <>
struct type_traits<void_type> {
    using data_ptr = void_type_data*;
    inline static constexpr type_kind kind{type_kind::void_};
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
using detail::array_bitflags;
using detail::array_flags;

using detail::class_bitflags;
using detail::class_flags;

using detail::constructor_bitflags;
using detail::constructor_flags;

using detail::destructor_bitflags;
using detail::destructor_flags;

using detail::enum_bitflags;
using detail::enum_flags;

using detail::function_bitflags;
using detail::function_flags;

using detail::member_bitflags;
using detail::member_flags;

using detail::method_bitflags;
using detail::method_flags;

using detail::number_bitflags;
using detail::number_flags;

using detail::pointer_bitflags;
using detail::pointer_flags;

using detail::reference_bitflags;
using detail::reference_flags;
}  // namespace ME::meta::r

namespace ME::meta::r {
class type_id final {
public:
    type_id() = default;

    [[nodiscard]] bool is_valid() const noexcept { return data_ != nullptr; }

    [[nodiscard]] explicit operator bool() const noexcept { return is_valid(); }

    void swap(type_id& other) noexcept { std::swap(data_, other.data_); }

    [[nodiscard]] std::size_t get_hash() const noexcept { return data_ != nullptr ? detail::hash_combiner{}(data_) : 0; }

    [[nodiscard]] std::strong_ordering operator<=>(const type_id& other) const = default;

private:
    template <detail::type_family T>
    friend class type_base;

    explicit type_id(const detail::type_data_base* data) : data_{data} {}

private:
    const detail::type_data_base* data_{};
};
}  // namespace ME::meta::r

namespace std {
template <>
struct hash<ME::meta::r::type_id> {
    size_t operator()(const ME::meta::r::type_id& id) const noexcept { return id.get_hash(); }
};
}  // namespace std

namespace ME::meta::r {
template <detail::type_family Type>
class type_base {
    using data_ptr = typename detail::type_traits<Type>::data_ptr;
    friend data_ptr detail::type_access<Type>(const Type&);

public:
    using id_type = type_id;

    type_base() = default;

    explicit type_base(data_ptr data) : data_{data} {}

    type_base(type_base&&) noexcept = default;
    type_base(const type_base&) = default;

    type_base& operator=(type_base&&) noexcept = default;
    type_base& operator=(const type_base&) = default;

    [[nodiscard]] bool is_valid() const noexcept { return data_ != nullptr; }

    [[nodiscard]] explicit operator bool() const noexcept { return is_valid(); }

    [[nodiscard]] id_type get_id() const noexcept { return id_type{data_}; }

    [[nodiscard]] type_kind get_kind() const noexcept { return data_->kind; }

    [[nodiscard]] const metadata_map& get_metadata() const noexcept { return data_->metadata; }

protected:
    ~type_base() = default;

    data_ptr data_{};
};
}  // namespace ME::meta::r

namespace ME::meta::r {
class any_type final : public type_base<any_type> {
public:
    using type_base<any_type>::type_base;

    template <detail::type_family Type>
    any_type(const Type& other) noexcept;

    template <detail::type_family Type>
    [[nodiscard]] bool is() const noexcept;
    [[nodiscard]] bool is(type_kind kind) const noexcept;

    template <detail::type_family Type>
    [[nodiscard]] Type as() const noexcept;

    [[nodiscard]] bool is_array() const noexcept;
    [[nodiscard]] bool is_class() const noexcept;
    [[nodiscard]] bool is_constructor() const noexcept;
    [[nodiscard]] bool is_destructor() const noexcept;
    [[nodiscard]] bool is_enum() const noexcept;
    [[nodiscard]] bool is_function() const noexcept;
    [[nodiscard]] bool is_member() const noexcept;
    [[nodiscard]] bool is_method() const noexcept;
    [[nodiscard]] bool is_nullptr() const noexcept;
    [[nodiscard]] bool is_number() const noexcept;
    [[nodiscard]] bool is_pointer() const noexcept;
    [[nodiscard]] bool is_reference() const noexcept;
    [[nodiscard]] bool is_void() const noexcept;

    [[nodiscard]] array_type as_array() const noexcept;
    [[nodiscard]] class_type as_class() const noexcept;
    [[nodiscard]] constructor_type as_constructor() const noexcept;
    [[nodiscard]] destructor_type as_destructor() const noexcept;
    [[nodiscard]] enum_type as_enum() const noexcept;
    [[nodiscard]] function_type as_function() const noexcept;
    [[nodiscard]] member_type as_member() const noexcept;
    [[nodiscard]] method_type as_method() const noexcept;
    [[nodiscard]] nullptr_type as_nullptr() const noexcept;
    [[nodiscard]] number_type as_number() const noexcept;
    [[nodiscard]] pointer_type as_pointer() const noexcept;
    [[nodiscard]] reference_type as_reference() const noexcept;
    [[nodiscard]] void_type as_void() const noexcept;
};

class array_type final : public type_base<array_type> {
public:
    using type_base<array_type>::type_base;
    [[nodiscard]] array_bitflags get_flags() const noexcept;

    [[nodiscard]] std::size_t get_extent() const noexcept;
    [[nodiscard]] any_type get_data_type() const noexcept;
};

class class_type final : public type_base<class_type> {
public:
    using type_base<class_type>::type_base;
    [[nodiscard]] class_bitflags get_flags() const noexcept;

    [[nodiscard]] std::size_t get_size() const noexcept;
    [[nodiscard]] std::size_t get_align() const noexcept;

    [[nodiscard]] std::size_t get_arity() const noexcept;
    [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
    [[nodiscard]] const any_type_list& get_argument_types() const noexcept;

    [[nodiscard]] const class_list& get_base_classes() const noexcept;
    [[nodiscard]] const class_list& get_derived_classes() const noexcept;
    [[nodiscard]] const constructor_list& get_constructors() const noexcept;
    [[nodiscard]] const destructor_list& get_destructors() const noexcept;
    [[nodiscard]] const function_list& get_functions() const noexcept;
    [[nodiscard]] const member_list& get_members() const noexcept;
    [[nodiscard]] const method_list& get_methods() const noexcept;
    [[nodiscard]] const typedef_map& get_typedefs() const noexcept;
    [[nodiscard]] const variable_list& get_variables() const noexcept;

    template <typename... Args>
    [[nodiscard]] uvalue create(Args&&... args) const;
    template <typename... Args>
    uvalue create_at(void* mem, Args&&... args) const;

    template <typename Arg>
    bool destroy(Arg&& arg) const;
    bool destroy_at(void* mem) const;

    template <detail::class_kind Derived>
    [[nodiscard]] bool is_base_of() const noexcept;
    [[nodiscard]] bool is_base_of(const class_type& derived) const noexcept;

    template <detail::class_kind Derived>
    [[nodiscard]] bool is_direct_base_of() const noexcept;
    [[nodiscard]] bool is_direct_base_of(const class_type& derived) const noexcept;

    template <detail::class_kind Derived>
    [[nodiscard]] bool is_virtual_base_of() const noexcept;
    [[nodiscard]] bool is_virtual_base_of(const class_type& derived) const noexcept;

    template <detail::class_kind Base>
    [[nodiscard]] bool is_derived_from() const noexcept;
    [[nodiscard]] bool is_derived_from(const class_type& base) const noexcept;

    template <detail::class_kind Base>
    [[nodiscard]] bool is_direct_derived_from() const noexcept;
    [[nodiscard]] bool is_direct_derived_from(const class_type& base) const noexcept;

    template <detail::class_kind Base>
    [[nodiscard]] bool is_virtual_derived_from() const noexcept;
    [[nodiscard]] bool is_virtual_derived_from(const class_type& base) const noexcept;

    [[nodiscard]] function get_function(std::string_view name, bool recursively = true) const noexcept;
    [[nodiscard]] member get_member(std::string_view name, bool recursively = true) const noexcept;
    [[nodiscard]] method get_method(std::string_view name, bool recursively = true) const noexcept;
    [[nodiscard]] any_type get_typedef(std::string_view name, bool recursively = true) const noexcept;
    [[nodiscard]] variable get_variable(std::string_view name, bool recursively = true) const noexcept;

    template <typename... Args>
    [[nodiscard]] constructor get_constructor_with() const noexcept;
    template <typename Iter>
    [[nodiscard]] constructor get_constructor_with(Iter first, Iter last) const noexcept;
    [[nodiscard]] constructor get_constructor_with(std::span<const any_type> args) const noexcept;
    [[nodiscard]] constructor get_constructor_with(std::initializer_list<any_type> args) const noexcept;

    [[nodiscard]] destructor get_destructor() const noexcept;

    template <typename... Args>
    [[nodiscard]] function get_function_with(  //
            std::string_view name, bool recursively = true) const noexcept;

    template <typename Iter>
    [[nodiscard]] function get_function_with(  //
            std::string_view name, Iter first, Iter last, bool recursively = true) const noexcept;

    [[nodiscard]] function get_function_with(  //
            std::string_view name, std::span<const any_type> args, bool recursively = true) const noexcept;

    [[nodiscard]] function get_function_with(  //
            std::string_view name, std::initializer_list<any_type> args, bool recursively = true) const noexcept;

    template <typename... Args>
    [[nodiscard]] method get_method_with(  //
            std::string_view name, bool recursively = true) const noexcept;

    template <typename Iter>
    [[nodiscard]] method get_method_with(  //
            std::string_view name, Iter first, Iter last, bool recursively = true) const noexcept;

    [[nodiscard]] method get_method_with(  //
            std::string_view name, std::span<const any_type> args, bool recursively = true) const noexcept;

    [[nodiscard]] method get_method_with(  //
            std::string_view name, std::initializer_list<any_type> args, bool recursively = true) const noexcept;
};

class constructor_type final : public type_base<constructor_type> {
public:
    using type_base<constructor_type>::type_base;
    [[nodiscard]] constructor_bitflags get_flags() const noexcept;

    [[nodiscard]] std::size_t get_arity() const noexcept;
    [[nodiscard]] class_type get_owner_type() const noexcept;
    [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
    [[nodiscard]] const any_type_list& get_argument_types() const noexcept;
};

class destructor_type final : public type_base<destructor_type> {
public:
    using type_base<destructor_type>::type_base;
    [[nodiscard]] destructor_bitflags get_flags() const noexcept;

    [[nodiscard]] class_type get_owner_type() const noexcept;
};

class enum_type final : public type_base<enum_type> {
public:
    using type_base<enum_type>::type_base;
    [[nodiscard]] enum_bitflags get_flags() const noexcept;

    [[nodiscard]] number_type get_underlying_type() const noexcept;

    [[nodiscard]] const evalue_list& get_evalues() const noexcept;

    [[nodiscard]] evalue get_evalue(std::string_view name) const noexcept;

    template <detail::enum_kind Enum>
    [[nodiscard]] std::string_view value_to_name(Enum value) const noexcept;
    [[nodiscard]] uvalue name_to_value(std::string_view name) const noexcept;
};

class function_type final : public type_base<function_type> {
public:
    using type_base<function_type>::type_base;
    [[nodiscard]] function_bitflags get_flags() const noexcept;

    [[nodiscard]] std::size_t get_arity() const noexcept;
    [[nodiscard]] any_type get_return_type() const noexcept;
    [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
    [[nodiscard]] const any_type_list& get_argument_types() const noexcept;
};

class member_type final : public type_base<member_type> {
public:
    using type_base<member_type>::type_base;
    [[nodiscard]] member_bitflags get_flags() const noexcept;

    [[nodiscard]] class_type get_owner_type() const noexcept;
    [[nodiscard]] any_type get_value_type() const noexcept;
};

class method_type final : public type_base<method_type> {
public:
    using type_base<method_type>::type_base;
    [[nodiscard]] method_bitflags get_flags() const noexcept;

    [[nodiscard]] std::size_t get_arity() const noexcept;
    [[nodiscard]] class_type get_owner_type() const noexcept;
    [[nodiscard]] any_type get_return_type() const noexcept;
    [[nodiscard]] any_type get_argument_type(std::size_t position) const noexcept;
    [[nodiscard]] const any_type_list& get_argument_types() const noexcept;
};

class nullptr_type final : public type_base<nullptr_type> {
public:
    using type_base<nullptr_type>::type_base;
};

class number_type final : public type_base<number_type> {
public:
    using type_base<number_type>::type_base;
    [[nodiscard]] number_bitflags get_flags() const noexcept;

    [[nodiscard]] std::size_t get_size() const noexcept;
    [[nodiscard]] std::size_t get_align() const noexcept;
};

class pointer_type final : public type_base<pointer_type> {
public:
    using type_base<pointer_type>::type_base;
    [[nodiscard]] pointer_bitflags get_flags() const noexcept;

    [[nodiscard]] any_type get_data_type() const noexcept;
};

class reference_type final : public type_base<reference_type> {
public:
    using type_base<reference_type>::type_base;
    [[nodiscard]] reference_bitflags get_flags() const noexcept;

    [[nodiscard]] any_type get_data_type() const noexcept;
};

class void_type final : public type_base<void_type> {
public:
    using type_base<void_type>::type_base;
};
}  // namespace ME::meta::r

namespace std {
template <ME::meta::r::detail::type_family Type>
struct hash<Type> {
    size_t operator()(const Type& type) const noexcept { return type.is_valid() ? type.get_id().get_hash() : 0; }
};
}  // namespace std

namespace ME::meta::r {
template <detail::type_family TypeL, detail::type_family TypeR>
[[nodiscard]] bool operator==(const TypeL& l, const TypeR& r) noexcept {
    return l.is_valid() == r.is_valid() && (!l.is_valid() || l.get_id() == r.get_id());
}

template <detail::type_family TypeL, detail::type_family TypeR>
[[nodiscard]] std::strong_ordering operator<=>(const TypeL& l, const TypeR& r) noexcept {
    if (const std::strong_ordering cmp{l.is_valid() <=> r.is_valid()}; cmp != std::strong_ordering::equal) {
        return cmp;
    }
    return l.is_valid() ? l.get_id() <=> r.get_id() : std::strong_ordering::equal;
}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::type_family Type>
[[nodiscard]] bool operator==(const Type& l, const typename Type::id_type& r) noexcept {
    return l.is_valid() && l.get_id() == r;
}

template <detail::type_family Type>
[[nodiscard]] std::strong_ordering operator<=>(const Type& l, const typename Type::id_type& r) noexcept {
    return l.is_valid() ? l.get_id() <=> r : std::strong_ordering::less;
}
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
struct type_data_base {
    // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
    const type_kind kind;
    // NOLINTEND(*-avoid-const-or-ref-data-members)

    metadata_map metadata{};

    explicit type_data_base(type_kind nkind) : kind{nkind} {}

    type_data_base(type_data_base&&) = delete;
    type_data_base(const type_data_base&) = delete;
    type_data_base& operator=(type_data_base&&) = delete;
    type_data_base& operator=(const type_data_base&) = delete;

protected:
    ~type_data_base() = default;
};

struct array_type_data final : type_data_base {
    // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
    const array_bitflags flags;
    const std::size_t extent;
    const any_type data_type;
    // NOLINTEND(*-avoid-const-or-ref-data-members)

    template <array_kind Array>
    explicit array_type_data(type_list<Array>);
};

struct class_type_data final : type_data_base {
    // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
    const class_bitflags flags;
    const std::size_t size;
    const std::size_t align;
    const any_type_list argument_types;
    // NOLINTEND(*-avoid-const-or-ref-data-members)

    class_list base_classes;
    class_list derived_classes;
    constructor_list constructors;
    destructor_list destructors;
    function_list functions;
    member_list members;
    method_list methods;
    typedef_map typedefs;
    variable_list variables;

    struct upcast_func_t final {
        using upcast_t = void* (*)(void*);

        upcast_t upcast{};
        class_type target{};
        bool is_virtual{};

        template <typename Derived, typename Base>
            requires std::is_base_of_v<Base, Derived>
        upcast_func_t(std::in_place_type_t<Derived>, std::in_place_type_t<Base>);

        [[nodiscard]] void* apply(void* ptr) const noexcept;
        [[nodiscard]] const void* apply(const void* ptr) const noexcept;
    };

    struct upcast_func_list_t final {
        using upcasts_t = std::vector<upcast_func_t>;
        using vbases_t = std::set<class_type, std::less<>>;

        upcasts_t upcasts{};
        vbases_t vbases{};
        bool is_ambiguous{};

        upcast_func_list_t(const upcast_func_t& _upcast);
        upcast_func_list_t(upcasts_t _upcasts, vbases_t _vbases);

        [[nodiscard]] void* apply(void* ptr) const noexcept;
        [[nodiscard]] const void* apply(const void* ptr) const noexcept;

        friend upcast_func_list_t operator+(const upcast_func_list_t& l, const upcast_func_list_t& r);
    };

    using base_upcasts_t = std::map<class_type, upcast_func_t, std::less<>>;
    using deep_upcasts_t = std::map<class_type, upcast_func_list_t, std::less<>>;

    base_upcasts_t base_upcasts;
    deep_upcasts_t deep_upcasts;

    template <class_kind Class>
    explicit class_type_data(type_list<Class>);
};

struct constructor_type_data final : type_data_base {
    // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
    const constructor_bitflags flags;
    const class_type owner_type;
    const any_type_list argument_types;
    // NOLINTEND(*-avoid-const-or-ref-data-members)

    template <class_kind Class, typename... Args>
    explicit constructor_type_data(type_list<Class>, type_list<Args...>);
};

struct destructor_type_data final : type_data_base {
    // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
    const destructor_bitflags flags;
    const class_type owner_type;
    // NOLINTEND(*-avoid-const-or-ref-data-members)

    template <class_kind Class>
    explicit destructor_type_data(type_list<Class>);
};

struct enum_type_data final : type_data_base {
    // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
    const enum_bitflags flags;
    const number_type underlying_type;
    // NOLINTEND(*-avoid-const-or-ref-data-members)

    evalue_list evalues;

    template <enum_kind Enum>
    explicit enum_type_data(type_list<Enum>);
};

struct function_type_data final : type_data_base {
    // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
    const function_bitflags flags;
    const any_type return_type;
    const any_type_list argument_types;
    // NOLINTEND(*-avoid-const-or-ref-data-members)

    template <function_kind Function>
    explicit function_type_data(type_list<Function>);
};

struct member_type_data final : type_data_base {
    // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
    const member_bitflags flags;
    const class_type owner_type;
    const any_type value_type;
    // NOLINTEND(*-avoid-const-or-ref-data-members)

    template <member_pointer_kind Member>
    explicit member_type_data(type_list<Member>);
};

struct method_type_data final : type_data_base {
    // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
    const method_bitflags flags;
    const class_type owner_type;
    const any_type return_type;
    const any_type_list argument_types;
    // NOLINTEND(*-avoid-const-or-ref-data-members)

    template <method_pointer_kind Method>
    explicit method_type_data(type_list<Method>);
};

struct nullptr_type_data final : type_data_base {
    template <nullptr_kind Nullptr>
    explicit nullptr_type_data(type_list<Nullptr>);
};

struct number_type_data final : type_data_base {
    // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
    const number_bitflags flags;
    const std::size_t size;
    const std::size_t align;
    // NOLINTEND(*-avoid-const-or-ref-data-members)

    template <number_kind Number>
    explicit number_type_data(type_list<Number>);
};

struct pointer_type_data final : type_data_base {
    // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
    const pointer_bitflags flags;
    const any_type data_type;
    // NOLINTEND(*-avoid-const-or-ref-data-members)

    template <pointer_kind Pointer>
    explicit pointer_type_data(type_list<Pointer>);
};

struct reference_type_data final : type_data_base {
    // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
    const reference_bitflags flags;
    const any_type data_type;
    // NOLINTEND(*-avoid-const-or-ref-data-members)

    template <reference_kind Reference>
    explicit reference_type_data(type_list<Reference>);
};

struct void_type_data final : type_data_base {
    template <void_kind Void>
    explicit void_type_data(type_list<Void>);
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
class argument_index final {
public:
    argument_index() = delete;
    explicit argument_index(any_type type, std::size_t position);

    [[nodiscard]] any_type get_type() const noexcept;
    [[nodiscard]] std::size_t get_position() const noexcept;

    void swap(argument_index& other) noexcept;
    [[nodiscard]] std::size_t get_hash() const noexcept;
    [[nodiscard]] std::strong_ordering operator<=>(const argument_index&) const = default;

private:
    any_type type_;
    std::size_t position_{};
};

class constructor_index final {
public:
    constructor_index() = delete;
    explicit constructor_index(constructor_type type);

    [[nodiscard]] constructor_type get_type() const noexcept;

    void swap(constructor_index& other) noexcept;
    [[nodiscard]] std::size_t get_hash() const noexcept;
    [[nodiscard]] std::strong_ordering operator<=>(const constructor_index&) const = default;

private:
    constructor_type type_;
};

class destructor_index final {
public:
    destructor_index() = delete;
    explicit destructor_index(destructor_type type);

    [[nodiscard]] destructor_type get_type() const noexcept;

    void swap(destructor_index& other) noexcept;
    [[nodiscard]] std::size_t get_hash() const noexcept;
    [[nodiscard]] std::strong_ordering operator<=>(const destructor_index&) const = default;

private:
    destructor_type type_;
};

class evalue_index final {
public:
    evalue_index() = delete;
    explicit evalue_index(enum_type type, std::string name);

    [[nodiscard]] enum_type get_type() const noexcept;

    [[nodiscard]] std::string&& get_name() && noexcept;
    [[nodiscard]] const std::string& get_name() const& noexcept;

    void swap(evalue_index& other) noexcept;
    [[nodiscard]] std::size_t get_hash() const noexcept;
    [[nodiscard]] std::strong_ordering operator<=>(const evalue_index&) const = default;

private:
    enum_type type_;
    std::string name_;
};

class function_index final {
public:
    function_index() = delete;
    explicit function_index(function_type type, std::string name);

    [[nodiscard]] function_type get_type() const noexcept;

    [[nodiscard]] std::string&& get_name() && noexcept;
    [[nodiscard]] const std::string& get_name() const& noexcept;

    void swap(function_index& other) noexcept;
    [[nodiscard]] std::size_t get_hash() const noexcept;
    [[nodiscard]] std::strong_ordering operator<=>(const function_index&) const = default;

private:
    function_type type_;
    std::string name_;
};

class member_index final {
public:
    member_index() = delete;
    explicit member_index(member_type type, std::string name);

    [[nodiscard]] member_type get_type() const noexcept;

    [[nodiscard]] std::string&& get_name() && noexcept;
    [[nodiscard]] const std::string& get_name() const& noexcept;

    void swap(member_index& other) noexcept;
    [[nodiscard]] std::size_t get_hash() const noexcept;
    [[nodiscard]] std::strong_ordering operator<=>(const member_index&) const = default;

private:
    member_type type_;
    std::string name_;
};

class method_index final {
public:
    method_index() = delete;
    explicit method_index(method_type type, std::string name);

    [[nodiscard]] method_type get_type() const noexcept;

    [[nodiscard]] std::string&& get_name() && noexcept;
    [[nodiscard]] const std::string& get_name() const& noexcept;

    void swap(method_index& other) noexcept;
    [[nodiscard]] std::size_t get_hash() const noexcept;
    [[nodiscard]] std::strong_ordering operator<=>(const method_index&) const = default;

private:
    method_type type_;
    std::string name_;
};

class scope_index final {
public:
    scope_index() = delete;
    explicit scope_index(std::string name);

    [[nodiscard]] std::string&& get_name() && noexcept;
    [[nodiscard]] const std::string& get_name() const& noexcept;

    void swap(scope_index& other) noexcept;
    [[nodiscard]] std::size_t get_hash() const noexcept;
    [[nodiscard]] std::strong_ordering operator<=>(const scope_index&) const = default;

private:
    std::string name_;
};

class variable_index final {
public:
    variable_index() = delete;
    explicit variable_index(pointer_type type, std::string name);

    [[nodiscard]] pointer_type get_type() const noexcept;

    [[nodiscard]] std::string&& get_name() && noexcept;
    [[nodiscard]] const std::string& get_name() const& noexcept;

    void swap(variable_index& other) noexcept;
    [[nodiscard]] std::size_t get_hash() const noexcept;
    [[nodiscard]] std::strong_ordering operator<=>(const variable_index&) const = default;

private:
    pointer_type type_;
    std::string name_;
};

template <detail::index_family Index>
void swap(Index& l, Index& r) noexcept {
    l.swap(r);
}
}  // namespace ME::meta::r

namespace std {
template <ME::meta::r::detail::index_family Index>
struct hash<Index> {
    size_t operator()(const Index& index) const noexcept { return index.get_hash(); }
};
}  // namespace std

namespace ME::meta::r {
class uvalue final {
public:
    uvalue() = default;
    ~uvalue() noexcept;

    uvalue(uvalue&& other) noexcept;
    uvalue(const uvalue& other);

    uvalue& operator=(uvalue&& other) noexcept;
    uvalue& operator=(const uvalue& other);

    template <                                          //
            typename T,                                 //
            typename Tp = std::decay_t<T>,              //
            typename = std::enable_if_t<                //
                    !detail::is_in_place_type_v<Tp> &&  //
                    detail::non_uvalue_family<Tp> &&    //
                    std::is_copy_constructible_v<Tp>>>  //
    uvalue(T&& val);

    template <                                          //
            typename T,                                 //
            typename Tp = std::decay_t<T>,              //
            typename = std::enable_if_t<                //
                    detail::non_uvalue_family<Tp> &&    //
                    std::is_copy_constructible_v<Tp>>>  //
    uvalue& operator=(T&& val);

    template <typename T, typename... Args, typename Tp = std::decay_t<T>>
        requires std::is_copy_constructible_v<Tp>         //
                 && std::is_constructible_v<Tp, Args...>  //
    explicit uvalue(std::in_place_type_t<T>, Args&&... args);

    template <typename T, typename U, typename... Args, typename Tp = std::decay_t<T>>
        requires std::is_copy_constructible_v<Tp>                                    //
                 && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...>  //
    explicit uvalue(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args);

    template <typename T, typename... Args, typename Tp = std::decay_t<T>>
        requires std::is_copy_constructible_v<Tp>         //
                 && std::is_constructible_v<Tp, Args...>  //
    Tp& emplace(Args&&... args);

    template <typename T, typename U, typename... Args, typename Tp = std::decay_t<T>>
        requires std::is_copy_constructible_v<Tp>                                    //
                 && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...>  //
    Tp& emplace(std::initializer_list<U> ilist, Args&&... args);

    [[nodiscard]] bool has_value() const noexcept;
    [[nodiscard]] explicit operator bool() const noexcept;

    void reset() noexcept;
    void swap(uvalue& other) noexcept;

    [[nodiscard]] any_type get_type() const noexcept;

    [[nodiscard]] void* get_data() noexcept;
    [[nodiscard]] const void* get_data() const noexcept;
    [[nodiscard]] const void* get_cdata() const noexcept;

    [[nodiscard]] uvalue operator*() const;
    [[nodiscard]] bool has_deref_op() const noexcept;

    [[nodiscard]] uvalue operator[](std::size_t index) const;
    [[nodiscard]] bool has_index_op() const noexcept;

    [[nodiscard]] uvalue unmap() const;
    [[nodiscard]] bool has_unmap_op() const noexcept;

    template <typename T>
    [[nodiscard]] bool is() const noexcept;

    template <detail::pointer_kind T>
    [[nodiscard]] T as();
    template <detail::pointer_kind T>
    [[nodiscard]] T as() const;

    template <detail::non_pointer_kind T>
    [[nodiscard]] T as() &&;
    template <detail::non_pointer_kind T>
    [[nodiscard]] T& as() &;
    template <detail::non_pointer_kind T>
    [[nodiscard]] const T& as() const&;
    template <detail::non_pointer_kind T>
    [[nodiscard]] const T&& as() const&&;

    template <detail::pointer_kind T>
    [[nodiscard]] T try_as() noexcept;
    template <detail::pointer_kind T>
    [[nodiscard]] T try_as() const noexcept;

    template <detail::non_pointer_kind T>
    [[nodiscard]] T* try_as() noexcept;
    template <detail::non_pointer_kind T>
    [[nodiscard]] const T* try_as() const noexcept;

private:
    struct vtable_t;

    struct alignas(std::max_align_t) internal_storage_t final {
        // NOLINTNEXTLINE(*-avoid-c-arrays)
        std::byte data[sizeof(void*) * 2];
    };

    struct external_storage_t final {
        // NOLINTNEXTLINE(*-avoid-c-arrays)
        std::byte padding[sizeof(internal_storage_t) - sizeof(void*)];
        void* ptr;
    };

    enum class storage_e : std::uintptr_t {
        nothing,
        trivial,
        internal,
        external,
    };

    struct storage_u final {
        union {
            internal_storage_t internal;
            external_storage_t external;
        };

        std::uintptr_t vtag;
    } storage_{};

    static_assert(std::is_standard_layout_v<storage_u>);
    static_assert(alignof(storage_u) == alignof(std::max_align_t));
    static_assert(sizeof(internal_storage_t) == sizeof(external_storage_t));
};

inline void swap(uvalue& l, uvalue& r) noexcept { l.swap(r); }

template <typename T, typename... Args>
uvalue make_uvalue(Args&&... args) {
    return uvalue(std::in_place_type<T>, std::forward<Args>(args)...);
}

template <typename T, typename U, typename... Args>
uvalue make_uvalue(std::initializer_list<U> ilist, Args&&... args) {
    return uvalue(std::in_place_type<T>, ilist, std::forward<Args>(args)...);
}
}  // namespace ME::meta::r

namespace ME::meta::r {
class uerror final {
public:
    uerror() = default;
    ~uerror() = default;

    uerror(uerror&&) noexcept = default;
    uerror(const uerror&) noexcept = default;

    uerror& operator=(uerror&&) noexcept = default;
    uerror& operator=(const uerror&) noexcept = default;

    explicit uerror(error_code error) noexcept;
    uerror& operator=(error_code error) noexcept;

    [[nodiscard]] bool has_error() const noexcept;
    [[nodiscard]] explicit operator bool() const noexcept;

    [[nodiscard]] error_code operator*() const noexcept;
    [[nodiscard]] error_code get_error() const noexcept;

    void reset() noexcept;
    void swap(uerror& other) noexcept;

    [[nodiscard]] std::size_t get_hash() const noexcept;
    [[nodiscard]] std::strong_ordering operator<=>(const uerror& other) const = default;

private:
    error_code error_{error_code::no_error};
};

inline void swap(uerror& l, uerror& r) noexcept { l.swap(r); }

inline uerror make_uerror(error_code error) { return uerror{error}; }
}  // namespace ME::meta::r

namespace std {
template <>
struct hash<ME::meta::r::uerror> {
    size_t operator()(ME::meta::r::uerror ue) const noexcept { return ue.get_hash(); }
};
}  // namespace std

namespace ME::meta::r {
class uresult final {
public:
    uresult() = default;
    ~uresult() = default;

    uresult(uresult&&) noexcept = default;
    uresult(const uresult&) = default;

    uresult& operator=(uresult&&) noexcept = default;
    uresult& operator=(const uresult&) = default;

    explicit(false) uresult(uerror error) noexcept;
    explicit(false) uresult(uvalue value) noexcept;

    uresult& operator=(uerror error) noexcept;
    uresult& operator=(uvalue value) noexcept;

    template <                                          //
            typename T,                                 //
            typename Tp = std::decay_t<T>,              //
            typename = std::enable_if_t<                //
                    !detail::is_in_place_type_v<Tp> &&  //
                    detail::non_uvalue_family<Tp> &&    //
                    std::is_copy_constructible_v<Tp>>>  //
    uresult(T&& val);

    template <                                          //
            typename T,                                 //
            typename Tp = std::decay_t<T>,              //
            typename = std::enable_if_t<                //
                    detail::non_uvalue_family<Tp> &&    //
                    std::is_copy_constructible_v<Tp>>>  //
    uresult& operator=(T&& val);

    template <typename T, typename... Args, typename Tp = std::decay_t<T>>
        requires std::is_copy_constructible_v<Tp>         //
                 && std::is_constructible_v<Tp, Args...>  //
    explicit uresult(std::in_place_type_t<T>, Args&&... args);

    template <typename T, typename U, typename... Args, typename Tp = std::decay_t<T>>
        requires std::is_copy_constructible_v<Tp>                                    //
                 && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...>  //
    explicit uresult(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args);

    template <typename T, typename... Args, typename Tp = std::decay_t<T>>
        requires std::is_copy_constructible_v<Tp>         //
                 && std::is_constructible_v<Tp, Args...>  //
    Tp& emplace(Args&&... args);

    template <typename T, typename U, typename... Args, typename Tp = std::decay_t<T>>
        requires std::is_copy_constructible_v<Tp>                                    //
                 && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...>  //
    Tp& emplace(std::initializer_list<U> ilist, Args&&... args);

    [[nodiscard]] bool has_error() const noexcept;
    [[nodiscard]] bool has_value() const noexcept;
    [[nodiscard]] explicit operator bool() const noexcept;

    [[nodiscard]] uvalue* operator->() noexcept;
    [[nodiscard]] const uvalue* operator->() const noexcept;

    [[nodiscard]] uvalue& operator*() & noexcept;
    [[nodiscard]] uvalue&& operator*() && noexcept;
    [[nodiscard]] const uvalue& operator*() const& noexcept;
    [[nodiscard]] const uvalue&& operator*() const&& noexcept;

    [[nodiscard]] uvalue& get_value() & noexcept;
    [[nodiscard]] uvalue&& get_value() && noexcept;
    [[nodiscard]] const uvalue& get_value() const& noexcept;
    [[nodiscard]] const uvalue&& get_value() const&& noexcept;

    [[nodiscard]] error_code get_error() const noexcept;

    void reset() noexcept;
    void swap(uresult& other) noexcept;

private:
    uvalue value_{};
    error_code error_{error_code::no_error};
};

inline void swap(uresult& l, uresult& r) noexcept { l.swap(r); }

template <typename T, typename... Args>
uresult make_uresult(Args&&... args) {
    return uresult(std::in_place_type<T>, std::forward<Args>(args)...);
}

template <typename T, typename U, typename... Args>
uresult make_uresult(std::initializer_list<U> ilist, Args&&... args) {
    return uresult(std::in_place_type<T>, ilist, std::forward<Args>(args)...);
}
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <state_family State>
struct state_traits;

template <state_family State>
[[nodiscard]] typename state_traits<State>::state_ptr state_access(const State& state) {
    return state.state_;
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <>
struct state_traits<argument> {
    using index_type = argument_index;
    using state_ptr = argument_state_ptr;
};

template <>
struct state_traits<constructor> {
    using index_type = constructor_index;
    using state_ptr = constructor_state_ptr;
};

template <>
struct state_traits<destructor> {
    using index_type = destructor_index;
    using state_ptr = destructor_state_ptr;
};

template <>
struct state_traits<evalue> {
    using index_type = evalue_index;
    using state_ptr = evalue_state_ptr;
};

template <>
struct state_traits<function> {
    using index_type = function_index;
    using state_ptr = function_state_ptr;
};

template <>
struct state_traits<member> {
    using index_type = member_index;
    using state_ptr = member_state_ptr;
};

template <>
struct state_traits<method> {
    using index_type = method_index;
    using state_ptr = method_state_ptr;
};

template <>
struct state_traits<scope> {
    using index_type = scope_index;
    using state_ptr = scope_state_ptr;
};

template <>
struct state_traits<variable> {
    using index_type = variable_index;
    using state_ptr = variable_state_ptr;
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
namespace constructor_policy {
struct as_object_t final {};

struct as_raw_pointer_t final {};

struct as_shared_pointer_t final {};

inline constexpr as_object_t as_object{};
inline constexpr as_raw_pointer_t as_raw_pointer{};
inline constexpr as_shared_pointer_t as_shared_pointer{};
}  // namespace constructor_policy

namespace function_policy {
struct as_copy_t final {};

struct discard_return_t final {};

struct return_reference_as_pointer_t final {};

inline constexpr as_copy_t as_copy{};
inline constexpr discard_return_t discard_return{};
inline constexpr return_reference_as_pointer_t return_reference_as_pointer{};
}  // namespace function_policy

namespace member_policy {
struct as_copy_t final {};

struct as_pointer_t final {};

struct as_reference_wrapper_t final {};

inline constexpr as_copy_t as_copy{};
inline constexpr as_pointer_t as_pointer{};
inline constexpr as_reference_wrapper_t as_reference_wrapper{};
}  // namespace member_policy

namespace method_policy {
struct as_copy_t final {};

struct discard_return_t final {};

struct return_reference_as_pointer_t final {};

inline constexpr as_copy_t as_copy{};
inline constexpr discard_return_t discard_return{};
inline constexpr return_reference_as_pointer_t return_reference_as_pointer{};
}  // namespace method_policy

namespace variable_policy {
struct as_copy_t final {};

struct as_pointer_t final {};

struct as_reference_wrapper_t final {};

inline constexpr as_copy_t as_copy{};
inline constexpr as_pointer_t as_pointer{};
inline constexpr as_reference_wrapper_t as_reference_wrapper{};
}  // namespace variable_policy

template <typename Policy>
concept constructor_policy_family                                          //
        = std::is_same_v<Policy, constructor_policy::as_object_t>          //
          || std::is_same_v<Policy, constructor_policy::as_raw_pointer_t>  //
          || std::is_same_v<Policy, constructor_policy::as_shared_pointer_t>;

template <typename Policy>
concept function_policy_family                                          //
        = std::is_same_v<Policy, function_policy::as_copy_t>            //
          || std::is_same_v<Policy, function_policy::discard_return_t>  //
          || std::is_same_v<Policy, function_policy::return_reference_as_pointer_t>;

template <typename Policy>
concept member_policy_family                                      //
        = std::is_same_v<Policy, member_policy::as_copy_t>        //
          || std::is_same_v<Policy, member_policy::as_pointer_t>  //
          || std::is_same_v<Policy, member_policy::as_reference_wrapper_t>;

template <typename Policy>
concept method_policy_family                                          //
        = std::is_same_v<Policy, method_policy::as_copy_t>            //
          || std::is_same_v<Policy, method_policy::discard_return_t>  //
          || std::is_same_v<Policy, method_policy::return_reference_as_pointer_t>;

template <typename Policy>
concept variable_policy_family                                      //
        = std::is_same_v<Policy, variable_policy::as_copy_t>        //
          || std::is_same_v<Policy, variable_policy::as_pointer_t>  //
          || std::is_same_v<Policy, variable_policy::as_reference_wrapper_t>;
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::state_family State>
class state_base {
    using state_ptr = typename detail::state_traits<State>::state_ptr;
    friend state_ptr detail::state_access<State>(const State&);

public:
    using index_type = typename detail::state_traits<State>::index_type;

    state_base() = default;

    explicit state_base(state_ptr state) : state_{state} {}

    state_base(state_base&&) noexcept = default;
    state_base(const state_base&) = default;

    state_base& operator=(state_base&&) noexcept = default;
    state_base& operator=(const state_base&) = default;

    [[nodiscard]] bool is_valid() const noexcept { return state_ != nullptr; }

    [[nodiscard]] explicit operator bool() const noexcept { return is_valid(); }

    [[nodiscard]] const index_type& get_index() const noexcept { return state_->index; }

    [[nodiscard]] const metadata_map& get_metadata() const noexcept { return state_->metadata; }

protected:
    ~state_base() = default;

    state_ptr state_{};
};
}  // namespace ME::meta::r

namespace ME::meta::r {
class argument final : public state_base<argument> {
public:
    using state_base<argument>::state_base;

    [[nodiscard]] any_type get_type() const noexcept;
    [[nodiscard]] std::size_t get_position() const noexcept;

    [[nodiscard]] const std::string& get_name() const noexcept;
};

class constructor final : public state_base<constructor> {
public:
    using state_base<constructor>::state_base;

    [[nodiscard]] constructor_type get_type() const noexcept;

    template <typename... Args>
    [[nodiscard]] uvalue create(Args&&... args) const;

    template <typename... Args>
    [[nodiscard]] uresult try_create(Args&&... args) const;

    template <typename... Args>
    uvalue create_at(void* mem, Args&&... args) const;

    template <typename... Args>
    uresult try_create_at(void* mem, Args&&... args) const;

    template <typename... Args>
    [[nodiscard]] bool is_invocable_with() const noexcept;

    template <typename... Args>
    [[nodiscard]] bool is_invocable_with(Args&&... args) const noexcept;

    template <typename... Args>
    [[nodiscard]] uerror check_invocable_error() const noexcept;

    template <typename... Args>
    [[nodiscard]] uerror check_invocable_error(Args&&... args) const noexcept;

    [[nodiscard]] argument get_argument(std::size_t position) const noexcept;
    [[nodiscard]] const argument_list& get_arguments() const noexcept;
};

class destructor final : public state_base<destructor> {
public:
    using state_base<destructor>::state_base;

    [[nodiscard]] destructor_type get_type() const noexcept;

    template <typename Arg>
    void destroy(Arg&& arg) const;

    template <typename Arg>
    uresult try_destroy(Arg&& arg) const;

    void destroy_at(void* mem) const;

    uresult try_destroy_at(void* mem) const;

    template <typename Arg>
    [[nodiscard]] bool is_invocable_with() const noexcept;

    template <typename Arg>
    [[nodiscard]] bool is_invocable_with(Arg&& arg) const noexcept;

    template <typename Arg>
    [[nodiscard]] uerror check_invocable_error() const noexcept;

    template <typename Arg>
    [[nodiscard]] uerror check_invocable_error(Arg&& arg) const noexcept;
};

class evalue final : public state_base<evalue> {
public:
    using state_base<evalue>::state_base;

    [[nodiscard]] enum_type get_type() const noexcept;
    [[nodiscard]] const std::string& get_name() const noexcept;

    [[nodiscard]] const uvalue& get_value() const noexcept;
    [[nodiscard]] const uvalue& get_underlying_value() const noexcept;
};

class function final : public state_base<function> {
public:
    using state_base<function>::state_base;

    [[nodiscard]] function_type get_type() const noexcept;
    [[nodiscard]] const std::string& get_name() const noexcept;

    template <typename... Args>
    uvalue invoke(Args&&... args) const;

    template <typename... Args>
    uresult try_invoke(Args&&... args) const;

    template <typename... Args>
    uvalue operator()(Args&&... args) const;

    template <typename... Args>
    [[nodiscard]] bool is_invocable_with() const noexcept;

    template <typename... Args>
    [[nodiscard]] bool is_invocable_with(Args&&... args) const noexcept;

    template <typename... Args>
    [[nodiscard]] uerror check_invocable_error() const noexcept;

    template <typename... Args>
    [[nodiscard]] uerror check_invocable_error(Args&&... args) const noexcept;

    [[nodiscard]] argument get_argument(std::size_t position) const noexcept;
    [[nodiscard]] const argument_list& get_arguments() const noexcept;
};

class member final : public state_base<member> {
public:
    using state_base<member>::state_base;

    [[nodiscard]] member_type get_type() const noexcept;
    [[nodiscard]] const std::string& get_name() const noexcept;

    template <typename Instance>
    [[nodiscard]] uvalue get(Instance&& instance) const;

    template <typename Instance>
    [[nodiscard]] uresult try_get(Instance&& instance) const;

    template <typename Instance>
    [[nodiscard]] uvalue operator()(Instance&& instance) const;

    template <typename Instance, typename Value>
    void set(Instance&& instance, Value&& value) const;

    template <typename Instance, typename Value>
    uresult try_set(Instance&& instance, Value&& value) const;

    template <typename Instance, typename Value>
    void operator()(Instance&& instance, Value&& value) const;

    template <typename Instance>
    [[nodiscard]] bool is_gettable_with() const noexcept;

    template <typename Instance>
    [[nodiscard]] bool is_gettable_with(Instance&& instance) const noexcept;

    template <typename Instance, typename Value>
    [[nodiscard]] bool is_settable_with() const noexcept;

    template <typename Instance, typename Value>
    [[nodiscard]] bool is_settable_with(Instance&& instance, Value&& value) const noexcept;

    template <typename Instance>
    [[nodiscard]] uerror check_gettable_error() const noexcept;

    template <typename Instance>
    [[nodiscard]] uerror check_gettable_error(Instance&& instance) const noexcept;

    template <typename Instance, typename Value>
    [[nodiscard]] uerror check_settable_error() const noexcept;

    template <typename Instance, typename Value>
    [[nodiscard]] uerror check_settable_error(Instance&& instance, Value&& value) const noexcept;
};

class method final : public state_base<method> {
public:
    using state_base<method>::state_base;

    [[nodiscard]] method_type get_type() const noexcept;
    [[nodiscard]] const std::string& get_name() const noexcept;

    template <typename Instance, typename... Args>
    uvalue invoke(Instance&& instance, Args&&... args) const;

    template <typename Instance, typename... Args>
    uresult try_invoke(Instance&& instance, Args&&... args) const;

    template <typename Instance, typename... Args>
    uvalue operator()(Instance&& instance, Args&&... args) const;

    template <typename Instance, typename... Args>
    [[nodiscard]] bool is_invocable_with() const noexcept;

    template <typename Instance, typename... Args>
    [[nodiscard]] bool is_invocable_with(Instance&& instance, Args&&... args) const noexcept;

    template <typename Instance, typename... Args>
    [[nodiscard]] uerror check_invocable_error() const noexcept;

    template <typename Instance, typename... Args>
    [[nodiscard]] uerror check_invocable_error(Instance&& instance, Args&&... args) const noexcept;

    [[nodiscard]] argument get_argument(std::size_t position) const noexcept;
    [[nodiscard]] const argument_list& get_arguments() const noexcept;
};

class scope final : public state_base<scope> {
public:
    using state_base<scope>::state_base;

    [[nodiscard]] const std::string& get_name() const noexcept;

    [[nodiscard]] const function_list& get_functions() const noexcept;
    [[nodiscard]] const typedef_map& get_typedefs() const noexcept;
    [[nodiscard]] const variable_list& get_variables() const noexcept;

    [[nodiscard]] function get_function(std::string_view name) const noexcept;
    [[nodiscard]] any_type get_typedef(std::string_view name) const noexcept;
    [[nodiscard]] variable get_variable(std::string_view name) const noexcept;

    template <typename... Args>
    [[nodiscard]] function get_function_with(  //
            std::string_view name) const noexcept;

    template <typename Iter>
    [[nodiscard]] function get_function_with(  //
            std::string_view name, Iter first, Iter last) const noexcept;

    [[nodiscard]] function get_function_with(  //
            std::string_view name, std::span<const any_type> args) const noexcept;

    [[nodiscard]] function get_function_with(  //
            std::string_view name, std::initializer_list<any_type> args) const noexcept;
};

class variable final : public state_base<variable> {
public:
    using state_base<variable>::state_base;

    [[nodiscard]] pointer_type get_type() const noexcept;
    [[nodiscard]] const std::string& get_name() const noexcept;

    [[nodiscard]] uvalue get() const;
    [[nodiscard]] uresult try_get() const;

    [[nodiscard]] uvalue operator()() const;

    template <typename Value>
    void set(Value&& value) const;

    template <typename Value>
    uresult try_set(Value&& value) const;

    template <typename Value>
    void operator()(Value&& value) const;

    template <typename Value>
    [[nodiscard]] bool is_settable_with() const noexcept;

    template <typename Value>
    [[nodiscard]] bool is_settable_with(Value&& value) const noexcept;

    template <typename Value>
    [[nodiscard]] uerror check_settable_error() const noexcept;

    template <typename Value>
    [[nodiscard]] uerror check_settable_error(Value&& value) const noexcept;
};
}  // namespace ME::meta::r

namespace std {
template <ME::meta::r::detail::state_family State>
struct hash<State> {
    size_t operator()(const State& state) const noexcept { return state.is_valid() ? state.get_index().get_hash() : 0; }
};
}  // namespace std

namespace ME::meta::r {
template <detail::state_family State>
[[nodiscard]] bool operator==(const State& l, const State& r) noexcept {
    return l.is_valid() == r.is_valid() && (!l.is_valid() || l.get_index() == r.get_index());
}

template <detail::state_family State>
[[nodiscard]] std::strong_ordering operator<=>(const State& l, const State& r) noexcept {
    if (const std::strong_ordering cmp{l.is_valid() <=> r.is_valid()}; cmp != std::strong_ordering::equal) {
        return cmp;
    }
    return l.is_valid() ? l.get_index() <=> r.get_index() : std::strong_ordering::equal;
}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::state_family State>
[[nodiscard]] bool operator==(const State& l, const typename State::index_type& r) noexcept {
    return l.is_valid() && l.get_index() == r;
}

template <detail::state_family State>
[[nodiscard]] std::strong_ordering operator<=>(const State& l, const typename State::index_type& r) noexcept {
    return l.is_valid() ? l.get_index() <=> r : std::strong_ordering::less;
}
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
struct argument_state final : intrusive_ref_counter<argument_state> {
    argument_index index;
    metadata_map metadata;

    std::string name{};

    template <typename Argument>
    [[nodiscard]] static argument_state_ptr make(std::size_t position, metadata_map metadata);
    explicit argument_state(argument_index index, metadata_map metadata);
};

struct constructor_state final : intrusive_ref_counter<constructor_state> {
    using create_impl = fixed_function<uvalue(std::span<const uarg>)>;
    using create_at_impl = fixed_function<uvalue(void*, std::span<const uarg>)>;
    using create_error_impl = fixed_function<uerror(std::span<const uarg_base>)>;

    constructor_index index;
    metadata_map metadata;

    create_impl create{};
    create_at_impl create_at{};
    create_error_impl create_error{};
    argument_list arguments{};

    template <constructor_policy_family Policy, class_kind Class, typename... Args>
    [[nodiscard]] static constructor_state_ptr make(metadata_map metadata);
    explicit constructor_state(constructor_index index, metadata_map metadata);
};

struct destructor_state final : intrusive_ref_counter<destructor_state> {
    using destroy_impl = fixed_function<void(const uarg&)>;
    using destroy_at_impl = fixed_function<void(void*)>;
    using destroy_error_impl = fixed_function<uerror(const uarg_base&)>;

    destructor_index index;
    metadata_map metadata;

    destroy_impl destroy{};
    destroy_at_impl destroy_at{};
    destroy_error_impl destroy_error{};

    template <class_kind Class>
    [[nodiscard]] static destructor_state_ptr make(metadata_map metadata);
    explicit destructor_state(destructor_index index, metadata_map metadata);
};

struct evalue_state final : intrusive_ref_counter<evalue_state> {
    evalue_index index;
    metadata_map metadata;

    uvalue enum_value{};
    uvalue underlying_value{};

    template <enum_kind Enum>
    [[nodiscard]] static evalue_state_ptr make(std::string name, Enum evalue, metadata_map metadata);
    explicit evalue_state(evalue_index index, metadata_map metadata);
};

struct function_state final : intrusive_ref_counter<function_state> {
    using invoke_impl = fixed_function<uvalue(std::span<const uarg>)>;
    using invoke_error_impl = fixed_function<uerror(std::span<const uarg_base>)>;

    function_index index;
    metadata_map metadata;

    invoke_impl invoke{};
    invoke_error_impl invoke_error{};
    argument_list arguments{};

    template <function_policy_family Policy, function_pointer_kind Function>
    [[nodiscard]] static function_state_ptr make(std::string name, Function function_ptr, metadata_map metadata);
    explicit function_state(function_index index, metadata_map metadata);
};

struct member_state final : intrusive_ref_counter<member_state> {
    using getter_impl = fixed_function<uvalue(const uinst&)>;
    using setter_impl = fixed_function<void(const uinst&, const uarg&)>;

    using getter_error_impl = fixed_function<uerror(const uinst_base&)>;
    using setter_error_impl = fixed_function<uerror(const uinst_base&, const uarg_base&)>;

    member_index index;
    metadata_map metadata;

    getter_impl getter{};
    setter_impl setter{};
    getter_error_impl getter_error{};
    setter_error_impl setter_error{};

    template <member_policy_family Policy, member_pointer_kind Member>
    [[nodiscard]] static member_state_ptr make(std::string name, Member member_ptr, metadata_map metadata);
    explicit member_state(member_index index, metadata_map metadata);
};

struct method_state final : intrusive_ref_counter<method_state> {
    using invoke_impl = fixed_function<uvalue(const uinst&, std::span<const uarg>)>;
    using invoke_error_impl = fixed_function<uerror(const uinst_base&, std::span<const uarg_base>)>;

    method_index index;
    metadata_map metadata;

    invoke_impl invoke{};
    invoke_error_impl invoke_error{};
    argument_list arguments{};

    template <method_policy_family Policy, method_pointer_kind Method>
    [[nodiscard]] static method_state_ptr make(std::string name, Method method_ptr, metadata_map metadata);
    explicit method_state(method_index index, metadata_map metadata);
};

struct scope_state final : intrusive_ref_counter<scope_state> {
    scope_index index;
    metadata_map metadata;

    function_list functions{};
    typedef_map typedefs{};
    variable_list variables{};

    [[nodiscard]] static scope_state_ptr make(std::string name, metadata_map metadata);
    explicit scope_state(scope_index index, metadata_map metadata);
};

struct variable_state final : intrusive_ref_counter<variable_state> {
    using getter_impl = fixed_function<uvalue()>;
    using setter_impl = fixed_function<void(const uarg&)>;
    using setter_error_impl = fixed_function<uerror(const uarg_base&)>;

    variable_index index;
    metadata_map metadata;

    getter_impl getter{};
    setter_impl setter{};
    setter_error_impl setter_error{};

    template <variable_policy_family Policy, pointer_kind Pointer>
    [[nodiscard]] static variable_state_ptr make(std::string name, Pointer variable_ptr, metadata_map metadata);
    explicit variable_state(variable_index index, metadata_map metadata);
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
class type_registry final {
public:
    class locker final {
    public:
        explicit locker() : lock_{instance().mutex_} {}

        ~locker() = default;

        locker(locker&&) = default;
        locker& operator=(locker&&) = default;

        locker(const locker&) = delete;
        locker& operator=(const locker&) = delete;

    private:
        std::unique_lock<std::recursive_mutex> lock_;
    };

    [[nodiscard]] static type_registry& instance() {
        static type_registry instance;
        return instance;
    }

public:
    template <typename F>
    void for_each_type(F&& f) const {
        const locker lock;

        for (const any_type& type : types_) {
            std::invoke(f, type);
        }
    }

public:
    template <typename T>
    [[nodiscard]] auto resolve_type() {
        // clang-format off
            if constexpr ( array_kind<T> ) { return resolve_array_type<T>(); }
            if constexpr ( class_kind<T> ) { return resolve_class_type<T>(); }
            if constexpr ( enum_kind<T> ) { return resolve_enum_type<T>(); }
            if constexpr ( function_kind<T> ) { return resolve_function_type<T>(); }
            if constexpr ( member_pointer_kind<T> ) { return resolve_member_type<T>(); }
            if constexpr ( method_pointer_kind<T> ) { return resolve_method_type<T>(); }
            if constexpr ( nullptr_kind<T> ) { return resolve_nullptr_type<T>(); }
            if constexpr ( number_kind<T> ) { return resolve_number_type<T>(); }
            if constexpr ( pointer_kind<T> ) { return resolve_pointer_type<T>(); }
            if constexpr ( reference_kind<T> ) { return resolve_reference_type<T>(); }
            if constexpr ( void_kind<T> ) { return resolve_void_type<T>(); }
        // clang-format on
    }

public:
    template <array_kind Array>
    [[nodiscard]] array_type resolve_array_type() {
        using array_t = std::remove_cv_t<Array>;
        static array_type type{ensure_type<array_type_data>(type_list<array_t>{})};
        return type;
    }

    template <class_kind Class>
    [[nodiscard]] class_type resolve_class_type() {
        using class_t = std::remove_cv_t<Class>;
        static class_type type{ensure_type<class_type_data>(type_list<class_t>{})};
        return type;
    }

    template <class_kind Class, typename... Args>
    [[nodiscard]] constructor_type resolve_constructor_type() {
        using class_t = std::remove_cv_t<Class>;
        static constructor_type type{ensure_type<constructor_type_data>(type_list<class_t>{}, type_list<Args...>{})};
        return type;
    }

    template <class_kind Class>
    [[nodiscard]] destructor_type resolve_destructor_type() {
        using class_t = std::remove_cv_t<Class>;
        static destructor_type type{ensure_type<destructor_type_data>(type_list<class_t>{})};
        return type;
    }

    template <enum_kind Enum>
    [[nodiscard]] enum_type resolve_enum_type() {
        using enum_t = std::remove_cv_t<Enum>;
        static enum_type type{ensure_type<enum_type_data>(type_list<enum_t>{})};
        return type;
    }

    template <function_kind Function>
    [[nodiscard]] function_type resolve_function_type() {
        using function_t = std::remove_cv_t<Function>;
        static function_type type{ensure_type<function_type_data>(type_list<function_t>{})};
        return type;
    }

    template <member_pointer_kind Member>
    [[nodiscard]] member_type resolve_member_type() {
        using member_t = std::remove_cv_t<Member>;
        static member_type type{ensure_type<member_type_data>(type_list<member_t>{})};
        return type;
    }

    template <method_pointer_kind Method>
    [[nodiscard]] method_type resolve_method_type() {
        using method_t = std::remove_cv_t<Method>;
        static method_type type{ensure_type<method_type_data>(type_list<method_t>{})};
        return type;
    }

    template <nullptr_kind Nullptr>
    [[nodiscard]] nullptr_type resolve_nullptr_type() {
        using nullptr_t = std::remove_cv_t<Nullptr>;
        static nullptr_type type{ensure_type<nullptr_type_data>(type_list<nullptr_t>{})};
        return type;
    }

    template <number_kind Number>
    [[nodiscard]] number_type resolve_number_type() {
        using number_t = std::remove_cv_t<Number>;
        static number_type type{ensure_type<number_type_data>(type_list<number_t>{})};
        return type;
    }

    template <pointer_kind Pointer>
    [[nodiscard]] pointer_type resolve_pointer_type() {
        using pointer_t = std::remove_cv_t<Pointer>;
        static pointer_type type{ensure_type<pointer_type_data>(type_list<pointer_t>{})};
        return type;
    }

    template <reference_kind Reference>
    [[nodiscard]] reference_type resolve_reference_type() {
        using reference_t = std::remove_cv_t<Reference>;
        static reference_type type{ensure_type<reference_type_data>(type_list<reference_t>{})};
        return type;
    }

    template <void_kind Void>
    [[nodiscard]] void_type resolve_void_type() {
        using void_t = std::remove_cv_t<Void>;
        static void_type type{ensure_type<void_type_data>(type_list<void_t>{})};
        return type;
    }

private:
    type_registry() = default;

    template <typename TypeData, typename... Args>
    TypeData* ensure_type(Args&&... args) {
        static auto data{std::make_unique<TypeData>(std::forward<Args>(args)...)};

        const locker lock;
        types_.emplace(any_type{data.get()});

        return data.get();
    }

private:
    std::recursive_mutex mutex_;
    std::set<any_type, std::less<>> types_;
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
struct poly_info final {
    const void* ptr{};
    class_type type{};
};

template <typename T>
concept check_poly_info_enabled  //
        = requires(type_registry& r, const T& v) {
              { v.get_most_derived_poly_info(r) } -> std::convertible_to<poly_info>;
          };
}  // namespace ME::meta::r::detail

#define META_HPP_ENABLE_POLY_INFO()                                                                                             \
public:                                                                                                                         \
    META_HPP_IGNORE_OVERRIDE_WARNINGS_PUSH()                                                                                    \
    virtual ::ME::meta::r::detail::poly_info get_most_derived_poly_info(::ME::meta::r::detail::type_registry& registry) const { \
        using self_type = std::remove_cvref_t<decltype(*this)>;                                                                 \
        return ::ME::meta::r::detail::poly_info{.ptr = this, .type = registry.resolve_class_type<self_type>()};                 \
    }                                                                                                                           \
    META_HPP_IGNORE_OVERRIDE_WARNINGS_POP()                                                                                     \
private:

namespace ME::meta::r::detail {
class state_registry final {
public:
    class locker final {
    public:
        explicit locker() : lock_{instance().mutex_} {}

        ~locker() = default;

        locker(locker&&) = default;
        locker& operator=(locker&&) = default;

        locker(const locker&) = delete;
        locker& operator=(const locker&) = delete;

    private:
        std::unique_lock<std::recursive_mutex> lock_;
    };

    [[nodiscard]] static state_registry& instance() {
        static state_registry instance;
        return instance;
    }

public:
    template <typename F>
    void for_each_scope(F&& f) const {
        const locker lock;

        for (auto&& [name, scope] : scopes_) {
            std::invoke(f, scope);
        }
    }

    [[nodiscard]] scope get_scope_by_name(std::string_view name) const noexcept {
        const locker lock;

        if (auto iter{scopes_.find(name)}; iter != scopes_.end()) {
            return iter->second;
        }

        return scope{};
    }

    [[nodiscard]] scope resolve_scope(std::string_view name) {
        const locker lock;

        if (auto iter{scopes_.find(name)}; iter != scopes_.end()) {
            return iter->second;
        }

        auto&& [iter, _] = scopes_.emplace(  //
                std::string{name}, scope_state::make(std::string{name}, metadata_map{}));

        return iter->second;
    }

private:
    state_registry() = default;

private:
    std::recursive_mutex mutex_;
    std::map<std::string, scope, std::less<>> scopes_;
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
template <detail::type_family Type = any_type, typename F>
void for_each_type(F&& f) {
    using namespace detail;
    type_registry& registry = type_registry::instance();
    registry.for_each_type([&f](const any_type& type) {
        if (type.is<Type>()) {
            std::invoke(f, type.as<Type>());
        }
    });
}

template <typename T>
[[nodiscard]] auto resolve_type() {
    using namespace detail;
    type_registry& registry = type_registry::instance();
    return registry.resolve_type<std::remove_cv_t<T>>();
}

template <typename T>
[[nodiscard]] auto resolve_type(T&&) {
    using namespace detail;
    type_registry& registry = type_registry::instance();
    return registry.resolve_type<std::remove_cvref_t<T>>();
}

template <typename... Ts>
[[nodiscard]] any_type_list resolve_types() {
    using namespace detail;
    type_registry& registry = type_registry::instance();
    return {registry.resolve_type<std::remove_cv_t<Ts>>()...};
}

template <typename... Ts>
[[nodiscard]] any_type_list resolve_types(type_list<Ts...>) {
    using namespace detail;
    type_registry& registry = type_registry::instance();
    return {registry.resolve_type<std::remove_cv_t<Ts>>()...};
}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <typename T>
[[nodiscard]] auto resolve_poly_type(T&& from) {
    using namespace detail;

    using raw_type = std::remove_cvref_t<T>;
    type_registry& registry = type_registry::instance();

    if constexpr (std::is_class_v<raw_type>) {
        static_assert(detail::check_poly_info_enabled<raw_type>, "The class doesn't support polymorphic type resolving. Use the META_HPP_ENABLE_POLY_INFO macro to fix it.");
        return from.get_most_derived_poly_info(registry).type;
    } else {
        (void)from;
        return registry.resolve_type<raw_type>();
    }
}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::class_kind Class, typename... Args>
[[nodiscard]] constructor_type resolve_constructor_type() {
    using namespace detail;
    type_registry& registry = type_registry::instance();
    return registry.resolve_constructor_type<Class, Args...>();
}

template <detail::class_kind Class>
[[nodiscard]] destructor_type resolve_destructor_type() {
    using namespace detail;
    type_registry& registry = type_registry::instance();
    return registry.resolve_destructor_type<Class>();
}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <typename F>
void for_each_scope(F&& f) {
    using namespace detail;
    state_registry& registry = state_registry::instance();
    registry.for_each_scope(std::forward<F>(f));
}

[[nodiscard]] inline scope resolve_scope(std::string_view name) {
    using namespace detail;
    state_registry& registry = state_registry::instance();
    return registry.resolve_scope(name);
}
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <typename Class, typename... Args>
concept class_bind_constructor_kind                    //
        = class_kind<Class>                            //
          && std::is_constructible_v<Class, Args...>;  //

template <typename Class>
concept class_bind_destructor_kind                             //
        = class_kind<Class> && std::is_destructible_v<Class>;  //

template <typename Class, typename Base>
concept class_bind_base_kind                     //
        = class_kind<Class> && class_kind<Base>  //
          && std::derived_from<Class, Base>;     //

template <typename Class, typename Member>
concept class_bind_member_kind                                                   //
        = class_kind<Class> && member_pointer_kind<Member>                       //
          && std::is_same_v<Class, typename member_traits<Member>::class_type>;  //

template <typename Class, typename Method>
concept class_bind_method_kind                                                   //
        = class_kind<Class> && method_pointer_kind<Method>                       //
          && std::is_same_v<Class, typename method_traits<Method>::class_type>;  //
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
struct argument_opts final {
    std::string name{};
    metadata_map metadata{};
};

using argument_opts_list = std::vector<argument_opts>;

struct constructor_opts final {
    argument_opts_list arguments{};
    metadata_map metadata{};
};

struct destructor_opts final {
    metadata_map metadata{};
};

struct evalue_opts final {
    metadata_map metadata{};
};

struct function_opts final {
    argument_opts_list arguments{};
    metadata_map metadata{};
};

struct member_opts final {
    metadata_map metadata{};
};

struct method_opts final {
    argument_opts_list arguments;
    metadata_map metadata{};
};

struct variable_opts final {
    metadata_map metadata{};
};
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::type_family Type>
class type_bind_base {
public:
    explicit type_bind_base(const Type& type, metadata_map metadata) : data_{detail::type_access(type)} { detail::insert_or_assign(data_->metadata, std::move(metadata)); }

    operator Type() const noexcept { return Type{data_}; }

protected:
    using data_ptr = typename detail::type_traits<Type>::data_ptr;
    using data_ref = decltype(*std::declval<data_ptr>());

    [[nodiscard]] data_ref get_data() noexcept { return *data_; }

private:
    data_ptr data_;
    detail::type_registry::locker locker_;
};

template <detail::state_family State>
class state_bind_base {
public:
    explicit state_bind_base(const State& state, metadata_map metadata) : state_{detail::state_access(state)} { detail::insert_or_assign(state_->metadata, std::move(metadata)); }

    operator State() const noexcept { return State{state_}; }

protected:
    using state_ptr = typename detail::state_traits<State>::state_ptr;
    using state_ref = decltype(*std::declval<state_ptr>());

    [[nodiscard]] state_ref get_state() noexcept { return *state_; }

private:
    state_ptr state_;
    detail::state_registry::locker locker_;
};
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::array_kind Array>
class array_bind final : public type_bind_base<array_type> {
public:
    explicit array_bind(metadata_map metadata);
};
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::class_kind Class>
class class_bind final : public type_bind_base<class_type> {
public:
    explicit class_bind(metadata_map metadata);

    // base_

    template <detail::class_kind... Bases>
        requires(... && detail::class_bind_base_kind<Class, Bases>)
    class_bind& base_();

    // constructor_

    template <typename... Args, constructor_policy_family Policy = constructor_policy::as_object_t>
    class_bind& constructor_(Policy = {})
        requires detail::class_bind_constructor_kind<Class, Args...>;

    template <typename... Args, constructor_policy_family Policy = constructor_policy::as_object_t>
    class_bind& constructor_(constructor_opts opts, Policy = {})
        requires detail::class_bind_constructor_kind<Class, Args...>;

    // destructor_

    class_bind& destructor_()
        requires detail::class_bind_destructor_kind<Class>;

    class_bind& destructor_(destructor_opts opts)
        requires detail::class_bind_destructor_kind<Class>;

    // function_

    template <detail::function_pointer_kind Function, function_policy_family Policy = function_policy::as_copy_t>
    class_bind& function_(std::string name, Function function_ptr, Policy = {});

    template <detail::function_pointer_kind Function, function_policy_family Policy = function_policy::as_copy_t>
    class_bind& function_(std::string name, Function function_ptr, function_opts opts, Policy = {});

    template <detail::function_pointer_kind Function, function_policy_family Policy = function_policy::as_copy_t>
    class_bind& function_(std::string name, Function function_ptr, string_ilist arguments, Policy = {});

    // member_

    template <detail::member_pointer_kind Member, member_policy_family Policy = member_policy::as_copy_t>
        requires detail::class_bind_member_kind<Class, Member>
    class_bind& member_(std::string name, Member member_ptr, Policy = {});

    template <detail::member_pointer_kind Member, member_policy_family Policy = member_policy::as_copy_t>
        requires detail::class_bind_member_kind<Class, Member>
    class_bind& member_(std::string name, Member member_ptr, member_opts opts, Policy = {});

    // method_

    template <detail::method_pointer_kind Method, method_policy_family Policy = method_policy::as_copy_t>
        requires detail::class_bind_method_kind<Class, Method>
    class_bind& method_(std::string name, Method method_ptr, Policy = {});

    template <detail::method_pointer_kind Method, method_policy_family Policy = method_policy::as_copy_t>
        requires detail::class_bind_method_kind<Class, Method>
    class_bind& method_(std::string name, Method method_ptr, method_opts opts, Policy = {});

    template <detail::method_pointer_kind Method, method_policy_family Policy = method_policy::as_copy_t>
        requires detail::class_bind_method_kind<Class, Method>
    class_bind& method_(std::string name, Method method_ptr, string_ilist arguments, Policy = {});

    // typdef_

    template <typename Type>
    class_bind& typedef_(std::string name);

    // variable_

    template <detail::pointer_kind Pointer, variable_policy_family Policy = variable_policy::as_copy_t>
    class_bind& variable_(std::string name, Pointer variable_ptr, Policy = {});

    template <detail::pointer_kind Pointer, variable_policy_family Policy = variable_policy::as_copy_t>
    class_bind& variable_(std::string name, Pointer variable_ptr, variable_opts opts, Policy = {});
};
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::enum_kind Enum>
class enum_bind final : public type_bind_base<enum_type> {
public:
    explicit enum_bind(metadata_map metadata);

    enum_bind& evalue_(std::string name, Enum value);
    enum_bind& evalue_(std::string name, Enum value, evalue_opts opts);
};
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::function_kind Function>
class function_bind final : public type_bind_base<function_type> {
public:
    explicit function_bind(metadata_map metadata);
};
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::member_pointer_kind Member>
class member_bind final : public type_bind_base<member_type> {
public:
    explicit member_bind(metadata_map metadata);
};
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::method_pointer_kind Method>
class method_bind final : public type_bind_base<method_type> {
public:
    explicit method_bind(metadata_map metadata);
};
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::nullptr_kind Nullptr>
class nullptr_bind final : public type_bind_base<nullptr_type> {
public:
    explicit nullptr_bind(metadata_map metadata);
};
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::number_kind Number>
class number_bind final : public type_bind_base<number_type> {
public:
    explicit number_bind(metadata_map metadata);
};
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::pointer_kind Pointer>
class pointer_bind final : public type_bind_base<pointer_type> {
public:
    explicit pointer_bind(metadata_map metadata);
};
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::reference_kind Reference>
class reference_bind final : public type_bind_base<reference_type> {
public:
    explicit reference_bind(metadata_map metadata);
};
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::void_kind Void>
class void_bind final : public type_bind_base<void_type> {
public:
    explicit void_bind(metadata_map metadata);
};
}  // namespace ME::meta::r

namespace ME::meta::r {
class scope_bind final : public state_bind_base<scope> {
public:
    explicit scope_bind(const scope& scope, metadata_map metadata);

    // function_

    template <detail::function_pointer_kind Function, function_policy_family Policy = function_policy::as_copy_t>
    scope_bind& function_(std::string name, Function function_ptr, Policy = {});

    template <detail::function_pointer_kind Function, function_policy_family Policy = function_policy::as_copy_t>
    scope_bind& function_(std::string name, Function function_ptr, function_opts opts, Policy = {});

    template <detail::function_pointer_kind Function, function_policy_family Policy = function_policy::as_copy_t>
    scope_bind& function_(std::string name, Function function_ptr, string_ilist arguments, Policy = {});

    // typedef_

    template <typename Type>
    scope_bind& typedef_(std::string name);

    // variable_

    template <detail::pointer_kind Pointer, variable_policy_family Policy = variable_policy::as_copy_t>
    scope_bind& variable_(std::string name, Pointer variable_ptr, Policy = {});

    template <detail::pointer_kind Pointer, variable_policy_family Policy = variable_policy::as_copy_t>
    scope_bind& variable_(std::string name, Pointer variable_ptr, variable_opts opts, Policy = {});
};
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::array_kind Array>
array_bind<Array> array_(metadata_map metadata = {}) {
    return array_bind<Array>{std::move(metadata)};
}

template <detail::class_kind Class>
class_bind<Class> class_(metadata_map metadata = {}) {
    return class_bind<Class>{std::move(metadata)};
}

template <detail::enum_kind Enum>
enum_bind<Enum> enum_(metadata_map metadata = {}) {
    return enum_bind<Enum>{std::move(metadata)};
}

template <detail::function_kind Function>
function_bind<Function> function_(metadata_map metadata = {}) {
    return function_bind<Function>{std::move(metadata)};
}

template <detail::member_pointer_kind Member>
member_bind<Member> member_(metadata_map metadata = {}) {
    return member_bind<Member>{std::move(metadata)};
}

template <detail::method_pointer_kind Method>
method_bind<Method> method_(metadata_map metadata = {}) {
    return method_bind<Method>{std::move(metadata)};
}

template <detail::nullptr_kind Nullptr>
nullptr_bind<Nullptr> nullptr_(metadata_map metadata = {}) {
    return nullptr_bind<Nullptr>{std::move(metadata)};
}

template <detail::number_kind Number>
number_bind<Number> number_(metadata_map metadata = {}) {
    return number_bind<Number>{std::move(metadata)};
}

template <detail::pointer_kind Pointer>
pointer_bind<Pointer> pointer_(metadata_map metadata = {}) {
    return pointer_bind<Pointer>{std::move(metadata)};
}

template <detail::reference_kind Reference>
reference_bind<Reference> reference_(metadata_map metadata = {}) {
    return reference_bind<Reference>{std::move(metadata)};
}

template <detail::void_kind Void>
void_bind<Void> void_(metadata_map metadata = {}) {
    return void_bind<Void>{std::move(metadata)};
}
}  // namespace ME::meta::r

namespace ME::meta::r {
inline scope_bind local_scope_(std::string name, metadata_map metadata = {}) {
    scope local_scope{detail::scope_state::make(std::move(name), std::move(metadata))};
    return scope_bind{local_scope, {}};
}

inline scope_bind static_scope_(std::string_view name, metadata_map metadata = {}) {
    scope static_scope{resolve_scope(name)};
    return scope_bind{static_scope, std::move(metadata)};
}

inline scope_bind extend_scope_(const scope& scope, metadata_map metadata = {}) { return scope_bind{scope, std::move(metadata)}; }
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::array_kind Array>
array_bind<Array>::array_bind(metadata_map metadata) : type_bind_base{resolve_type<Array>(), std::move(metadata)} {}
}  // namespace ME::meta::r

namespace ME::meta::r::detail::class_bind_impl {
using base_upcasts_t = class_type_data::base_upcasts_t;
using deep_upcasts_t = class_type_data::deep_upcasts_t;

using upcast_func_t = class_type_data::upcast_func_t;
using upcast_func_list_t = class_type_data::upcast_func_list_t;

using new_bases_db_t = std::vector<std::pair<class_type, upcast_func_t>>;
using deep_upcasts_db_t = std::map<class_type, deep_upcasts_t, std::less<>>;
using derived_classes_db_t = std::map<class_type, class_list, std::less<>>;

template <class_kind Class, class_kind Base>
    requires std::is_base_of_v<Base, Class>
void update_new_bases_db(  //
        new_bases_db_t& new_bases_db) {
    const class_type& new_base = resolve_type<Base>();

    for (auto&& [db_base, _] : new_bases_db) {
        if (db_base == new_base) {
            return;
        }
    }

    new_bases_db.emplace_back(  //
            new_base, upcast_func_t{std::in_place_type<Class>, std::in_place_type<Base>});
}

inline void update_deep_upcasts_db(  //
        const class_type& derived_class, const class_type& new_base_class, upcast_func_list_t&& derived_to_new_base, deep_upcasts_db_t& deep_upcasts_db) {
    const class_type_data& derived_class_data = *type_access(derived_class);
    const class_type_data& new_base_class_data = *type_access(new_base_class);

    const auto [deep_upcasts_db_iter, _] = deep_upcasts_db.try_emplace(derived_class, derived_class_data.deep_upcasts);
    deep_upcasts_t& derived_deep_upcasts = deep_upcasts_db_iter->second;

    const auto add_derived_deep_upcast = [&derived_deep_upcasts](const class_type& deep_class, upcast_func_list_t&& upcasts) {
        auto&& [position, emplaced] = derived_deep_upcasts.try_emplace(deep_class, std::move(upcasts));
        if (!emplaced) {
            position->second.is_ambiguous = is_disjoint(position->second.vbases, upcasts.vbases);
        }
    };

    for (auto&& [new_deep_class, new_base_to_deep] : new_base_class_data.deep_upcasts) {
        upcast_func_list_t derived_to_new_deep = derived_to_new_base + new_base_to_deep;
        add_derived_deep_upcast(new_deep_class, std::move(derived_to_new_deep));
    }

    for (const class_type& subderived_class : derived_class_data.derived_classes) {
        const class_type_data& subderived_data = *type_access(subderived_class);
        upcast_func_t subderived_to_derived = subderived_data.base_upcasts.at(derived_class);
        upcast_func_list_t subderived_to_new_base = subderived_to_derived + derived_to_new_base;
        update_deep_upcasts_db(subderived_class, new_base_class, std::move(subderived_to_new_base), deep_upcasts_db);
    }

    add_derived_deep_upcast(new_base_class, std::move(derived_to_new_base));
}

inline void updata_derived_classes_db(  //
        const class_type& self_class, const class_type& new_base_class, derived_classes_db_t& derived_classes_db) {
    const class_type_data& base_class_data = *type_access(new_base_class);
    class_list new_derived_classes{base_class_data.derived_classes};
    insert_or_assign(new_derived_classes, self_class);
    derived_classes_db.emplace(new_base_class, std::move(new_derived_classes));
}
}  // namespace ME::meta::r::detail::class_bind_impl

namespace ME::meta::r {
template <detail::class_kind Class>
class_bind<Class>::class_bind(metadata_map metadata) : type_bind_base{resolve_type<Class>(), std::move(metadata)} {
    if constexpr (std::is_destructible_v<Class>) {
        destructor_();
    }
}

//
// base_
//

template <detail::class_kind Class>
template <detail::class_kind... Bases>
    requires(... && detail::class_bind_base_kind<Class, Bases>)
class_bind<Class>& class_bind<Class>::base_() {
    using namespace detail;
    using namespace detail::class_bind_impl;

    new_bases_db_t new_bases_db;
    (update_new_bases_db<Class, Bases>(new_bases_db), ...);

    deep_upcasts_db_t deep_upcasts_db;
    derived_classes_db_t derived_classes_db;

    class_list new_base_classes{get_data().base_classes};
    base_upcasts_t new_base_upcasts{get_data().base_upcasts};

    for (auto&& [new_base_class, self_to_new_base] : new_bases_db) {
        if (std::find(new_base_classes.begin(), new_base_classes.end(), new_base_class) != new_base_classes.end()) {
            continue;
        }

        update_deep_upcasts_db(*this, new_base_class, self_to_new_base, deep_upcasts_db);
        updata_derived_classes_db(*this, new_base_class, derived_classes_db);

        new_base_classes.emplace_back(new_base_class);
        new_base_upcasts.emplace(new_base_class, self_to_new_base);
    }

    get_data().base_classes.swap(new_base_classes);
    get_data().base_upcasts.swap(new_base_upcasts);

    for (auto&& [derived_class, new_deep_upcasts] : deep_upcasts_db) {
        class_type_data& derived_data = *type_access(derived_class);
        derived_data.deep_upcasts.swap(new_deep_upcasts);
    }

    for (auto&& [base_class, new_derived_classes] : derived_classes_db) {
        class_type_data& base_data = *type_access(base_class);
        base_data.derived_classes.swap(new_derived_classes);
    }

    return *this;
}

//
// constructor_
//

template <detail::class_kind Class>
template <typename... Args, constructor_policy_family Policy>
class_bind<Class>& class_bind<Class>::constructor_(Policy policy)
    requires detail::class_bind_constructor_kind<Class, Args...>
{
    return constructor_<Args...>({}, policy);
}

template <detail::class_kind Class>
template <typename... Args, constructor_policy_family Policy>
class_bind<Class>& class_bind<Class>::constructor_(constructor_opts opts, Policy)
    requires detail::class_bind_constructor_kind<Class, Args...>
{
    auto state = detail::constructor_state::make<Policy, Class, Args...>(std::move(opts.metadata));

    META_HPP_ASSERT(                                          //
            opts.arguments.size() <= state->arguments.size()  //
            && "provided argument names don't match constructor argument count");

    for (std::size_t i{}, e{std::min(opts.arguments.size(), state->arguments.size())}; i < e; ++i) {
        argument& arg = state->arguments[i];
        detail::state_access(arg)->name = std::move(opts.arguments[i].name);
        detail::state_access(arg)->metadata = std::move(opts.arguments[i].metadata);
    }

    detail::insert_or_assign(get_data().constructors, constructor{std::move(state)});
    return *this;
}

//
// destructor_
//

template <detail::class_kind Class>
class_bind<Class>& class_bind<Class>::destructor_()
    requires detail::class_bind_destructor_kind<Class>
{
    return destructor_({});
}

template <detail::class_kind Class>
class_bind<Class>& class_bind<Class>::destructor_(destructor_opts opts)
    requires detail::class_bind_destructor_kind<Class>
{
    auto state = detail::destructor_state::make<Class>(std::move(opts.metadata));
    detail::insert_or_assign(get_data().destructors, destructor{std::move(state)});
    return *this;
}

//
// function_
//

template <detail::class_kind Class>
template <detail::function_pointer_kind Function, function_policy_family Policy>
class_bind<Class>& class_bind<Class>::function_(std::string name, Function function_ptr, Policy policy) {
    return function_(std::move(name), function_ptr, {}, policy);
}

template <detail::class_kind Class>
template <detail::function_pointer_kind Function, function_policy_family Policy>
class_bind<Class>& class_bind<Class>::function_(std::string name, Function function_ptr, function_opts opts, Policy) {
    auto state = detail::function_state::make<Policy>(std::move(name), function_ptr, std::move(opts.metadata));

    META_HPP_ASSERT(                                          //
            opts.arguments.size() <= state->arguments.size()  //
            && "provided arguments don't match function argument count");

    for (std::size_t i{}, e{std::min(opts.arguments.size(), state->arguments.size())}; i < e; ++i) {
        argument& arg = state->arguments[i];
        detail::state_access(arg)->name = std::move(opts.arguments[i].name);
        detail::state_access(arg)->metadata = std::move(opts.arguments[i].metadata);
    }

    detail::insert_or_assign(get_data().functions, function{std::move(state)});
    return *this;
}

template <detail::class_kind Class>
template <detail::function_pointer_kind Function, function_policy_family Policy>
class_bind<Class>& class_bind<Class>::function_(std::string name, Function function_ptr, string_ilist arguments, Policy) {
    auto state = detail::function_state::make<Policy>(std::move(name), function_ptr, {});

    META_HPP_ASSERT(arguments.size() <= state->arguments.size()  //
                    && "provided argument names don't match function argument count");

    for (std::size_t i{}, e{std::min(arguments.size(), state->arguments.size())}; i < e; ++i) {
        argument& arg = state->arguments[i];
        // NOLINTNEXTLINE(*-pointer-arithmetic)
        detail::state_access(arg)->name = std::data(arguments)[i];
    }

    detail::insert_or_assign(get_data().functions, function{std::move(state)});
    return *this;
}

//
// member_
//

template <detail::class_kind Class>
template <detail::member_pointer_kind Member, member_policy_family Policy>
    requires detail::class_bind_member_kind<Class, Member>
class_bind<Class>& class_bind<Class>::member_(std::string name, Member member_ptr, Policy policy) {
    return member_(std::move(name), member_ptr, {}, policy);
}

template <detail::class_kind Class>
template <detail::member_pointer_kind Member, member_policy_family Policy>
    requires detail::class_bind_member_kind<Class, Member>
class_bind<Class>& class_bind<Class>::member_(std::string name, Member member_ptr, member_opts opts, Policy) {
    auto state = detail::member_state::make<Policy>(std::move(name), member_ptr, std::move(opts.metadata));
    detail::insert_or_assign(get_data().members, member{std::move(state)});
    return *this;
}

//
// method_
//

template <detail::class_kind Class>
template <detail::method_pointer_kind Method, method_policy_family Policy>
    requires detail::class_bind_method_kind<Class, Method>
class_bind<Class>& class_bind<Class>::method_(std::string name, Method method_ptr, Policy policy) {
    return method_(std::move(name), method_ptr, {}, policy);
}

template <detail::class_kind Class>
template <detail::method_pointer_kind Method, method_policy_family Policy>
    requires detail::class_bind_method_kind<Class, Method>
class_bind<Class>& class_bind<Class>::method_(std::string name, Method method_ptr, method_opts opts, Policy) {
    auto state = detail::method_state::make<Policy>(std::move(name), method_ptr, std::move(opts.metadata));

    META_HPP_ASSERT(                                          //
            opts.arguments.size() <= state->arguments.size()  //
            && "provided arguments don't match method argument count");

    for (std::size_t i{}, e{std::min(opts.arguments.size(), state->arguments.size())}; i < e; ++i) {
        argument& arg = state->arguments[i];
        detail::state_access(arg)->name = std::move(opts.arguments[i].name);
        detail::state_access(arg)->metadata = std::move(opts.arguments[i].metadata);
    }

    detail::insert_or_assign(get_data().methods, method{std::move(state)});
    return *this;
}

template <detail::class_kind Class>
template <detail::method_pointer_kind Method, method_policy_family Policy>
    requires detail::class_bind_method_kind<Class, Method>
class_bind<Class>& class_bind<Class>::method_(std::string name, Method method_ptr, string_ilist arguments, Policy) {
    auto state = detail::method_state::make<Policy>(std::move(name), method_ptr, {});

    META_HPP_ASSERT(                                     //
            arguments.size() <= state->arguments.size()  //
            && "provided argument names don't match method argument count");

    for (std::size_t i{}, e{std::min(arguments.size(), state->arguments.size())}; i < e; ++i) {
        argument& arg = state->arguments[i];
        // NOLINTNEXTLINE(*-pointer-arithmetic)
        detail::state_access(arg)->name = std::data(arguments)[i];
    }

    detail::insert_or_assign(get_data().methods, method{std::move(state)});
    return *this;
}

//
// typedef_
//

template <detail::class_kind Class>
template <typename Type>
class_bind<Class>& class_bind<Class>::typedef_(std::string name) {
    get_data().typedefs.insert_or_assign(std::move(name), resolve_type<Type>());
    return *this;
}

//
// variable_
//

template <detail::class_kind Class>
template <detail::pointer_kind Pointer, variable_policy_family Policy>
class_bind<Class>& class_bind<Class>::variable_(std::string name, Pointer variable_ptr, Policy policy) {
    return variable_(std::move(name), variable_ptr, {}, policy);
}

template <detail::class_kind Class>
template <detail::pointer_kind Pointer, variable_policy_family Policy>
class_bind<Class>& class_bind<Class>::variable_(std::string name, Pointer variable_ptr, variable_opts opts, Policy) {
    auto state = detail::variable_state::make<Policy>(std::move(name), variable_ptr, std::move(opts.metadata));
    detail::insert_or_assign(get_data().variables, variable{std::move(state)});
    return *this;
}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::enum_kind Enum>
enum_bind<Enum>::enum_bind(metadata_map metadata) : type_bind_base{resolve_type<Enum>(), std::move(metadata)} {}

template <detail::enum_kind Enum>
enum_bind<Enum>& enum_bind<Enum>::evalue_(std::string name, Enum value) {
    return evalue_(std::move(name), std::move(value), {});
}

template <detail::enum_kind Enum>
enum_bind<Enum>& enum_bind<Enum>::evalue_(std::string name, Enum value, evalue_opts opts) {
    auto state = detail::evalue_state::make(std::move(name), std::move(value), std::move(opts.metadata));
    detail::insert_or_assign(get_data().evalues, evalue{std::move(state)});
    return *this;
}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::function_kind Function>
function_bind<Function>::function_bind(metadata_map metadata) : type_bind_base{resolve_type<Function>(), std::move(metadata)} {}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::member_pointer_kind Member>
member_bind<Member>::member_bind(metadata_map metadata) : type_bind_base{resolve_type<Member>(), std::move(metadata)} {}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::method_pointer_kind Method>
method_bind<Method>::method_bind(metadata_map metadata) : type_bind_base{resolve_type<Method>(), std::move(metadata)} {}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::nullptr_kind Nullptr>
nullptr_bind<Nullptr>::nullptr_bind(metadata_map metadata) : type_bind_base{resolve_type<Nullptr>(), std::move(metadata)} {}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::number_kind Number>
number_bind<Number>::number_bind(metadata_map metadata) : type_bind_base{resolve_type<Number>(), std::move(metadata)} {}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::pointer_kind Pointer>
pointer_bind<Pointer>::pointer_bind(metadata_map metadata) : type_bind_base{resolve_type<Pointer>(), std::move(metadata)} {}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::reference_kind Reference>
reference_bind<Reference>::reference_bind(metadata_map metadata) : type_bind_base{resolve_type<Reference>(), std::move(metadata)} {}
}  // namespace ME::meta::r

namespace ME::meta::r {
inline scope_bind::scope_bind(const scope& scope, metadata_map metadata) : state_bind_base{scope, std::move(metadata)} {}

//
// function_
//

template <detail::function_pointer_kind Function, function_policy_family Policy>
scope_bind& scope_bind::function_(std::string name, Function function_ptr, Policy policy) {
    return function_(std::move(name), function_ptr, {}, policy);
}

template <detail::function_pointer_kind Function, function_policy_family Policy>
scope_bind& scope_bind::function_(std::string name, Function function_ptr, function_opts opts, Policy) {
    auto state = detail::function_state::make<Policy>(std::move(name), function_ptr, std::move(opts.metadata));

    META_HPP_ASSERT(                                          //
            opts.arguments.size() <= state->arguments.size()  //
            && "provided arguments don't match function argument count");

    for (std::size_t i{}, e{std::min(opts.arguments.size(), state->arguments.size())}; i < e; ++i) {
        argument& arg = state->arguments[i];
        detail::state_access(arg)->name = std::move(opts.arguments[i].name);
        detail::state_access(arg)->metadata = std::move(opts.arguments[i].metadata);
    }

    detail::insert_or_assign(get_state().functions, function{std::move(state)});
    return *this;
}

template <detail::function_pointer_kind Function, function_policy_family Policy>
scope_bind& scope_bind::function_(std::string name, Function function_ptr, string_ilist arguments, Policy) {
    auto state = detail::function_state::make<Policy>(std::move(name), function_ptr, {});

    META_HPP_ASSERT(                                     //
            arguments.size() <= state->arguments.size()  //
            && "provided argument names don't match function argument count");

    for (std::size_t i{}, e{std::min(arguments.size(), state->arguments.size())}; i < e; ++i) {
        argument& arg = state->arguments[i];
        // NOLINTNEXTLINE(*-pointer-arithmetic)
        detail::state_access(arg)->name = std::data(arguments)[i];
    }

    detail::insert_or_assign(get_state().functions, function{std::move(state)});
    return *this;
}

//
// typedef_
//

template <typename Type>
scope_bind& scope_bind::typedef_(std::string name) {
    get_state().typedefs.insert_or_assign(std::move(name), resolve_type<Type>());
    return *this;
}

//
// variable_
//

template <detail::pointer_kind Pointer, variable_policy_family Policy>
scope_bind& scope_bind::variable_(std::string name, Pointer variable_ptr, Policy policy) {
    return variable_(std::move(name), variable_ptr, {}, policy);
}

template <detail::pointer_kind Pointer, variable_policy_family Policy>
scope_bind& scope_bind::variable_(std::string name, Pointer variable_ptr, variable_opts opts, Policy) {
    auto state = detail::variable_state::make<Policy>(std::move(name), variable_ptr, std::move(opts.metadata));
    detail::insert_or_assign(get_state().variables, variable{std::move(state)});
    return *this;
}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::void_kind Void>
void_bind<Void>::void_bind(metadata_map metadata) : type_bind_base{resolve_type<Void>(), std::move(metadata)} {}
}  // namespace ME::meta::r

namespace ME::meta::r {
inline argument_index::argument_index(any_type type, std::size_t position) : type_{type}, position_{position} {}

inline any_type argument_index::get_type() const noexcept { return type_; }

inline std::size_t argument_index::get_position() const noexcept { return position_; }

inline void argument_index::swap(argument_index& other) noexcept {
    std::swap(type_, other.type_);
    std::swap(position_, other.position_);
}

inline std::size_t argument_index::get_hash() const noexcept { return detail::hash_combiner{}(detail::hash_combiner{}(type_), position_); }
}  // namespace ME::meta::r

namespace ME::meta::r {
inline constructor_index::constructor_index(constructor_type type) : type_{type} {}

inline constructor_type constructor_index::get_type() const noexcept { return type_; }

inline void constructor_index::swap(constructor_index& other) noexcept { std::swap(type_, other.type_); }

inline std::size_t constructor_index::get_hash() const noexcept { return detail::hash_combiner{}(type_); }
}  // namespace ME::meta::r

namespace ME::meta::r {
inline destructor_index::destructor_index(destructor_type type) : type_{type} {}

inline destructor_type destructor_index::get_type() const noexcept { return type_; }

inline void destructor_index::swap(destructor_index& other) noexcept { std::swap(type_, other.type_); }

inline std::size_t destructor_index::get_hash() const noexcept { return detail::hash_combiner{}(type_); }
}  // namespace ME::meta::r

namespace ME::meta::r {
inline evalue_index::evalue_index(enum_type type, std::string name) : type_{type}, name_{std::move(name)} {}

inline enum_type evalue_index::get_type() const noexcept { return type_; }

inline std::string&& evalue_index::get_name() && noexcept { return std::move(name_); }

inline const std::string& evalue_index::get_name() const& noexcept { return name_; }

inline void evalue_index::swap(evalue_index& other) noexcept {
    std::swap(type_, other.type_);
    std::swap(name_, other.name_);
}

inline std::size_t evalue_index::get_hash() const noexcept { return detail::hash_combiner{}(detail::hash_combiner{}(type_), name_); }
}  // namespace ME::meta::r

namespace ME::meta::r {
inline function_index::function_index(function_type type, std::string name) : type_{type}, name_{std::move(name)} {}

inline function_type function_index::get_type() const noexcept { return type_; }

inline std::string&& function_index::get_name() && noexcept { return std::move(name_); }

inline const std::string& function_index::get_name() const& noexcept { return name_; }

inline void function_index::swap(function_index& other) noexcept {
    std::swap(type_, other.type_);
    std::swap(name_, other.name_);
}

inline std::size_t function_index::get_hash() const noexcept { return detail::hash_combiner{}(detail::hash_combiner{}(type_), name_); }
}  // namespace ME::meta::r

namespace ME::meta::r {
inline member_index::member_index(member_type type, std::string name) : type_{type}, name_{std::move(name)} {}

inline member_type member_index::get_type() const noexcept { return type_; }

inline std::string&& member_index::get_name() && noexcept { return std::move(name_); }

inline const std::string& member_index::get_name() const& noexcept { return name_; }

inline void member_index::swap(member_index& other) noexcept {
    std::swap(type_, other.type_);
    std::swap(name_, other.name_);
}

inline std::size_t member_index::get_hash() const noexcept { return detail::hash_combiner{}(detail::hash_combiner{}(type_), name_); }
}  // namespace ME::meta::r

namespace ME::meta::r {
inline method_index::method_index(method_type type, std::string name) : type_{type}, name_{std::move(name)} {}

inline method_type method_index::get_type() const noexcept { return type_; }

inline std::string&& method_index::get_name() && noexcept { return std::move(name_); }

inline const std::string& method_index::get_name() const& noexcept { return name_; }

inline void method_index::swap(method_index& other) noexcept {
    std::swap(type_, other.type_);
    std::swap(name_, other.name_);
}

inline std::size_t method_index::get_hash() const noexcept { return detail::hash_combiner{}(detail::hash_combiner{}(type_), name_); }
}  // namespace ME::meta::r

namespace ME::meta::r {
inline scope_index::scope_index(std::string name) : name_{std::move(name)} {}

inline std::string&& scope_index::get_name() && noexcept { return std::move(name_); }

inline const std::string& scope_index::get_name() const& noexcept { return name_; }

inline void scope_index::swap(scope_index& other) noexcept { std::swap(name_, other.name_); }

inline std::size_t scope_index::get_hash() const noexcept { return detail::hash_combiner{}(name_); }
}  // namespace ME::meta::r

namespace ME::meta::r {
inline variable_index::variable_index(pointer_type type, std::string name) : type_{type}, name_{std::move(name)} {}

inline pointer_type variable_index::get_type() const noexcept { return type_; }

inline std::string&& variable_index::get_name() && noexcept { return std::move(name_); }

inline const std::string& variable_index::get_name() const& noexcept { return name_; }

inline void variable_index::swap(variable_index& other) noexcept {
    std::swap(type_, other.type_);
    std::swap(name_, other.name_);
}

inline std::size_t variable_index::get_hash() const noexcept { return detail::hash_combiner{}(detail::hash_combiner{}(type_), name_); }
}  // namespace ME::meta::r

namespace ME::meta::r {
template <typename... Args>
uvalue invoke(const function& function, Args&&... args);

template <typename... Args>
uresult try_invoke(const function& function, Args&&... args);

template <detail::function_pointer_kind Function, typename... Args>
uvalue invoke(Function function_ptr, Args&&... args);

template <detail::function_pointer_kind Function, typename... Args>
uresult try_invoke(Function function_ptr, Args&&... args);
}  // namespace ME::meta::r

namespace ME::meta::r {
template <typename Instance>
uvalue invoke(const member& member, Instance&& instance);

template <typename Instance>
uresult try_invoke(const member& member, Instance&& instance);

template <detail::member_pointer_kind Member, typename Instance>
uvalue invoke(Member member_ptr, Instance&& instance);

template <detail::member_pointer_kind Member, typename Instance>
uresult try_invoke(Member member_ptr, Instance&& instance);
}  // namespace ME::meta::r

namespace ME::meta::r {
template <typename Instance, typename... Args>
uvalue invoke(const method& method, Instance&& instance, Args&&... args);

template <typename Instance, typename... Args>
uresult try_invoke(const method& method, Instance&& instance, Args&&... args);

template <detail::method_pointer_kind Method, typename Instance, typename... Args>
uvalue invoke(Method method_ptr, Instance&& instance, Args&&... args);

template <detail::method_pointer_kind Method, typename Instance, typename... Args>
uresult try_invoke(Method method_ptr, Instance&& instance, Args&&... args);
}  // namespace ME::meta::r

namespace ME::meta::r {
template <typename... Args>
bool is_invocable_with(const function& function) noexcept;

template <typename... Args>
bool is_invocable_with(const function& function, Args&&... args) noexcept;

template <typename... Args, detail::function_pointer_kind Function>
bool is_invocable_with(Function function_ptr) noexcept;

template <typename... Args, detail::function_pointer_kind Function>
bool is_invocable_with(Function function_ptr, Args&&... args) noexcept;

template <typename... Args>
uerror check_invocable_error(const function& function) noexcept;

template <typename... Args>
uerror check_invocable_error(const function& function, Args&&... args) noexcept;

template <typename... Args, detail::function_pointer_kind Function>
uerror check_invocable_error(Function function_ptr) noexcept;

template <typename... Args, detail::function_pointer_kind Function>
uerror check_invocable_error(Function function_ptr, Args&&... args) noexcept;
}  // namespace ME::meta::r

namespace ME::meta::r {
template <typename Instance>
bool is_invocable_with(const member& member) noexcept;

template <typename Instance>
bool is_invocable_with(const member& member, Instance&& instance) noexcept;

template <typename Instance, detail::member_pointer_kind Member>
bool is_invocable_with(Member member_ptr) noexcept;

template <typename Instance, detail::member_pointer_kind Member>
bool is_invocable_with(Member member_ptr, Instance&& instance) noexcept;

template <typename Instance>
uerror check_invocable_error(const member& member) noexcept;

template <typename Instance>
uerror check_invocable_error(const member& member, Instance&& instance) noexcept;

template <typename Instance, detail::member_pointer_kind Member>
uerror check_invocable_error(Member member_ptr) noexcept;

template <typename Instance, detail::member_pointer_kind Member>
uerror check_invocable_error(Member member_ptr, Instance&& instance) noexcept;
}  // namespace ME::meta::r

namespace ME::meta::r {
template <typename Instance, typename... Args>
bool is_invocable_with(const method& method) noexcept;

template <typename Instance, typename... Args>
bool is_invocable_with(const method& method, Instance&& instance, Args&&... args) noexcept;

template <typename Instance, typename... Args, detail::method_pointer_kind Method>
bool is_invocable_with(Method method_ptr) noexcept;

template <typename Instance, typename... Args, detail::method_pointer_kind Method>
bool is_invocable_with(Method method_ptr, Instance&& instance, Args&&... args) noexcept;

template <typename Instance, typename... Args>
uerror check_invocable_error(const method& method) noexcept;

template <typename Instance, typename... Args>
uerror check_invocable_error(const method& method, Instance&& instance, Args&&... args) noexcept;

template <typename Instance, typename... Args, detail::method_pointer_kind Method>
uerror check_invocable_error(Method method_ptr) noexcept;

template <typename Instance, typename... Args, detail::method_pointer_kind Method>
uerror check_invocable_error(Method method_ptr, Instance&& instance, Args&&... args) noexcept;
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <typename T, typename Tp = std::decay_t<T>>
concept arg_lvalue_ref_kind        //
        = (non_uvalue_family<Tp>)  //
          && (std::is_lvalue_reference_v<T>);

template <typename T, typename Tp = std::decay_t<T>>
concept arg_rvalue_ref_kind        //
        = (non_uvalue_family<Tp>)  //
          && (!std::is_reference_v<T> || std::is_rvalue_reference_v<T>);
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename T>
concept inst_class_ref_kind     //
        = (std::is_class_v<T>)  //
          || (std::is_reference_v<T> && std::is_class_v<std::remove_reference_t<T>>);

template <typename T, typename Tp = std::decay_t<T>>
concept inst_class_lvalue_ref_kind            //
        = (non_uvalue_family<Tp>)             //
          && (std::is_lvalue_reference_v<T>)  //
          && (std::is_class_v<std::remove_pointer_t<std::remove_reference_t<T>>>);

template <typename T, typename Tp = std::decay_t<T>>
concept inst_class_rvalue_ref_kind                                       //
        = (non_uvalue_family<Tp>)                                        //
          && (!std::is_reference_v<T> || std::is_rvalue_reference_v<T>)  //
          && (std::is_class_v<std::remove_pointer_t<std::remove_reference_t<T>>>);
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
namespace impl {
template <inst_class_ref_kind Q, bool is_const, bool is_lvalue, bool is_rvalue>
struct inst_traits_impl;

template <inst_class_ref_kind Q>
struct inst_traits_impl<Q, false, false, false> {
    using class_type = std::remove_cvref_t<Q>;
    using method_type = void (class_type::*)();
};

template <inst_class_ref_kind Q>
struct inst_traits_impl<Q, false, true, false> {
    using class_type = std::remove_cvref_t<Q>;
    using method_type = void (class_type::*)() &;
};

template <inst_class_ref_kind Q>
struct inst_traits_impl<Q, false, false, true> {
    using class_type = std::remove_cvref_t<Q>;
    using method_type = void (class_type::*)() &&;
};

template <inst_class_ref_kind Q>
struct inst_traits_impl<Q, true, false, false> {
    using class_type = std::remove_cvref_t<Q>;
    using method_type = void (class_type::*)() const;
};

template <inst_class_ref_kind Q>
struct inst_traits_impl<Q, true, true, false> {
    using class_type = std::remove_cvref_t<Q>;
    using method_type = void (class_type::*)() const&;
};

template <inst_class_ref_kind Q>
struct inst_traits_impl<Q, true, false, true> {
    using class_type = std::remove_cvref_t<Q>;
    using method_type = void (class_type::*)() const&&;
};
}  // namespace impl

template <inst_class_ref_kind Q>
struct inst_traits final : impl::inst_traits_impl<  //
                                   Q, cvref_traits<Q>::is_const, cvref_traits<Q>::is_lvalue, cvref_traits<Q>::is_rvalue> {};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
[[nodiscard]] inline bool is_a(const any_type& base, const any_type& derived) noexcept {
    if (base == derived) {
        return true;
    }

    const class_type& base_class = base.as_class();
    const class_type& derived_class = derived.as_class();

    if (base_class && derived_class) {
        const class_type_data& derived_data = *type_access(derived_class);
        const class_type_data::deep_upcasts_t& deep_upcasts = derived_data.deep_upcasts;

        if (auto iter{deep_upcasts.find(base)}; iter != deep_upcasts.end() && !iter->second.is_ambiguous) {
            return true;
        }
    }

    return false;
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
[[nodiscard]] inline void* pointer_upcast(void* ptr, const class_type& from, const class_type& to) {
    if (nullptr == ptr || !from || !to) {
        return nullptr;
    }

    if (from == to) {
        return ptr;
    }

    const class_type_data& from_data = *type_access(from);
    const class_type_data::deep_upcasts_t& deep_upcasts = from_data.deep_upcasts;

    if (auto iter{deep_upcasts.find(to)}; iter != deep_upcasts.end() && !iter->second.is_ambiguous) {
        return iter->second.apply(ptr);
    }

    return nullptr;
}

[[nodiscard]] inline const void* pointer_upcast(const void* ptr, const class_type& from, const class_type& to) {
    // NOLINTNEXTLINE(*-const-cast)
    return pointer_upcast(const_cast<void*>(ptr), from, to);
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
[[nodiscard]] inline void* pointer_upcast(void* ptr, const any_type& from, const any_type& to) {
    if (nullptr == ptr || !from || !to) {
        return nullptr;
    }

    if (to.is_void() || from == to) {
        return ptr;
    }

    const class_type& to_class = to.as_class();
    const class_type& from_class = from.as_class();

    if (to_class && from_class) {
        if (void* base_ptr = pointer_upcast(ptr, from_class, to_class)) {
            return base_ptr;
        }
    }

    return nullptr;
}

[[nodiscard]] inline const void* pointer_upcast(const void* ptr, const any_type& from, const any_type& to) {
    // NOLINTNEXTLINE(*-const-cast)
    return pointer_upcast(const_cast<void*>(ptr), from, to);
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename To, typename From>
[[nodiscard]] To* pointer_upcast(type_registry& registry, From* ptr) {
    return static_cast<To*>(pointer_upcast(  //
            ptr, registry.resolve_type<From>(), registry.resolve_type<To>()));
}

template <typename To, typename From>
[[nodiscard]] const To* pointer_upcast(type_registry& registry, const From* ptr) {
    return static_cast<const To*>(pointer_upcast(  //
            ptr, registry.resolve_type<From>(), registry.resolve_type<To>()));
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
class uarg_base {
public:
    enum class ref_types {
        lvalue,
        const_lvalue,
        rvalue,
        const_rvalue,
    };

public:
    uarg_base() = default;
    ~uarg_base() = default;

    uarg_base(uarg_base&&) = default;
    uarg_base(const uarg_base&) = default;

    uarg_base& operator=(uarg_base&&) = delete;
    uarg_base& operator=(const uarg_base&) = delete;

    template <typename T, non_uvalue_family Tp = std::decay_t<T>>
    explicit uarg_base(type_registry& registry, T&&) : uarg_base{registry, type_list<T&&>{}} {}

    template <arg_lvalue_ref_kind T>
    explicit uarg_base(type_registry& registry, type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_lvalue : ref_types::lvalue}, raw_type_{registry.resolve_type<std::remove_cvref_t<T>>()} {}

    template <arg_rvalue_ref_kind T>
    explicit uarg_base(type_registry& registry, type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_rvalue : ref_types::rvalue}, raw_type_{registry.resolve_type<std::remove_cvref_t<T>>()} {}

    explicit uarg_base(type_registry&, uvalue& v) : ref_type_{ref_types::lvalue}, raw_type_{v.get_type()} {}

    explicit uarg_base(type_registry&, const uvalue& v) : ref_type_{ref_types::const_lvalue}, raw_type_{v.get_type()} {}

    explicit uarg_base(type_registry&, uvalue&& v) : ref_type_{ref_types::rvalue}, raw_type_{v.get_type()} {}

    explicit uarg_base(type_registry&, const uvalue&& v) : ref_type_{ref_types::const_rvalue}, raw_type_{v.get_type()} {}

    template <typename T, typename Tp = std::decay_t<T>>
        requires std::is_same_v<Tp, uresult>
    explicit uarg_base(type_registry& registry, T&& v) : uarg_base{registry, *std::forward<T>(v)} {}

    [[nodiscard]] bool is_ref_const() const noexcept {
        return ref_type_ == ref_types::const_lvalue  //
               || ref_type_ == ref_types::const_rvalue;
    }

    [[nodiscard]] ref_types get_ref_type() const noexcept { return ref_type_; }

    [[nodiscard]] any_type get_raw_type() const noexcept { return raw_type_; }

    template <non_function_pointer_kind To>
    [[nodiscard]] bool can_cast_to(type_registry& registry) const noexcept;

    template <typename To>
        requires(!non_function_pointer_kind<To>)
    [[nodiscard]] bool can_cast_to(type_registry& registry) const noexcept;

private:
    ref_types ref_type_{};
    any_type raw_type_{};
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
class uarg final : public uarg_base {
public:
    uarg() = default;
    ~uarg() = default;

    uarg(uarg&&) = default;
    uarg(const uarg&) = default;

    uarg& operator=(uarg&&) = delete;
    uarg& operator=(const uarg&) = delete;

    template <typename T, typename Tp = std::decay_t<T>>
        requires std::is_same_v<Tp, uvalue>
    explicit uarg(type_registry& registry, T&& v) : uarg_base{registry, std::forward<T>(v)}, data_{const_cast<void*>(v.get_data())} {  // NOLINT(*-const-cast)
        // there is no 'use after move' here because
        // 'uarg_base' doesn't actually move 'v', just gets its type
    }

    template <typename T, typename Tp = std::decay_t<T>>
        requires std::is_same_v<Tp, uresult>
    explicit uarg(type_registry& registry, T&& v) : uarg_base{registry, std::forward<T>(v)}, data_{const_cast<void*>(v->get_data())} {  // NOLINT(*-const-cast)
        // there is no 'use after move' here because
        // 'uarg_base' doesn't actually move 'v', just gets its type
    }

    template <typename T, non_uvalue_family Tp = std::decay_t<T>>
    explicit uarg(type_registry& registry, T&& v) : uarg_base{registry, std::forward<T>(v)}, data_{const_cast<std::remove_cvref_t<T>*>(std::addressof(v))} {  // NOLINT(*-const-cast)
        // there is no 'use after move' here because
        // 'uarg_base' doesn't actually move 'v', just gets its type
    }

    template <non_function_pointer_kind To>
    [[nodiscard]] decltype(auto) cast(type_registry& registry) const;

    template <typename To>
        requires(!non_function_pointer_kind<To>)
    [[nodiscard]] decltype(auto) cast(type_registry& registry) const;

private:
    void* data_{};
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <non_function_pointer_kind To>
[[nodiscard]] bool uarg_base::can_cast_to(type_registry& registry) const noexcept {
    using to_raw_type = std::remove_cv_t<To>;

    const any_type& from_type = get_raw_type();
    const pointer_type& to_type_ptr = registry.resolve_type<to_raw_type>();

    if (from_type.is_nullptr()) {
        return true;
    }

    if (from_type.is_array()) {
        const array_type& from_type_array = from_type.as_array();

        const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);
        const bool from_type_array_readonly = is_ref_const();

        const any_type& to_data_type = to_type_ptr.get_data_type();
        const any_type& from_data_type = from_type_array.get_data_type();

        if (to_type_ptr_readonly >= from_type_array_readonly) {
            if (to_data_type.is_void() || is_a(to_data_type, from_data_type)) {
                return true;
            }
        }
    }

    if (from_type.is_pointer()) {
        const pointer_type& from_type_ptr = from_type.as_pointer();

        const bool to_type_ptr_readonly = to_type_ptr.get_flags().has(pointer_flags::is_readonly);
        const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);

        const any_type& to_data_type = to_type_ptr.get_data_type();
        const any_type& from_data_type = from_type_ptr.get_data_type();

        if (to_type_ptr_readonly >= from_type_ptr_readonly) {
            if (to_data_type.is_void() || is_a(to_data_type, from_data_type)) {
                return true;
            }
        }
    }

    return false;
}

template <typename To>
    requires(!non_function_pointer_kind<To>)
[[nodiscard]] bool uarg_base::can_cast_to(type_registry& registry) const noexcept {
    using to_raw_type_cv = std::remove_reference_t<To>;
    using to_raw_type = std::remove_cv_t<to_raw_type_cv>;

    static_assert(  //
            !(std::is_reference_v<To> && pointer_kind<to_raw_type>), "references to pointers are not supported yet");

    const any_type& from_type = get_raw_type();
    const any_type& to_type = registry.resolve_type<to_raw_type>();

    const auto is_convertible_to_ref = [this]<typename ToRef>(type_list<ToRef>) {
        switch (get_ref_type()) {
            case ref_types::lvalue:
                return std::is_convertible_v<noncopyable&, copy_cvref_t<ToRef, noncopyable>>;
            case ref_types::const_lvalue:
                return std::is_convertible_v<const noncopyable&, copy_cvref_t<ToRef, noncopyable>>;
            case ref_types::rvalue:
                return std::is_convertible_v<noncopyable&&, copy_cvref_t<ToRef, noncopyable>>;
            case ref_types::const_rvalue:
                return std::is_convertible_v<const noncopyable&&, copy_cvref_t<ToRef, noncopyable>>;
        }
        return false;
    };

    const auto is_constructible_from_type = [this, &is_convertible_to_ref]<typename FromType>(type_list<FromType>) {
        switch (get_ref_type()) {
            case ref_types::lvalue:
                return std::is_constructible_v<To, FromType&> && is_convertible_to_ref(type_list<FromType&>{});
            case ref_types::const_lvalue:
                return std::is_constructible_v<To, const FromType&> && is_convertible_to_ref(type_list<const FromType&>{});
            case ref_types::rvalue:
                return std::is_constructible_v<To, FromType&&> && is_convertible_to_ref(type_list<FromType&&>{});
            case ref_types::const_rvalue:
                return std::is_constructible_v<To, const FromType&&> && is_convertible_to_ref(type_list<const FromType&&>{});
        }
        return false;
    };

    if constexpr (std::is_reference_v<To>) {
        if (is_a(to_type, from_type) && is_convertible_to_ref(type_list<To>{})) {
            return true;
        }
    }

    if constexpr (!std::is_reference_v<To>) {
        if (is_a(to_type, from_type) && is_constructible_from_type(type_list<to_raw_type>{})) {
            return true;
        }
    }

    return false;
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <non_function_pointer_kind To>
[[nodiscard]] decltype(auto) uarg::cast(type_registry& registry) const {
    META_HPP_DEV_ASSERT(can_cast_to<To>(registry) && "bad argument cast");

    using to_raw_type = std::remove_cv_t<To>;

    const any_type& from_type = get_raw_type();
    const pointer_type& to_type_ptr = registry.resolve_type<to_raw_type>();

    if (from_type.is_nullptr()) {
        return static_cast<To>(nullptr);
    }

    if (from_type.is_array()) {
        const array_type& from_type_array = from_type.as_array();

        void* to_ptr = pointer_upcast(  //
                data_, from_type_array.get_data_type(), to_type_ptr.get_data_type());
        META_HPP_ASSERT(to_ptr);

        return static_cast<To>(to_ptr);
    }

    if (from_type.is_pointer()) {
        const pointer_type& from_type_ptr = from_type.as_pointer();

        void* to_ptr = pointer_upcast(  //
                *static_cast<void**>(data_), from_type_ptr.get_data_type(), to_type_ptr.get_data_type());
        META_HPP_ASSERT(to_ptr);

        return static_cast<To>(to_ptr);
    }

    throw_exception(error_code::bad_argument_cast);
}

template <typename To>
    requires(!non_function_pointer_kind<To>)
[[nodiscard]] decltype(auto) uarg::cast(type_registry& registry) const {
    META_HPP_DEV_ASSERT(can_cast_to<To>(registry) && "bad argument cast");

    using to_raw_type_cv = std::remove_reference_t<To>;
    using to_raw_type = std::remove_cv_t<to_raw_type_cv>;

    static_assert(  //
            !(std::is_reference_v<To> && pointer_kind<to_raw_type>), "references to pointers are not supported yet");

    const any_type& from_type = get_raw_type();
    const any_type& to_type = registry.resolve_type<to_raw_type>();

    void* to_ptr = pointer_upcast(data_, from_type, to_type);
    META_HPP_ASSERT(to_ptr);

    if constexpr (std::is_lvalue_reference_v<To>) {
        return *static_cast<to_raw_type_cv*>(to_ptr);
    }

    if constexpr (std::is_rvalue_reference_v<To>) {
        return std::move(*static_cast<to_raw_type_cv*>(to_ptr));
    }

    if constexpr (!std::is_reference_v<To>) {
        switch (get_ref_type()) {
            case ref_types::lvalue:
                if constexpr (std::is_constructible_v<To, to_raw_type&>) {
                    return To{*static_cast<to_raw_type*>(to_ptr)};
                }
                break;
            case ref_types::const_lvalue:
                if constexpr (std::is_constructible_v<To, const to_raw_type&>) {
                    return To{*static_cast<const to_raw_type*>(to_ptr)};
                }
                break;
            case ref_types::rvalue:
                if constexpr (std::is_constructible_v<To, to_raw_type&&>) {
                    return To{std::move(*static_cast<to_raw_type*>(to_ptr))};
                }
                break;
            case ref_types::const_rvalue:
                if constexpr (std::is_constructible_v<To, const to_raw_type&&>) {
                    return To{std::move(*static_cast<const to_raw_type*>(to_ptr))};
                }
                break;
        }
        throw_exception(error_code::bad_argument_cast);
    }
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename ArgTypeList>
bool can_cast_all_uargs(type_registry& registry, std::span<const uarg> args) noexcept {
    if (args.size() != type_list_arity_v<ArgTypeList>) {
        return false;
    }
    return [ args, &registry ]<std::size_t... Is>(std::index_sequence<Is...>) { return (... && args[Is].can_cast_to<type_list_at_t<Is, ArgTypeList>>(registry)); }
    (std::make_index_sequence<type_list_arity_v<ArgTypeList>>());
}

template <typename ArgTypeList>
bool can_cast_all_uargs(type_registry& registry, std::span<const uarg_base> args) noexcept {
    if (args.size() != type_list_arity_v<ArgTypeList>) {
        return false;
    }
    return [ args, &registry ]<std::size_t... Is>(std::index_sequence<Is...>) { return (... && args[Is].can_cast_to<type_list_at_t<Is, ArgTypeList>>(registry)); }
    (std::make_index_sequence<type_list_arity_v<ArgTypeList>>());
}

template <typename ArgTypeList, typename F>
auto unchecked_call_with_uargs(type_registry& registry, std::span<const uarg> args, F&& f) {
    META_HPP_DEV_ASSERT(args.size() == type_list_arity_v<ArgTypeList>);
    return [ args, &registry, &f ]<std::size_t... Is>(std::index_sequence<Is...>) { return f(args[Is].cast<type_list_at_t<Is, ArgTypeList>>(registry)...); }
    (std::make_index_sequence<type_list_arity_v<ArgTypeList>>());
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <function_kind Function>
function_type_data::function_type_data(type_list<Function>)
    : type_data_base{type_kind::function_},
      flags{function_traits<Function>::make_flags()},
      return_type{resolve_type<typename function_traits<Function>::return_type>()},
      argument_types{resolve_types(typename function_traits<Function>::argument_types{})} {}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline function_bitflags function_type::get_flags() const noexcept { return data_->flags; }

inline std::size_t function_type::get_arity() const noexcept { return data_->argument_types.size(); }

inline any_type function_type::get_return_type() const noexcept { return data_->return_type; }

inline any_type function_type::get_argument_type(std::size_t position) const noexcept { return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{}; }

inline const any_type_list& function_type::get_argument_types() const noexcept { return data_->argument_types; }
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <function_policy_family Policy, function_pointer_kind Function>
uvalue raw_function_invoke(type_registry& registry, Function function_ptr, std::span<const uarg> args) {
    using ft = function_traits<std::remove_pointer_t<Function>>;
    using return_type = typename ft::return_type;
    using argument_types = typename ft::argument_types;

    constexpr bool as_copy                               //
            = std::is_copy_constructible_v<return_type>  //
              && std::is_same_v<Policy, function_policy::as_copy_t>;

    constexpr bool as_void                 //
            = std::is_void_v<return_type>  //
              || std::is_same_v<Policy, function_policy::discard_return_t>;

    constexpr bool ref_as_ptr                   //
            = std::is_reference_v<return_type>  //
              && std::is_same_v<Policy, function_policy::return_reference_as_pointer_t>;

    static_assert(as_copy || as_void || ref_as_ptr);

    META_HPP_ASSERT(                  //
            args.size() == ft::arity  //
            && "an attempt to call a function with an incorrect arity");

    META_HPP_ASSERT(                                            //
            can_cast_all_uargs<argument_types>(registry, args)  //
            && "an attempt to call a function with incorrect argument types");

    return unchecked_call_with_uargs<argument_types>(registry, args, [function_ptr](auto&&... all_args) {
        if constexpr (std::is_void_v<return_type>) {
            function_ptr(META_HPP_FWD(all_args)...);
            return uvalue{};
        }

        if constexpr (std::is_same_v<Policy, function_policy::discard_return_t>) {
            std::ignore = function_ptr(META_HPP_FWD(all_args)...);
            return uvalue{};
        }

        if constexpr (!std::is_void_v<return_type>) {
            return_type&& result = function_ptr(META_HPP_FWD(all_args)...);
            return ref_as_ptr ? uvalue{std::addressof(result)} : uvalue{META_HPP_FWD(result)};
        }
    });
}

template <function_pointer_kind Function>
uerror raw_function_invoke_error(type_registry& registry, std::span<const uarg_base> args) noexcept {
    using ft = function_traits<std::remove_pointer_t<Function>>;
    using argument_types = typename ft::argument_types;

    if (args.size() != ft::arity) {
        return uerror{error_code::arity_mismatch};
    }

    if (!can_cast_all_uargs<argument_types>(registry, args)) {
        return uerror{error_code::argument_type_mismatch};
    }

    return uerror{error_code::no_error};
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <function_policy_family Policy, function_pointer_kind Function>
function_state::invoke_impl make_function_invoke(type_registry& registry, Function function_ptr) {
    return [&registry, function_ptr](std::span<const uarg> args) {  //
        return raw_function_invoke<Policy>(registry, function_ptr, args);
    };
}

template <function_pointer_kind Function>
function_state::invoke_error_impl make_function_invoke_error(type_registry& registry) {
    return [&registry](std::span<const uarg_base> args) {  //
        return raw_function_invoke_error<Function>(registry, args);
    };
}

template <function_pointer_kind Function>
argument_list make_function_arguments() {
    using ft = function_traits<std::remove_pointer_t<Function>>;
    using ft_argument_types = typename ft::argument_types;

    return []<std::size_t... Is>(std::index_sequence<Is...>) {
        [[maybe_unused]] const auto make_argument = []<std::size_t I>(index_constant<I>) {
            using P = type_list_at_t<I, ft_argument_types>;
            return argument{argument_state::make<P>(I, metadata_map{})};
        };
        return argument_list{make_argument(index_constant<Is>{})...};
    }
    (std::make_index_sequence<ft::arity>());
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
inline function_state::function_state(function_index nindex, metadata_map nmetadata) : index{std::move(nindex)}, metadata{std::move(nmetadata)} {}

template <function_policy_family Policy, function_pointer_kind Function>
function_state_ptr function_state::make(std::string name, Function function_ptr, metadata_map metadata) {
    type_registry& registry{type_registry::instance()};

    function_state state{
            function_index{registry.resolve_type<std::remove_pointer_t<Function>>(), std::move(name)},
            std::move(metadata),
    };

    state.invoke = make_function_invoke<Policy>(registry, function_ptr);
    state.invoke_error = make_function_invoke_error<Function>(registry);
    state.arguments = make_function_arguments<Function>();

    return make_intrusive<function_state>(std::move(state));
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline function_type function::get_type() const noexcept { return state_->index.get_type(); }

inline const std::string& function::get_name() const noexcept { return state_->index.get_name(); }

template <typename... Args>
uvalue function::invoke(Args&&... args) const {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
    return state_->invoke(vargs);
}

template <typename... Args>
uresult function::try_invoke(Args&&... args) const {
    using namespace detail;
    type_registry& registry{type_registry::instance()};

    {
        // doesn't actually move 'args', just checks conversion errors
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
        if (const uerror err = state_->invoke_error(vargs)) {
            return err;
        }
    }

    const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
    return state_->invoke(vargs);
}

template <typename... Args>
uvalue function::operator()(Args&&... args) const {
    return invoke(META_HPP_FWD(args)...);
}

template <typename... Args>
bool function::is_invocable_with() const noexcept {
    return !check_invocable_error<Args...>();
}

template <typename... Args>
bool function::is_invocable_with(Args&&... args) const noexcept {
    return !check_invocable_error(META_HPP_FWD(args)...);
}

template <typename... Args>
uerror function::check_invocable_error() const noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
    return state_->invoke_error(vargs);
}

template <typename... Args>
uerror function::check_invocable_error(Args&&... args) const noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
    return state_->invoke_error(vargs);
}

inline argument function::get_argument(std::size_t position) const noexcept { return position < state_->arguments.size() ? state_->arguments[position] : argument{}; }

inline const argument_list& function::get_arguments() const noexcept { return state_->arguments; }
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
class uinst_base {
public:
    enum class ref_types {
        lvalue,
        const_lvalue,
        rvalue,
        const_rvalue,
    };

public:
    uinst_base() = default;
    ~uinst_base() = default;

    uinst_base(uinst_base&&) = default;
    uinst_base(const uinst_base&) = default;

    uinst_base& operator=(uinst_base&&) = delete;
    uinst_base& operator=(const uinst_base&) = delete;

    template <typename T, non_uvalue_family Tp = std::decay_t<T>>
    explicit uinst_base(type_registry& registry, T&&) : uinst_base{registry, type_list<T&&>{}} {}

    template <inst_class_lvalue_ref_kind T>
    explicit uinst_base(type_registry& registry, type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_lvalue : ref_types::lvalue}, raw_type_{registry.resolve_type<std::remove_cvref_t<T>>()} {}

    template <inst_class_rvalue_ref_kind T>
    explicit uinst_base(type_registry& registry, type_list<T>)
        : ref_type_{std::is_const_v<std::remove_reference_t<T>> ? ref_types::const_rvalue : ref_types::rvalue}, raw_type_{registry.resolve_type<std::remove_cvref_t<T>>()} {}

    explicit uinst_base(type_registry&, uvalue& v) : ref_type_{ref_types::lvalue}, raw_type_{v.get_type()} {}

    explicit uinst_base(type_registry&, const uvalue& v) : ref_type_{ref_types::const_lvalue}, raw_type_{v.get_type()} {}

    explicit uinst_base(type_registry&, uvalue&& v) : ref_type_{ref_types::rvalue}, raw_type_{v.get_type()} {}

    explicit uinst_base(type_registry&, const uvalue&& v) : ref_type_{ref_types::const_rvalue}, raw_type_{v.get_type()} {}

    template <typename T, typename Tp = std::decay_t<T>>
        requires std::is_same_v<Tp, uresult>
    explicit uinst_base(type_registry& registry, T&& v) : uinst_base{registry, *std::forward<T>(v)} {}

    [[nodiscard]] bool is_inst_const() const noexcept {
        if (raw_type_.is_pointer()) {
            const pointer_type& from_type_ptr = raw_type_.as_pointer();
            const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);
            return from_type_ptr_readonly;
        }
        return ref_type_ == ref_types::const_lvalue  //
               || ref_type_ == ref_types::const_rvalue;
    }

    [[nodiscard]] ref_types get_ref_type() const noexcept { return ref_type_; }

    [[nodiscard]] any_type get_raw_type() const noexcept { return raw_type_; }

    template <inst_class_ref_kind Q>
    [[nodiscard]] bool can_cast_to(type_registry& registry) const noexcept;

private:
    ref_types ref_type_{};
    any_type raw_type_{};
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
class uinst final : public uinst_base {
public:
    uinst() = default;
    ~uinst() = default;

    uinst(uinst&&) = default;
    uinst(const uinst&) = default;

    uinst& operator=(uinst&&) = delete;
    uinst& operator=(const uinst&) = delete;

    template <typename T, typename Tp = std::decay_t<T>>
        requires std::is_same_v<Tp, uvalue>
    explicit uinst(type_registry& registry, T&& v) : uinst_base{registry, std::forward<T>(v)}, data_{const_cast<void*>(v.get_data())} {  // NOLINT(*-const-cast)
        // there is no 'use after move' here because
        // 'uinst_base' doesn't actually move 'v', just gets its type
    }

    template <typename T, typename Tp = std::decay_t<T>>
        requires std::is_same_v<Tp, uresult>
    explicit uinst(type_registry& registry, T&& v) : uinst_base{registry, std::forward<T>(v)}, data_{const_cast<void*>(v->get_data())} {  // NOLINT(*-const-cast)
        // there is no 'use after move' here because
        // 'uinst_base' doesn't actually move 'v', just gets its type
    }

    template <typename T, non_uvalue_family Tp = std::decay_t<T>>
    explicit uinst(type_registry& registry, T&& v) : uinst_base{registry, std::forward<T>(v)}, data_{const_cast<std::remove_cvref_t<T>*>(std::addressof(v))} {  // NOLINT(*-const-cast)
        // there is no 'use after move' here because
        // 'uinst_base' doesn't actually move 'v', just gets its type
    }

    template <inst_class_ref_kind Q>
    [[nodiscard]] decltype(auto) cast(type_registry& registry) const;

private:
    void* data_{};
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <inst_class_ref_kind Q>
bool uinst_base::can_cast_to(type_registry& registry) const noexcept {
    using inst_class = typename inst_traits<Q>::class_type;
    using inst_method = typename inst_traits<Q>::method_type;

    const any_type& from_type = get_raw_type();
    const any_type& to_type = registry.resolve_type<inst_class>();

    if (from_type.is_class()) {
        const auto is_invocable = [this]() {
            switch (get_ref_type()) {
                case ref_types::lvalue:
                    return std::is_invocable_v<inst_method, inst_class&>;
                case ref_types::const_lvalue:
                    return std::is_invocable_v<inst_method, const inst_class&>;
                case ref_types::rvalue:
                    return std::is_invocable_v<inst_method, inst_class&&>;
                case ref_types::const_rvalue:
                    return std::is_invocable_v<inst_method, const inst_class&&>;
            }
            return false;
        };

        return is_invocable() && is_a(to_type, from_type);
    }

    if (from_type.is_pointer()) {
        const pointer_type& from_type_ptr = from_type.as_pointer();
        const bool from_type_ptr_readonly = from_type_ptr.get_flags().has(pointer_flags::is_readonly);
        const any_type& from_data_type = from_type_ptr.get_data_type();

        const auto is_invocable = [from_type_ptr_readonly]() { return from_type_ptr_readonly ? std::is_invocable_v<inst_method, const inst_class&> : std::is_invocable_v<inst_method, inst_class&>; };

        return is_invocable() && is_a(to_type, from_data_type);
    }

    return false;
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <inst_class_ref_kind Q>
decltype(auto) uinst::cast(type_registry& registry) const {
    META_HPP_DEV_ASSERT(can_cast_to<Q>(registry) && "bad instance cast");

    using inst_class_cv = std::remove_reference_t<Q>;
    using inst_class = std::remove_cv_t<inst_class_cv>;

    const any_type& from_type = get_raw_type();
    const any_type& to_type = registry.resolve_type<inst_class>();

    if (from_type.is_class() && to_type.is_class()) {
        void* to_ptr = pointer_upcast(  //
                data_, from_type.as_class(), to_type.as_class());
        META_HPP_ASSERT(to_ptr);

        if constexpr (!std::is_reference_v<Q>) {
            return *static_cast<inst_class_cv*>(to_ptr);
        }

        if constexpr (std::is_lvalue_reference_v<Q>) {
            return *static_cast<inst_class_cv*>(to_ptr);
        }

        if constexpr (std::is_rvalue_reference_v<Q>) {
            return std::move(*static_cast<inst_class_cv*>(to_ptr));
        }
    }

    if (from_type.is_pointer()) {
        const pointer_type& from_type_ptr = from_type.as_pointer();
        const any_type& from_data_type = from_type_ptr.get_data_type();

        if (from_data_type.is_class() && to_type.is_class()) {
            void* to_ptr = pointer_upcast(  //
                    *static_cast<void**>(data_), from_data_type.as_class(), to_type.as_class());
            META_HPP_ASSERT(to_ptr);

            if constexpr (!std::is_reference_v<Q>) {
                return *static_cast<inst_class_cv*>(to_ptr);
            }

            if constexpr (std::is_lvalue_reference_v<Q>) {
                return *static_cast<inst_class_cv*>(to_ptr);
            }
        }
    }

    throw_exception(error_code::bad_instance_cast);
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <member_pointer_kind Member>
member_type_data::member_type_data(type_list<Member>)
    : type_data_base{type_kind::member_},
      flags{member_traits<Member>::make_flags()},
      owner_type{resolve_type<typename member_traits<Member>::class_type>()},
      value_type{resolve_type<typename member_traits<Member>::value_type>()} {}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline member_bitflags member_type::get_flags() const noexcept { return data_->flags; }

inline class_type member_type::get_owner_type() const noexcept { return data_->owner_type; }

inline any_type member_type::get_value_type() const noexcept { return data_->value_type; }
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <member_policy_family Policy, member_pointer_kind Member>
uvalue raw_member_getter(type_registry& registry, Member member_ptr, const uinst& inst) {
    using mt = member_traits<Member>;
    using class_type = typename mt::class_type;
    using value_type = typename mt::value_type;

    constexpr bool as_copy                                          //
            = std::is_copy_constructible_v<value_type>              //
              && std::is_same_v<Policy, member_policy::as_copy_t>;  //

    constexpr bool as_ptr                                           //
            = std::is_same_v<Policy, member_policy::as_pointer_t>;  //

    constexpr bool as_ref_wrap                                                //
            = std::is_same_v<Policy, member_policy::as_reference_wrapper_t>;  //

    static_assert(as_copy || as_ptr || as_ref_wrap);

    if (inst.is_inst_const()) {
        META_HPP_ASSERT(                                      //
                inst.can_cast_to<const class_type>(registry)  //
                && "an attempt to get a member with an incorrect instance type");

        auto&& return_value = inst.cast<const class_type>(registry).*member_ptr;

        if constexpr (as_copy) {
            return uvalue{META_HPP_FWD(return_value)};
        }

        if constexpr (as_ptr) {
            return uvalue{std::addressof(return_value)};
        }

        if constexpr (as_ref_wrap) {
            return uvalue{std::ref(return_value)};
        }
    } else {
        META_HPP_ASSERT(                                //
                inst.can_cast_to<class_type>(registry)  //
                && "an attempt to get a member with an incorrect instance type");

        auto&& return_value = inst.cast<class_type>(registry).*member_ptr;

        if constexpr (as_copy) {
            return uvalue{META_HPP_FWD(return_value)};
        }

        if constexpr (as_ptr) {
            return uvalue{std::addressof(return_value)};
        }

        if constexpr (as_ref_wrap) {
            return uvalue{std::ref(return_value)};
        }
    }
}

template <member_pointer_kind Member>
uerror raw_member_getter_error(type_registry& registry, const uinst_base& inst) noexcept {
    using mt = member_traits<Member>;
    using class_type = typename mt::class_type;

    if (inst.is_inst_const()) {
        if (!inst.can_cast_to<const class_type>(registry)) {
            return uerror{error_code::bad_instance_cast};
        }
    } else {
        if (!inst.can_cast_to<class_type>(registry)) {
            return uerror{error_code::bad_instance_cast};
        }
    }

    return uerror{error_code::no_error};
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <member_pointer_kind Member>
void raw_member_setter(type_registry& registry, Member member_ptr, const uinst& inst, const uarg& arg) {
    using mt = member_traits<Member>;
    using class_type = typename mt::class_type;
    using value_type = typename mt::value_type;

    if constexpr (std::is_const_v<value_type>) {
        (void)registry;
        (void)member_ptr;
        (void)inst;
        (void)arg;
        META_HPP_ASSERT(false && "an attempt to set a constant member");
    } else {
        META_HPP_ASSERT(               //
                !inst.is_inst_const()  //
                && "an attempt to set a member with an const instance type");

        META_HPP_ASSERT(                                //
                inst.can_cast_to<class_type>(registry)  //
                && "an attempt to set a member with an incorrect instance type");

        META_HPP_ASSERT(                               //
                arg.can_cast_to<value_type>(registry)  //
                && "an attempt to set a member with an incorrect argument type");

        inst.cast<class_type>(registry).*member_ptr = arg.cast<value_type>(registry);
    }
}

template <member_pointer_kind Member>
uerror raw_member_setter_error(type_registry& registry, const uinst_base& inst, const uarg_base& arg) noexcept {
    using mt = member_traits<Member>;
    using class_type = typename mt::class_type;
    using value_type = typename mt::value_type;

    if constexpr (std::is_const_v<value_type>) {
        (void)registry;
        (void)inst;
        (void)arg;
        return uerror{error_code::bad_const_access};
    } else {
        if (inst.is_inst_const()) {
            return uerror{error_code::bad_const_access};
        }

        if (!inst.can_cast_to<class_type>(registry)) {
            return uerror{error_code::instance_type_mismatch};
        }

        if (!arg.can_cast_to<value_type>(registry)) {
            return uerror{error_code::argument_type_mismatch};
        }

        return uerror{error_code::no_error};
    }
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <member_policy_family Policy, member_pointer_kind Member>
member_state::getter_impl make_member_getter(type_registry& registry, Member member_ptr) {
    return [&registry, member_ptr](const uinst& inst) {  //
        return raw_member_getter<Policy>(registry, member_ptr, inst);
    };
}

template <member_pointer_kind Member>
member_state::getter_error_impl make_member_getter_error(type_registry& registry) {
    return [&registry](const uinst_base& inst) {  //
        return raw_member_getter_error<Member>(registry, inst);
    };
}

template <member_pointer_kind Member>
member_state::setter_impl make_member_setter(type_registry& registry, Member member_ptr) {
    return [&registry, member_ptr](const uinst& inst, const uarg& arg) {  //
        return raw_member_setter(registry, member_ptr, inst, arg);
    };
}

template <member_pointer_kind Member>
member_state::setter_error_impl make_member_setter_error(type_registry& registry) {
    return [&registry](const uinst_base& inst, const uarg_base& arg) {  //
        return raw_member_setter_error<Member>(registry, inst, arg);
    };
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
inline member_state::member_state(member_index nindex, metadata_map nmetadata) : index{std::move(nindex)}, metadata{std::move(nmetadata)} {}

template <member_policy_family Policy, member_pointer_kind Member>
member_state_ptr member_state::make(std::string name, Member member_ptr, metadata_map metadata) {
    type_registry& registry{type_registry::instance()};

    member_state state{
            member_index{registry.resolve_type<Member>(), std::move(name)},
            std::move(metadata),
    };

    state.getter = make_member_getter<Policy>(registry, member_ptr);
    state.setter = make_member_setter(registry, member_ptr);
    state.getter_error = make_member_getter_error<Member>(registry);
    state.setter_error = make_member_setter_error<Member>(registry);

    return make_intrusive<member_state>(std::move(state));
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline member_type member::get_type() const noexcept { return state_->index.get_type(); }

inline const std::string& member::get_name() const noexcept { return state_->index.get_name(); }

template <typename Instance>
uvalue member::get(Instance&& instance) const {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uinst vinst{registry, META_HPP_FWD(instance)};
    return state_->getter(vinst);
}

template <typename Instance>
uresult member::try_get(Instance&& instance) const {
    using namespace detail;
    type_registry& registry{type_registry::instance()};

    {
        // doesn't actually move an 'instance', just checks conversion errors
        const uinst_base vinst{registry, META_HPP_FWD(instance)};
        if (const uerror err = state_->getter_error(vinst)) {
            return err;
        }
    }

    const uinst vinst{registry, META_HPP_FWD(instance)};
    return state_->getter(vinst);
}

template <typename Instance>
uvalue member::operator()(Instance&& instance) const {
    return get(META_HPP_FWD(instance));
}

template <typename Instance, typename Value>
void member::set(Instance&& instance, Value&& value) const {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uinst vinst{registry, META_HPP_FWD(instance)};
    const uarg vvalue{registry, META_HPP_FWD(value)};
    state_->setter(vinst, vvalue);
}

template <typename Instance, typename Value>
uresult member::try_set(Instance&& instance, Value&& value) const {
    using namespace detail;
    type_registry& registry{type_registry::instance()};

    {
        // doesn't actually move an 'instance' and 'args', just checks conversion errors
        const uinst_base vinst{registry, META_HPP_FWD(instance)};
        const uarg_base vvalue{registry, META_HPP_FWD(value)};
        if (const uerror err = state_->setter_error(vinst, vvalue)) {
            return err;
        }
    }

    const uinst vinst{registry, META_HPP_FWD(instance)};
    const uarg vvalue{registry, META_HPP_FWD(value)};
    state_->setter(vinst, vvalue);
    return uerror{error_code::no_error};
}

template <typename Instance, typename Value>
void member::operator()(Instance&& instance, Value&& value) const {
    set(META_HPP_FWD(instance), META_HPP_FWD(value));
}

template <typename Instance>
[[nodiscard]] bool member::is_gettable_with() const noexcept {
    return !check_gettable_error<Instance>();
}

template <typename Instance>
[[nodiscard]] bool member::is_gettable_with(Instance&& instance) const noexcept {
    return !check_gettable_error(META_HPP_FWD(instance));
}

template <typename Instance, typename Value>
[[nodiscard]] bool member::is_settable_with() const noexcept {
    return !check_settable_error<Instance, Value>();
}

template <typename Instance, typename Value>
[[nodiscard]] bool member::is_settable_with(Instance&& instance, Value&& value) const noexcept {
    return !check_settable_error(META_HPP_FWD(instance), META_HPP_FWD(value));
}

template <typename Instance>
[[nodiscard]] uerror member::check_gettable_error() const noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uinst_base vinst{registry, type_list<Instance>{}};
    return state_->getter_error(vinst);
}

template <typename Instance>
[[nodiscard]] uerror member::check_gettable_error(Instance&& instance) const noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uinst_base vinst{registry, META_HPP_FWD(instance)};
    return state_->getter_error(vinst);
}

template <typename Instance, typename Value>
[[nodiscard]] uerror member::check_settable_error() const noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uinst_base vinst{registry, type_list<Instance>{}};
    const uarg_base vvalue{registry, type_list<Value>{}};
    return state_->setter_error(vinst, vvalue);
}

template <typename Instance, typename Value>
[[nodiscard]] uerror member::check_settable_error(Instance&& instance, Value&& value) const noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uinst_base vinst{registry, META_HPP_FWD(instance)};
    const uarg_base vvalue{registry, META_HPP_FWD(value)};
    return state_->setter_error(vinst, vvalue);
}
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <method_pointer_kind Method>
method_type_data::method_type_data(type_list<Method>)
    : type_data_base{type_kind::method_},
      flags{method_traits<Method>::make_flags()},
      owner_type{resolve_type<typename method_traits<Method>::class_type>()},
      return_type{resolve_type<typename method_traits<Method>::return_type>()},
      argument_types{resolve_types(typename method_traits<Method>::argument_types{})} {}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline method_bitflags method_type::get_flags() const noexcept { return data_->flags; }

inline std::size_t method_type::get_arity() const noexcept { return data_->argument_types.size(); }

inline class_type method_type::get_owner_type() const noexcept { return data_->owner_type; }

inline any_type method_type::get_return_type() const noexcept { return data_->return_type; }

inline any_type method_type::get_argument_type(std::size_t position) const noexcept { return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{}; }

inline const any_type_list& method_type::get_argument_types() const noexcept { return data_->argument_types; }
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <method_policy_family Policy, method_pointer_kind Method>
uvalue raw_method_invoke(type_registry& registry, Method method_ptr, const uinst& inst, std::span<const uarg> args) {
    using mt = method_traits<Method>;
    using return_type = typename mt::return_type;
    using qualified_type = typename mt::qualified_type;
    using argument_types = typename mt::argument_types;

    constexpr bool as_copy                               //
            = std::is_copy_constructible_v<return_type>  //
              && std::is_same_v<Policy, method_policy::as_copy_t>;

    constexpr bool as_void                 //
            = std::is_void_v<return_type>  //
              || std::is_same_v<Policy, method_policy::discard_return_t>;

    constexpr bool ref_as_ptr                   //
            = std::is_reference_v<return_type>  //
              && std::is_same_v<Policy, method_policy::return_reference_as_pointer_t>;

    static_assert(as_copy || as_void || ref_as_ptr);

    META_HPP_ASSERT(                  //
            args.size() == mt::arity  //
            && "an attempt to call a method with an incorrect arity");

    META_HPP_ASSERT(                                    //
            inst.can_cast_to<qualified_type>(registry)  //
            && "an attempt to call a method with an incorrect instance type");

    META_HPP_ASSERT(                                            //
            can_cast_all_uargs<argument_types>(registry, args)  //
            && "an attempt to call a method with incorrect argument types");

    return unchecked_call_with_uargs<argument_types>(registry, args, [method_ptr, &inst, &registry](auto&&... all_args) {
        if constexpr (std::is_void_v<return_type>) {
            (inst.cast<qualified_type>(registry).*method_ptr)(META_HPP_FWD(all_args)...);
            return uvalue{};
        }

        if constexpr (std::is_same_v<Policy, method_policy::discard_return_t>) {
            std::ignore = (inst.cast<qualified_type>().*method_ptr)(META_HPP_FWD(all_args)...);
            return uvalue{};
        }

        if constexpr (!std::is_void_v<return_type>) {
            return_type&& result = (inst.cast<qualified_type>(registry).*method_ptr)(META_HPP_FWD(all_args)...);
            return ref_as_ptr ? uvalue{std::addressof(result)} : uvalue{META_HPP_FWD(result)};
        }
    });
}

template <method_pointer_kind Method>
uerror raw_method_invoke_error(type_registry& registry, const uinst_base& inst, std::span<const uarg_base> args) noexcept {
    using mt = method_traits<Method>;
    using qualified_type = typename mt::qualified_type;
    using argument_types = typename mt::argument_types;

    if (args.size() != mt::arity) {
        return uerror{error_code::arity_mismatch};
    }

    if (!inst.can_cast_to<qualified_type>(registry)) {
        return uerror{error_code::instance_type_mismatch};
    }

    if (!can_cast_all_uargs<argument_types>(registry, args)) {
        return uerror{error_code::argument_type_mismatch};
    }

    return uerror{error_code::no_error};
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <method_policy_family Policy, method_pointer_kind Method>
method_state::invoke_impl make_method_invoke(type_registry& registry, Method method_ptr) {
    return [&registry, method_ptr](const uinst& inst, std::span<const uarg> args) { return raw_method_invoke<Policy>(registry, method_ptr, inst, args); };
}

template <method_pointer_kind Method>
method_state::invoke_error_impl make_method_invoke_error(type_registry& registry) {
    return [&registry](const uinst_base& inst, std::span<const uarg_base> args) { return raw_method_invoke_error<Method>(registry, inst, args); };
}

template <method_pointer_kind Method>
argument_list make_method_arguments() {
    using mt = method_traits<Method>;
    using mt_argument_types = typename mt::argument_types;

    return []<std::size_t... Is>(std::index_sequence<Is...>) {
        [[maybe_unused]] const auto make_argument = []<std::size_t I>(index_constant<I>) {
            using P = type_list_at_t<I, mt_argument_types>;
            return argument{argument_state::make<P>(I, metadata_map{})};
        };
        return argument_list{make_argument(index_constant<Is>{})...};
    }
    (std::make_index_sequence<mt::arity>());
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
inline method_state::method_state(method_index nindex, metadata_map nmetadata) : index{std::move(nindex)}, metadata{std::move(nmetadata)} {}

template <method_policy_family Policy, method_pointer_kind Method>
method_state_ptr method_state::make(std::string name, Method method_ptr, metadata_map metadata) {
    type_registry& registry{type_registry::instance()};

    method_state state{
            method_index{registry.resolve_type<Method>(), std::move(name)},
            std::move(metadata),
    };

    state.invoke = make_method_invoke<Policy>(registry, method_ptr);
    state.invoke_error = make_method_invoke_error<Method>(registry);
    state.arguments = make_method_arguments<Method>();

    return make_intrusive<method_state>(std::move(state));
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline method_type method::get_type() const noexcept { return state_->index.get_type(); }

inline const std::string& method::get_name() const noexcept { return state_->index.get_name(); }

template <typename Instance, typename... Args>
uvalue method::invoke(Instance&& instance, Args&&... args) const {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uinst vinst{registry, META_HPP_FWD(instance)};
    const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
    return state_->invoke(vinst, vargs);
}

template <typename Instance, typename... Args>
uresult method::try_invoke(Instance&& instance, Args&&... args) const {
    using namespace detail;
    type_registry& registry{type_registry::instance()};

    {
        // doesn't actually move an 'instance' and 'args', just checks conversion errors
        const uinst_base vinst{registry, META_HPP_FWD(instance)};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
        if (const uerror err = state_->invoke_error(vinst, vargs)) {
            return err;
        }
    }

    const uinst vinst{registry, META_HPP_FWD(instance)};
    const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
    return state_->invoke(vinst, vargs);
}

template <typename Instance, typename... Args>
uvalue method::operator()(Instance&& instance, Args&&... args) const {
    return invoke(META_HPP_FWD(instance), META_HPP_FWD(args)...);
}

template <typename Instance, typename... Args>
bool method::is_invocable_with() const noexcept {
    return !check_invocable_error<Instance, Args...>();
}

template <typename Instance, typename... Args>
bool method::is_invocable_with(Instance&& instance, Args&&... args) const noexcept {
    return !check_invocable_error(META_HPP_FWD(instance), META_HPP_FWD(args)...);
}

template <typename Instance, typename... Args>
uerror method::check_invocable_error() const noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uinst_base vinst{registry, type_list<Instance>{}};
    const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
    return state_->invoke_error(vinst, vargs);
}

template <typename Instance, typename... Args>
uerror method::check_invocable_error(Instance&& instance, Args&&... args) const noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uinst_base vinst{registry, META_HPP_FWD(instance)};
    const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
    return state_->invoke_error(vinst, vargs);
}

inline argument method::get_argument(std::size_t position) const noexcept { return position < state_->arguments.size() ? state_->arguments[position] : argument{}; }

inline const argument_list& method::get_arguments() const noexcept { return state_->arguments; }
}  // namespace ME::meta::r

namespace ME::meta::r {
template <typename... Args>
uvalue invoke(const function& function, Args&&... args) {
    return function.invoke(META_HPP_FWD(args)...);
}

template <typename... Args>
uresult try_invoke(const function& function, Args&&... args) {
    return function.try_invoke(META_HPP_FWD(args)...);
}

template <detail::function_pointer_kind Function, typename... Args>
uvalue invoke(Function function_ptr, Args&&... args) {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
    return raw_function_invoke<function_policy::as_copy_t>(registry, function_ptr, vargs);
}

template <detail::function_pointer_kind Function, typename... Args>
uresult try_invoke(Function function_ptr, Args&&... args) {
    using namespace detail;
    type_registry& registry{type_registry::instance()};

    {
        // doesn't actually move 'args', just checks conversion errors
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
        if (const uerror err = raw_function_invoke_error<Function>(registry, vargs)) {
            return err;
        }
    }

    const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
    return raw_function_invoke<function_policy::as_copy_t>(registry, function_ptr, vargs);
}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <typename Instance>
uvalue invoke(const member& member, Instance&& instance) {
    return member.get(META_HPP_FWD(instance));
}

template <typename Instance>
uresult try_invoke(const member& member, Instance&& instance) {
    return member.try_get(META_HPP_FWD(instance));
}

template <detail::member_pointer_kind Member, typename Instance>
uvalue invoke(Member member_ptr, Instance&& instance) {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uinst vinst{registry, META_HPP_FWD(instance)};
    return raw_member_getter<member_policy::as_copy_t>(registry, member_ptr, vinst);
}

template <detail::member_pointer_kind Member, typename Instance>
uresult try_invoke(Member member_ptr, Instance&& instance) {
    using namespace detail;
    type_registry& registry{type_registry::instance()};

    {
        // doesn't actually move an 'instance', just checks conversion errors
        const uinst_base vinst{registry, META_HPP_FWD(instance)};
        if (const uerror err = raw_member_getter_error<Member>(registry, vinst)) {
            return err;
        }
    }

    const uinst vinst{registry, META_HPP_FWD(instance)};
    return raw_member_getter<member_policy::as_copy_t>(registry, member_ptr, vinst);
}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <typename Instance, typename... Args>
uvalue invoke(const method& method, Instance&& instance, Args&&... args) {
    return method.invoke(META_HPP_FWD(instance), META_HPP_FWD(args)...);
}

template <typename Instance, typename... Args>
uresult try_invoke(const method& method, Instance&& instance, Args&&... args) {
    return method.try_invoke(META_HPP_FWD(instance), META_HPP_FWD(args)...);
}

template <detail::method_pointer_kind Method, typename Instance, typename... Args>
uvalue invoke(Method method_ptr, Instance&& instance, Args&&... args) {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uinst vinst{registry, META_HPP_FWD(instance)};
    const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
    return raw_method_invoke<method_policy::as_copy_t>(registry, method_ptr, vinst, vargs);
}

template <detail::method_pointer_kind Method, typename Instance, typename... Args>
uresult try_invoke(Method method_ptr, Instance&& instance, Args&&... args) {
    using namespace detail;
    type_registry& registry{type_registry::instance()};

    {
        // doesn't actually move an 'instance' and 'args', just checks conversion errors
        const uinst_base vinst{registry, META_HPP_FWD(instance)};
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
        if (const uerror err = raw_method_invoke_error<Method>(registry, vinst, vargs)) {
            return err;
        }
    }

    const uinst vinst{registry, META_HPP_FWD(instance)};
    const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
    return raw_method_invoke<method_policy::as_copy_t>(registry, method_ptr, vinst, vargs);
}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <typename... Args>
bool is_invocable_with(const function& function) noexcept {
    return !check_invocable_error<Args...>(function);
}

template <typename... Args>
bool is_invocable_with(const function& function, Args&&... args) noexcept {
    return !check_invocable_error(function, META_HPP_FWD(args)...);
}

template <typename... Args, detail::function_pointer_kind Function>
bool is_invocable_with(Function function_ptr) noexcept {
    return !check_invocable_error<Args...>(function_ptr);
}

template <typename... Args, detail::function_pointer_kind Function>
bool is_invocable_with(Function function_ptr, Args&&... args) noexcept {
    return !check_invocable_error(function_ptr, META_HPP_FWD(args)...);
}

template <typename... Args>
uerror check_invocable_error(const function& function) noexcept {
    return function.check_invocable_error<Args...>();
}

template <typename... Args>
uerror check_invocable_error(const function& function, Args&&... args) noexcept {
    return function.check_invocable_error(META_HPP_FWD(args)...);
}

template <typename... Args, detail::function_pointer_kind Function>
uerror check_invocable_error(Function) noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
    return raw_function_invoke_error<Function>(registry, vargs);
}

template <typename... Args, detail::function_pointer_kind Function>
uerror check_invocable_error(Function, Args&&... args) noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
    return raw_function_invoke_error<Function>(registry, vargs);
}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <typename Instance>
bool is_invocable_with(const member& member) noexcept {
    return !check_invocable_error<Instance>(member);
}

template <typename Instance>
bool is_invocable_with(const member& member, Instance&& instance) noexcept {
    return !check_invocable_error(member, META_HPP_FWD(instance));
}

template <typename Instance, detail::member_pointer_kind Member>
bool is_invocable_with(Member member_ptr) noexcept {
    return !check_invocable_error<Instance>(member_ptr);
}

template <typename Instance, detail::member_pointer_kind Member>
bool is_invocable_with(Member member_ptr, Instance&& instance) noexcept {
    return !check_invocable_error(member_ptr, META_HPP_FWD(instance));
}

template <typename Instance>
uerror check_invocable_error(const member& member) noexcept {
    return member.check_gettable_error<Instance>();
}

template <typename Instance>
uerror check_invocable_error(const member& member, Instance&& instance) noexcept {
    return member.check_gettable_error(META_HPP_FWD(instance));
}

template <typename Instance, detail::member_pointer_kind Member>
uerror check_invocable_error(Member) noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uinst_base vinst{registry, type_list<Instance>{}};
    return raw_member_getter_error<Member>(registry, vinst);
}

template <typename Instance, detail::member_pointer_kind Member>
uerror check_invocable_error(Member, Instance&& instance) noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uinst_base vinst{registry, META_HPP_FWD(instance)};
    return raw_member_getter_error<Member>(registry, vinst);
}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <typename Instance, typename... Args>
bool is_invocable_with(const method& method) noexcept {
    return !check_invocable_error<Instance, Args...>(method);
}

template <typename Instance, typename... Args>
bool is_invocable_with(const method& method, Instance&& instance, Args&&... args) noexcept {
    return !check_invocable_error(method, META_HPP_FWD(instance), META_HPP_FWD(args)...);
}

template <typename Instance, typename... Args, detail::method_pointer_kind Method>
bool is_invocable_with(Method method_ptr) noexcept {
    return !check_invocable_error<Instance, Args...>(method_ptr);
}

template <typename Instance, typename... Args, detail::method_pointer_kind Method>
bool is_invocable_with(Method method_ptr, Instance&& instance, Args&&... args) noexcept {
    return !check_invocable_error(method_ptr, META_HPP_FWD(instance), META_HPP_FWD(args)...);
}

template <typename Instance, typename... Args>
uerror check_invocable_error(const method& method) noexcept {
    return method.check_invocable_error<Instance, Args...>();
}

template <typename Instance, typename... Args>
uerror check_invocable_error(const method& method, Instance&& instance, Args&&... args) noexcept {
    return method.check_invocable_error(META_HPP_FWD(instance), META_HPP_FWD(args)...);
}

template <typename Instance, typename... Args, detail::method_pointer_kind Method>
uerror check_invocable_error(Method) noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uinst_base vinst{registry, type_list<Instance>{}};
    const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
    return raw_method_invoke_error<Method>(registry, vinst, vargs);
}

template <typename Instance, typename... Args, detail::method_pointer_kind Method>
uerror check_invocable_error(Method, Instance&& instance, Args&&... args) noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uinst_base vinst{registry, META_HPP_FWD(instance)};
    const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
    return raw_method_invoke_error<Method>(registry, vinst, vargs);
}
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
inline argument_state::argument_state(argument_index nindex, metadata_map nmetadata) : index{nindex}, metadata{std::move(nmetadata)} {}

template <typename Argument>
inline argument_state_ptr argument_state::make(std::size_t position, metadata_map metadata) {
    type_registry& registry{type_registry::instance()};

    argument_state state{
            argument_index{registry.resolve_type<Argument>(), position},
            std::move(metadata),
    };

    return make_intrusive<argument_state>(std::move(state));
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline any_type argument::get_type() const noexcept { return state_->index.get_type(); }

inline std::size_t argument::get_position() const noexcept { return state_->index.get_position(); }

inline const std::string& argument::get_name() const noexcept { return state_->name; }
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <class_kind Class, typename... Args>
constructor_type_data::constructor_type_data(type_list<Class>, type_list<Args...>)
    : type_data_base{type_kind::constructor_},
      flags{constructor_traits<Class, Args...>::make_flags()},
      owner_type{resolve_type<typename constructor_traits<Class, Args...>::class_type>()},
      argument_types{resolve_types(typename constructor_traits<Class, Args...>::argument_types{})} {}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline constructor_bitflags constructor_type::get_flags() const noexcept { return data_->flags; }

inline std::size_t constructor_type::get_arity() const noexcept { return data_->argument_types.size(); }

inline class_type constructor_type::get_owner_type() const noexcept { return data_->owner_type; }

inline any_type constructor_type::get_argument_type(std::size_t position) const noexcept { return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{}; }

inline const any_type_list& constructor_type::get_argument_types() const noexcept { return data_->argument_types; }
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <constructor_policy_family Policy, class_kind Class, typename... Args>
uvalue raw_constructor_create(type_registry& registry, std::span<const uarg> args) {
    using ct = constructor_traits<Class, Args...>;
    using class_type = typename ct::class_type;
    using argument_types = typename ct::argument_types;

    constexpr bool as_object                            //
            = std::is_copy_constructible_v<class_type>  //
              && std::is_same_v<Policy, constructor_policy::as_object_t>;

    constexpr bool as_raw_ptr  //
            = std::is_same_v<Policy, constructor_policy::as_raw_pointer_t>;

    constexpr bool as_shared_ptr  //
            = std::is_same_v<Policy, constructor_policy::as_shared_pointer_t>;

    static_assert(as_object || as_raw_ptr || as_shared_ptr);

    META_HPP_ASSERT(                  //
            args.size() == ct::arity  //
            && "an attempt to call a constructor with an incorrect arity");

    META_HPP_ASSERT(                                            //
            can_cast_all_uargs<argument_types>(registry, args)  //
            && "an attempt to call a constructor with incorrect argument types");

    return unchecked_call_with_uargs<argument_types>(registry, args, [](auto&&... all_args) -> uvalue {
        if constexpr (as_object) {
            return make_uvalue<class_type>(META_HPP_FWD(all_args)...);
        }

        if constexpr (as_raw_ptr) {
            return std::make_unique<class_type>(META_HPP_FWD(all_args)...).release();
        }

        if constexpr (as_shared_ptr) {
            return std::make_shared<class_type>(META_HPP_FWD(all_args)...);
        }
    });
}

template <class_kind Class, typename... Args>
uvalue raw_constructor_create_at(type_registry& registry, void* mem, std::span<const uarg> args) {
    using ct = constructor_traits<Class, Args...>;
    using class_type = typename ct::class_type;
    using argument_types = typename ct::argument_types;

    META_HPP_ASSERT(                  //
            args.size() == ct::arity  //
            && "an attempt to call a constructor with an incorrect arity");

    META_HPP_ASSERT(                                            //
            can_cast_all_uargs<argument_types>(registry, args)  //
            && "an attempt to call a constructor with incorrect argument types");

    return unchecked_call_with_uargs<argument_types>(registry, args, [mem](auto&&... all_args) { return std::construct_at(static_cast<class_type*>(mem), META_HPP_FWD(all_args)...); });
}

template <class_kind Class, typename... Args>
uerror raw_constructor_create_error(type_registry& registry, std::span<const uarg_base> args) noexcept {
    using ct = constructor_traits<Class, Args...>;
    using argument_types = typename ct::argument_types;

    if (args.size() != ct::arity) {
        return uerror{error_code::arity_mismatch};
    }

    if (!can_cast_all_uargs<argument_types>(registry, args)) {
        return uerror{error_code::argument_type_mismatch};
    }

    return uerror{error_code::no_error};
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <constructor_policy_family Policy, class_kind Class, typename... Args>
constructor_state::create_impl make_constructor_create(type_registry& registry) {
    return [&registry](std::span<const uarg> args) {  //
        return raw_constructor_create<Policy, Class, Args...>(registry, args);
    };
}

template <class_kind Class, typename... Args>
constructor_state::create_at_impl make_constructor_create_at(type_registry& registry) {
    return [&registry](void* mem, std::span<const uarg> args) {  //
        return raw_constructor_create_at<Class, Args...>(registry, mem, args);
    };
}

template <class_kind Class, typename... Args>
constructor_state::create_error_impl make_constructor_create_error(type_registry& registry) {
    return [&registry](std::span<const uarg_base> args) {  //
        return raw_constructor_create_error<Class, Args...>(registry, args);
    };
}

template <class_kind Class, typename... Args>
argument_list make_constructor_arguments() {
    using ct = constructor_traits<Class, Args...>;
    using ct_argument_types = typename ct::argument_types;

    return []<std::size_t... Is>(std::index_sequence<Is...>) {
        [[maybe_unused]] const auto make_argument = []<std::size_t I>(index_constant<I>) {
            using P = type_list_at_t<I, ct_argument_types>;
            return argument{argument_state::make<P>(I, metadata_map{})};
        };
        return argument_list{make_argument(index_constant<Is>{})...};
    }
    (std::make_index_sequence<ct::arity>());
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
inline constructor_state::constructor_state(constructor_index nindex, metadata_map nmetadata) : index{nindex}, metadata{std::move(nmetadata)} {}

template <constructor_policy_family Policy, class_kind Class, typename... Args>
constructor_state_ptr constructor_state::make(metadata_map metadata) {
    type_registry& registry{type_registry::instance()};

    constructor_state state{
            constructor_index{registry.resolve_constructor_type<Class, Args...>()},
            std::move(metadata),
    };

    state.create = make_constructor_create<Policy, Class, Args...>(registry);
    state.create_at = make_constructor_create_at<Class, Args...>(registry);
    state.create_error = make_constructor_create_error<Class, Args...>(registry);
    state.arguments = make_constructor_arguments<Class, Args...>();

    return make_intrusive<constructor_state>(std::move(state));
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline constructor_type constructor::get_type() const noexcept { return state_->index.get_type(); }

template <typename... Args>
uvalue constructor::create(Args&&... args) const {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
    return state_->create(vargs);
}

template <typename... Args>
uresult constructor::try_create(Args&&... args) const {
    using namespace detail;
    type_registry& registry{type_registry::instance()};

    {
        // doesn't actually move 'args', just checks conversion errors
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
        if (const uerror err = state_->create_error(vargs)) {
            return err;
        }
    }

    const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
    return state_->create(vargs);
}

template <typename... Args>
uvalue constructor::create_at(void* mem, Args&&... args) const {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
    return state_->create_at(mem, vargs);
}

template <typename... Args>
uresult constructor::try_create_at(void* mem, Args&&... args) const {
    using namespace detail;
    type_registry& registry{type_registry::instance()};

    {
        // doesn't actually move 'args', just checks conversion errors
        const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
        if (const uerror err = state_->create_error(vargs)) {
            return err;
        }
    }

    const std::array<uarg, sizeof...(Args)> vargs{uarg{registry, META_HPP_FWD(args)}...};
    return state_->create_at(mem, vargs);
}

template <typename... Args>
bool constructor::is_invocable_with() const noexcept {
    return !check_invocable_error<Args...>();
}

template <typename... Args>
bool constructor::is_invocable_with(Args&&... args) const noexcept {
    return !check_invocable_error(META_HPP_FWD(args)...);
}

template <typename... Args>
uerror constructor::check_invocable_error() const noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, type_list<Args>{}}...};
    return state_->create_error(vargs);
}

template <typename... Args>
uerror constructor::check_invocable_error(Args&&... args) const noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const std::array<uarg_base, sizeof...(Args)> vargs{uarg_base{registry, META_HPP_FWD(args)}...};
    return state_->create_error(vargs);
}

inline argument constructor::get_argument(std::size_t position) const noexcept { return position < state_->arguments.size() ? state_->arguments[position] : argument{}; }

inline const argument_list& constructor::get_arguments() const noexcept { return state_->arguments; }
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <class_kind Class>
destructor_type_data::destructor_type_data(type_list<Class>)
    : type_data_base{type_kind::destructor_}, flags{destructor_traits<Class>::make_flags()}, owner_type{resolve_type<typename destructor_traits<Class>::class_type>()} {}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline destructor_bitflags destructor_type::get_flags() const noexcept { return data_->flags; }

inline class_type destructor_type::get_owner_type() const noexcept { return data_->owner_type; }
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <class_kind Class>
void raw_destructor_destroy(type_registry& registry, const uarg& arg) {
    using dt = destructor_traits<Class>;
    using class_type = typename dt::class_type;

    META_HPP_ASSERT(                                //
            arg.can_cast_to<class_type*>(registry)  //
            && "an attempt to call a destructor with an incorrect argument type");

    std::unique_ptr<class_type>{arg.cast<class_type*>(registry)}.reset();
}

template <class_kind Class>
void raw_destructor_destroy_at(void* mem) {
    using dt = destructor_traits<Class>;
    using class_type = typename dt::class_type;

    std::destroy_at(static_cast<class_type*>(mem));
}

template <class_kind Class>
uerror raw_destructor_destroy_error(type_registry& registry, const uarg_base& arg) noexcept {
    using dt = destructor_traits<Class>;
    using class_type = typename dt::class_type;

    if (!arg.can_cast_to<class_type*>(registry)) {
        return uerror{error_code::argument_type_mismatch};
    }

    return uerror{error_code::no_error};
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <class_kind Class>
destructor_state::destroy_impl make_destructor_destroy(type_registry& registry) {
    return [&registry](const uarg& arg) {  //
        return raw_destructor_destroy<Class>(registry, arg);
    };
}

template <class_kind Class>
destructor_state::destroy_at_impl make_destructor_destroy_at() {
    return &raw_destructor_destroy_at<Class>;
}

template <class_kind Class>
destructor_state::destroy_error_impl make_destructor_destroy_error(type_registry& registry) {
    return [&registry](const uarg_base& arg) {  //
        return raw_destructor_destroy_error<Class>(registry, arg);
    };
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
inline destructor_state::destructor_state(destructor_index nindex, metadata_map nmetadata) : index{nindex}, metadata{std::move(nmetadata)} {}

template <class_kind Class>
destructor_state_ptr destructor_state::make(metadata_map metadata) {
    type_registry& registry{type_registry::instance()};

    destructor_state state{
            destructor_index{registry.resolve_destructor_type<Class>()},
            std::move(metadata),
    };

    state.destroy = make_destructor_destroy<Class>(registry);
    state.destroy_at = make_destructor_destroy_at<Class>();
    state.destroy_error = make_destructor_destroy_error<Class>(registry);

    return make_intrusive<destructor_state>(std::move(state));
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline destructor_type destructor::get_type() const noexcept { return state_->index.get_type(); }

template <typename Arg>
void destructor::destroy(Arg&& arg) const {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uarg varg{registry, META_HPP_FWD(arg)};
    return state_->destroy(varg);
}

template <typename Arg>
uresult destructor::try_destroy(Arg&& arg) const {
    using namespace detail;
    type_registry& registry{type_registry::instance()};

    {
        // doesn't actually move an 'arg', just checks conversion errors
        const uarg_base varg{registry, META_HPP_FWD(arg)};
        if (const uerror err = state_->destroy_error(varg)) {
            return err;
        }
    }

    const uarg varg{registry, META_HPP_FWD(arg)};
    state_->destroy(varg);
    return uerror{error_code::no_error};
}

inline void destructor::destroy_at(void* mem) const { state_->destroy_at(mem); }

inline uresult destructor::try_destroy_at(void* mem) const {
    state_->destroy_at(mem);
    return uerror{error_code::no_error};
}

template <typename Arg>
bool destructor::is_invocable_with() const noexcept {
    return !check_invocable_error<Arg>();
}

template <typename Arg>
bool destructor::is_invocable_with(Arg&& arg) const noexcept {
    return !check_invocable_error(META_HPP_FWD(arg));
}

template <typename Arg>
uerror destructor::check_invocable_error() const noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uarg_base varg{registry, type_list<Arg>{}};
    return state_->destroy_error(varg);
}

template <typename Arg>
uerror destructor::check_invocable_error(Arg&& arg) const noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uarg_base varg{registry, META_HPP_FWD(arg)};
    return state_->destroy_error(varg);
}
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <enum_kind Enum>
enum_type_data::enum_type_data(type_list<Enum>)
    : type_data_base{type_kind::enum_}, flags{enum_traits<Enum>::make_flags()}, underlying_type{resolve_type<typename enum_traits<Enum>::underlying_type>()} {}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline enum_bitflags enum_type::get_flags() const noexcept { return data_->flags; }

inline number_type enum_type::get_underlying_type() const noexcept { return data_->underlying_type; }

inline const evalue_list& enum_type::get_evalues() const noexcept { return data_->evalues; }

inline evalue enum_type::get_evalue(std::string_view name) const noexcept {
    for (const evalue& evalue : data_->evalues) {
        if (evalue.get_name() == name) {
            return evalue;
        }
    }
    return evalue{};
}

template <detail::enum_kind Enum>
std::string_view enum_type::value_to_name(Enum value) const noexcept {
    if (resolve_type<Enum>() != *this) {
        return std::string_view{};
    }

    for (const evalue& evalue : data_->evalues) {
        if (evalue.get_value().as<Enum>() == value) {
            return evalue.get_name();
        }
    }

    return std::string_view{};
}

inline uvalue enum_type::name_to_value(std::string_view name) const noexcept {
    if (const evalue& value = get_evalue(name)) {
        return value.get_value();
    }

    return uvalue{};
}
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
inline evalue_state::evalue_state(evalue_index nindex, metadata_map nmetadata) : index{std::move(nindex)}, metadata{std::move(nmetadata)} {}

template <enum_kind Enum>
evalue_state_ptr evalue_state::make(std::string name, Enum evalue, metadata_map metadata) {
    type_registry& registry{type_registry::instance()};

    evalue_state state{
            evalue_index{registry.resolve_type<Enum>(), std::move(name)},
            std::move(metadata),
    };

    state.enum_value = uvalue{evalue};
    state.underlying_value = uvalue{to_underlying(evalue)};

    return make_intrusive<evalue_state>(std::move(state));
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline enum_type evalue::get_type() const noexcept { return state_->index.get_type(); }

inline const std::string& evalue::get_name() const noexcept { return state_->index.get_name(); }

inline const uvalue& evalue::get_value() const noexcept { return state_->enum_value; }

inline const uvalue& evalue::get_underlying_value() const noexcept { return state_->underlying_value; }
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <pointer_kind Pointer>
pointer_type_data::pointer_type_data(type_list<Pointer>)
    : type_data_base{type_kind::pointer_}, flags{pointer_traits<Pointer>::make_flags()}, data_type{resolve_type<typename pointer_traits<Pointer>::data_type>()} {}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline pointer_bitflags pointer_type::get_flags() const noexcept { return data_->flags; }

inline any_type pointer_type::get_data_type() const noexcept { return data_->data_type; }
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <variable_policy_family Policy, pointer_kind Pointer>
uvalue raw_variable_getter(type_registry&, Pointer variable_ptr) {
    using pt = pointer_traits<Pointer>;
    using data_type = typename pt::data_type;

    constexpr bool as_copy                                            //
            = std::is_copy_constructible_v<data_type>                 //
              && std::is_same_v<Policy, variable_policy::as_copy_t>;  //

    constexpr bool as_ptr                                             //
            = std::is_same_v<Policy, variable_policy::as_pointer_t>;  //

    constexpr bool as_ref_wrap                                                  //
            = std::is_same_v<Policy, variable_policy::as_reference_wrapper_t>;  //

    static_assert(as_copy || as_ptr || as_ref_wrap);

    auto&& return_value = *variable_ptr;

    if constexpr (as_copy) {
        return uvalue{META_HPP_FWD(return_value)};
    }

    if constexpr (as_ptr) {
        return uvalue{std::addressof(return_value)};
    }

    if constexpr (as_ref_wrap) {
        return uvalue{std::ref(return_value)};
    }
}

template <pointer_kind Pointer>
void raw_variable_setter(type_registry& registry, Pointer variable_ptr, const uarg& arg) {
    using pt = pointer_traits<Pointer>;
    using data_type = typename pt::data_type;

    if constexpr (std::is_const_v<data_type>) {
        (void)registry;
        (void)variable_ptr;
        (void)arg;
        META_HPP_ASSERT(false && "an attempt to set a constant variable");
    } else {
        META_HPP_ASSERT(                              //
                arg.can_cast_to<data_type>(registry)  //
                && "an attempt to set a variable with an incorrect argument type");

        *variable_ptr = arg.cast<data_type>(registry);
    }
}

template <pointer_kind Pointer>
uerror raw_variable_setter_error(type_registry& registry, const uarg_base& arg) noexcept {
    using pt = pointer_traits<Pointer>;
    using data_type = typename pt::data_type;

    if constexpr (std::is_const_v<data_type>) {
        (void)registry;
        (void)arg;
        return uerror{error_code::bad_const_access};
    } else {
        if (!arg.can_cast_to<data_type>(registry)) {
            return uerror{error_code::argument_type_mismatch};
        }
        return uerror{error_code::no_error};
    }
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <variable_policy_family Policy, pointer_kind Pointer>
variable_state::getter_impl make_variable_getter(type_registry& registry, Pointer variable_ptr) {
    return [&registry, variable_ptr]() {  //
        return raw_variable_getter<Policy>(registry, variable_ptr);
    };
}

template <pointer_kind Pointer>
variable_state::setter_impl make_variable_setter(type_registry& registry, Pointer variable_ptr) {
    return [&registry, variable_ptr](const uarg& arg) {  //
        return raw_variable_setter(registry, variable_ptr, arg);
    };
}

template <pointer_kind Pointer>
variable_state::setter_error_impl make_variable_setter_error(type_registry& registry) {
    return [&registry](const uarg_base& arg) {  //
        return raw_variable_setter_error<Pointer>(registry, arg);
    };
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
inline variable_state::variable_state(variable_index nindex, metadata_map nmetadata) : index{std::move(nindex)}, metadata{std::move(nmetadata)} {}

template <variable_policy_family Policy, pointer_kind Pointer>
variable_state_ptr variable_state::make(std::string name, Pointer variable_ptr, metadata_map metadata) {
    type_registry& registry{type_registry::instance()};

    variable_state state{
            variable_index{registry.resolve_type<Pointer>(), std::move(name)},
            std::move(metadata),
    };

    state.getter = make_variable_getter<Policy>(registry, variable_ptr);
    state.setter = make_variable_setter(registry, variable_ptr);
    state.setter_error = make_variable_setter_error<Pointer>(registry);

    return make_intrusive<variable_state>(std::move(state));
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline pointer_type variable::get_type() const noexcept { return state_->index.get_type(); }

inline const std::string& variable::get_name() const noexcept { return state_->index.get_name(); }

inline uvalue variable::get() const { return state_->getter(); }

inline uresult variable::try_get() const { return state_->getter(); }

inline uvalue variable::operator()() const { return get(); }

template <typename Value>
void variable::set(Value&& value) const {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uarg vvalue{registry, META_HPP_FWD(value)};
    state_->setter(vvalue);
}

template <typename Value>
uresult variable::try_set(Value&& value) const {
    using namespace detail;
    type_registry& registry{type_registry::instance()};

    {
        // doesn't actually move a 'value', just checks conversion errors
        const uarg_base vvalue{registry, META_HPP_FWD(value)};
        if (const uerror err = state_->setter_error(vvalue)) {
            return err;
        }
    }

    const uarg vvalue{registry, META_HPP_FWD(value)};
    state_->setter(vvalue);
    return uerror{error_code::no_error};
}

template <typename Value>
void variable::operator()(Value&& value) const {
    set(META_HPP_FWD(value));
}

template <typename Value>
bool variable::is_settable_with() const noexcept {
    return !check_settable_error<Value>();
}

template <typename Value>
bool variable::is_settable_with(Value&& value) const noexcept {
    return !check_settable_error(META_HPP_FWD(value));
}

template <typename Value>
uerror variable::check_settable_error() const noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uarg_base vvalue{registry, type_list<Value>{}};
    return state_->setter_error(vvalue);
}

template <typename Value>
uerror variable::check_settable_error(Value&& value) const noexcept {
    using namespace detail;
    type_registry& registry{type_registry::instance()};
    const uarg_base vvalue{registry, META_HPP_FWD(value)};
    return state_->setter_error(vvalue);
}
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <class_kind Class>
class_type_data::class_type_data(type_list<Class>)
    : type_data_base{type_kind::class_},
      flags{class_traits<Class>::make_flags()},
      size{class_traits<Class>::size},
      align{class_traits<Class>::align},
      argument_types{resolve_types(typename class_traits<Class>::argument_types{})} {}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename Derived, typename Base>
    requires std::is_base_of_v<Base, Derived>
inline class_type_data::upcast_func_t::upcast_func_t(std::in_place_type_t<Derived>, std::in_place_type_t<Base>)
    : upcast{[](void* from) -> void* { return static_cast<Base*>(static_cast<Derived*>(from)); }}, target{resolve_type<Base>()}, is_virtual{is_virtual_base_of_v<Base, Derived>} {}

inline void* class_type_data::upcast_func_t::apply(void* ptr) const noexcept { return upcast(ptr); }

inline const void* class_type_data::upcast_func_t::apply(const void* ptr) const noexcept {
    // NOLINTNEXTLINE(*-const-cast)
    return apply(const_cast<void*>(ptr));
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
inline class_type_data::upcast_func_list_t::upcast_func_list_t(const upcast_func_t& _upcast) : upcasts{_upcast} {
    for (const upcast_func_t& upcast : upcasts) {
        if (upcast.is_virtual) {
            vbases.emplace(upcast.target);
        }
    }
}

inline class_type_data::upcast_func_list_t::upcast_func_list_t(upcasts_t _upcasts, vbases_t _vbases) : upcasts{std::move(_upcasts)}, vbases{std::move(_vbases)} {}

inline void* class_type_data::upcast_func_list_t::apply(void* ptr) const noexcept {
    for (const upcast_func_t& upcast : upcasts) {
        ptr = upcast.apply(ptr);
    }
    return ptr;
}

inline const void* class_type_data::upcast_func_list_t::apply(const void* ptr) const noexcept {
    // NOLINTNEXTLINE(*-const-cast)
    return apply(const_cast<void*>(ptr));
}

inline class_type_data::upcast_func_list_t operator+(  //
        const class_type_data::upcast_func_list_t& l, const class_type_data::upcast_func_list_t& r) {
    class_type_data::upcast_func_list_t::upcasts_t new_upcasts;
    new_upcasts.reserve(l.upcasts.size() + r.upcasts.size());
    new_upcasts.insert(new_upcasts.end(), l.upcasts.begin(), l.upcasts.end());
    new_upcasts.insert(new_upcasts.end(), r.upcasts.begin(), r.upcasts.end());

    class_type_data::upcast_func_list_t::vbases_t new_vbases;
    new_vbases.insert(l.vbases.begin(), l.vbases.end());
    new_vbases.insert(r.vbases.begin(), r.vbases.end());

    return class_type_data::upcast_func_list_t{std::move(new_upcasts), std::move(new_vbases)};
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline class_bitflags class_type::get_flags() const noexcept { return data_->flags; }

inline std::size_t class_type::get_size() const noexcept { return data_->size; }

inline std::size_t class_type::get_align() const noexcept { return data_->align; }

inline std::size_t class_type::get_arity() const noexcept { return data_->argument_types.size(); }

inline any_type class_type::get_argument_type(std::size_t position) const noexcept { return position < data_->argument_types.size() ? data_->argument_types[position] : any_type{}; }

inline const any_type_list& class_type::get_argument_types() const noexcept { return data_->argument_types; }

inline const class_list& class_type::get_base_classes() const noexcept { return data_->base_classes; }

inline const class_list& class_type::get_derived_classes() const noexcept { return data_->derived_classes; }

inline const constructor_list& class_type::get_constructors() const noexcept { return data_->constructors; }

inline const destructor_list& class_type::get_destructors() const noexcept { return data_->destructors; }

inline const function_list& class_type::get_functions() const noexcept { return data_->functions; }

inline const member_list& class_type::get_members() const noexcept { return data_->members; }

inline const method_list& class_type::get_methods() const noexcept { return data_->methods; }

inline const typedef_map& class_type::get_typedefs() const noexcept { return data_->typedefs; }

inline const variable_list& class_type::get_variables() const noexcept { return data_->variables; }

template <typename... Args>
uvalue class_type::create(Args&&... args) const {
    for (const constructor& ctor : data_->constructors) {
        if (ctor.is_invocable_with(META_HPP_FWD(args)...)) {
            // there is no 'use after move' here because
            // 'is_invocable_with' doesn't actually move 'args'
            return ctor.create(META_HPP_FWD(args)...);
        }
    }
    return uvalue{};
}

template <typename... Args>
uvalue class_type::create_at(void* mem, Args&&... args) const {
    for (const constructor& ctor : data_->constructors) {
        if (ctor.is_invocable_with(META_HPP_FWD(args)...)) {
            // there is no 'use after move' here because
            // 'is_invocable_with' doesn't actually move 'args'
            return ctor.create_at(mem, META_HPP_FWD(args)...);
        }
    }
    return uvalue{};
}

template <typename Arg>
bool class_type::destroy(Arg&& arg) const {
    if (const destructor& dtor = get_destructor()) {
        if (dtor.is_invocable_with(META_HPP_FWD(arg))) {
            // there is no 'use after move' here because
            // 'is_invocable_with' doesn't actually move an 'arg'
            dtor.destroy(META_HPP_FWD(arg));
            return true;
        }
    }
    return false;
}

inline bool class_type::destroy_at(void* mem) const {
    if (const destructor& dtor = get_destructor()) {
        dtor.destroy_at(mem);
        return true;
    }
    return false;
}

template <detail::class_kind Derived>
bool class_type::is_base_of() const noexcept {
    return is_base_of(resolve_type<Derived>());
}

inline bool class_type::is_base_of(const class_type& derived) const noexcept { return is_valid() && derived.is_valid() && derived.data_->deep_upcasts.contains(*this); }

template <detail::class_kind Derived>
bool class_type::is_direct_base_of() const noexcept {
    return is_direct_base_of(resolve_type<Derived>());
}

inline bool class_type::is_direct_base_of(const class_type& derived) const noexcept { return is_valid() && derived.is_valid() && derived.data_->base_upcasts.contains(*this); }

template <detail::class_kind Derived>
bool class_type::is_virtual_base_of() const noexcept {
    return is_virtual_base_of(resolve_type<Derived>());
}

inline bool class_type::is_virtual_base_of(const class_type& derived) const noexcept {
    if (!is_valid() || !derived.is_valid()) {
        return false;
    }

    using deep_upcasts_t = detail::class_type_data::deep_upcasts_t;
    const deep_upcasts_t& deep_upcasts = derived.data_->deep_upcasts;

    if (auto iter{deep_upcasts.find(*this)}; iter != deep_upcasts.end()) {
        return !iter->second.is_ambiguous && !iter->second.vbases.empty();
    }

    return false;
}

template <detail::class_kind Base>
bool class_type::is_derived_from() const noexcept {
    return is_derived_from(resolve_type<Base>());
}

inline bool class_type::is_derived_from(const class_type& base) const noexcept { return base.is_base_of(*this); }

template <detail::class_kind Base>
bool class_type::is_direct_derived_from() const noexcept {
    return is_direct_derived_from(resolve_type<Base>());
}

inline bool class_type::is_direct_derived_from(const class_type& base) const noexcept { return base.is_direct_base_of(*this); }

template <detail::class_kind Base>
bool class_type::is_virtual_derived_from() const noexcept {
    return is_virtual_derived_from(resolve_type<Base>());
}

inline bool class_type::is_virtual_derived_from(const class_type& base) const noexcept { return base.is_virtual_base_of(*this); }

inline function class_type::get_function(std::string_view name, bool recursively) const noexcept {
    for (const function& function : data_->functions) {
        if (function.get_name() == name) {
            return function;
        }
    }

    if (recursively) {
        for (auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter) {
            if (const function& function = iter->get_function(name, recursively)) {
                return function;
            }
        }
    }

    return function{};
}

inline member class_type::get_member(std::string_view name, bool recursively) const noexcept {
    for (const member& member : data_->members) {
        if (member.get_name() == name) {
            return member;
        }
    }

    if (recursively) {
        for (auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter) {
            if (const member& member = iter->get_member(name, recursively)) {
                return member;
            }
        }
    }

    return member{};
}

inline method class_type::get_method(std::string_view name, bool recursively) const noexcept {
    for (const method& method : data_->methods) {
        if (method.get_name() == name) {
            return method;
        }
    }

    if (recursively) {
        for (auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter) {
            if (const method& method = iter->get_method(name, recursively)) {
                return method;
            }
        }
    }

    return method{};
}

inline any_type class_type::get_typedef(std::string_view name, bool recursively) const noexcept {
    if (auto iter{data_->typedefs.find(name)}; iter != data_->typedefs.end()) {
        return iter->second;
    }

    if (recursively) {
        for (auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter) {
            if (const any_type& type = iter->get_typedef(name, recursively)) {
                return type;
            }
        }
    }

    return any_type{};
}

inline variable class_type::get_variable(std::string_view name, bool recursively) const noexcept {
    for (const variable& variable : data_->variables) {
        if (variable.get_name() == name) {
            return variable;
        }
    }

    if (recursively) {
        for (auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter) {
            if (const variable& variable = iter->get_variable(name, recursively)) {
                return variable;
            }
        }
    }

    return variable{};
}

//
// get_constructor_with
//

template <typename... Args>
constructor class_type::get_constructor_with() const noexcept {
    detail::type_registry& registry{detail::type_registry::instance()};
    return get_constructor_with({registry.resolve_type<Args>()...});
}

template <typename Iter>
constructor class_type::get_constructor_with(Iter first, Iter last) const noexcept {
    for (const constructor& ctor : data_->constructors) {
        const any_type_list& args = ctor.get_type().get_argument_types();
        if (std::equal(first, last, args.begin(), args.end())) {
            return ctor;
        }
    }
    return constructor{};
}

inline constructor class_type::get_constructor_with(std::span<const any_type> args) const noexcept { return get_constructor_with(args.begin(), args.end()); }

inline constructor class_type::get_constructor_with(std::initializer_list<any_type> args) const noexcept { return get_constructor_with(args.begin(), args.end()); }

//
// get_destructor
//

inline destructor class_type::get_destructor() const noexcept {
    if (data_->destructors.empty()) {
        return destructor{};
    }
    return *data_->destructors.begin();
}

//
// get_function_with
//

template <typename... Args>
function class_type::get_function_with(  //
        std::string_view name, bool recursively) const noexcept {
    detail::type_registry& registry{detail::type_registry::instance()};
    return get_function_with(name, {registry.resolve_type<Args>()...}, recursively);
}

template <typename Iter>
function class_type::get_function_with(  //
        std::string_view name, Iter first, Iter last, bool recursively) const noexcept {
    for (const function& function : data_->functions) {
        if (function.get_name() != name) {
            continue;
        }

        const any_type_list& args = function.get_type().get_argument_types();
        if (std::equal(first, last, args.begin(), args.end())) {
            return function;
        }
    }

    if (recursively) {
        for (auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter) {
            if (const function& function = iter->get_function_with(name, first, last, recursively)) {
                return function;
            }
        }
    }

    return function{};
}

inline function class_type::get_function_with(  //
        std::string_view name, std::span<const any_type> args, bool recursively) const noexcept {
    return get_function_with(name, args.begin(), args.end(), recursively);
}

inline function class_type::get_function_with(  //
        std::string_view name, std::initializer_list<any_type> args, bool recursively) const noexcept {
    return get_function_with(name, args.begin(), args.end(), recursively);
}

//
// get_method_with
//

template <typename... Args>
method class_type::get_method_with(  //
        std::string_view name, bool recursively) const noexcept {
    detail::type_registry& registry{detail::type_registry::instance()};
    return get_method_with(name, {registry.resolve_type<Args>()...}, recursively);
}

template <typename Iter>
method class_type::get_method_with(  //
        std::string_view name, Iter first, Iter last, bool recursively) const noexcept {
    for (const method& method : data_->methods) {
        if (method.get_name() != name) {
            continue;
        }

        const any_type_list& args = method.get_type().get_argument_types();
        if (std::equal(first, last, args.begin(), args.end())) {
            return method;
        }
    }

    if (recursively) {
        for (auto iter{data_->base_classes.rbegin()}, end{data_->base_classes.rend()}; iter != end; ++iter) {
            if (const method& method = iter->get_method_with(name, first, last, recursively)) {
                return method;
            }
        }
    }

    return method{};
}

inline method class_type::get_method_with(  //
        std::string_view name, std::span<const any_type> args, bool recursively) const noexcept {
    return get_method_with(name, args.begin(), args.end(), recursively);
}

inline method class_type::get_method_with(  //
        std::string_view name, std::initializer_list<any_type> args, bool recursively) const noexcept {
    return get_method_with(name, args.begin(), args.end(), recursively);
}
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
inline scope_state::scope_state(scope_index nindex, metadata_map nmetadata) : index{std::move(nindex)}, metadata{std::move(nmetadata)} {}

inline scope_state_ptr scope_state::make(std::string name, metadata_map metadata) {
    scope_state state{
            scope_index{std::move(name)},
            std::move(metadata),
    };
    return make_intrusive<scope_state>(std::move(state));
}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline const std::string& scope::get_name() const noexcept { return state_->index.get_name(); }

inline const function_list& scope::get_functions() const noexcept { return state_->functions; }

inline const typedef_map& scope::get_typedefs() const noexcept { return state_->typedefs; }

inline const variable_list& scope::get_variables() const noexcept { return state_->variables; }

inline function scope::get_function(std::string_view name) const noexcept {
    for (const function& function : state_->functions) {
        if (function.get_name() == name) {
            return function;
        }
    }
    return function{};
}

inline any_type scope::get_typedef(std::string_view name) const noexcept {
    if (auto iter{state_->typedefs.find(name)}; iter != state_->typedefs.end()) {
        return iter->second;
    }
    return any_type{};
}

inline variable scope::get_variable(std::string_view name) const noexcept {
    for (const variable& variable : state_->variables) {
        if (variable.get_name() == name) {
            return variable;
        }
    }
    return variable{};
}

template <typename... Args>
function scope::get_function_with(  //
        std::string_view name) const noexcept {
    detail::type_registry& registry{detail::type_registry::instance()};
    return get_function_with(name, {registry.resolve_type<Args>()...});
}

template <typename Iter>
function scope::get_function_with(  //
        std::string_view name, Iter first, Iter last) const noexcept {
    for (const function& function : state_->functions) {
        if (function.get_name() != name) {
            continue;
        }

        const any_type_list& args = function.get_type().get_argument_types();
        if (std::equal(first, last, args.begin(), args.end())) {
            return function;
        }
    }
    return function{};
}

inline function scope::get_function_with(  //
        std::string_view name, std::span<const any_type> args) const noexcept {
    return get_function_with(name, args.begin(), args.end());
}

inline function scope::get_function_with(  //
        std::string_view name, std::initializer_list<any_type> args) const noexcept {
    return get_function_with(name, args.begin(), args.end());
}
}  // namespace ME::meta::r

namespace ME::meta::r {
template <detail::type_family Type>
any_type::any_type(const Type& other) noexcept : any_type{detail::type_access(other)} {}

template <detail::type_family Type>
bool any_type::is() const noexcept {
    if constexpr (std::is_same_v<Type, any_type>) {
        return data_ != nullptr;
    } else {
        return is(detail::type_traits<Type>::kind);
    }
}

[[nodiscard]] inline bool any_type::is(type_kind kind) const noexcept { return data_ != nullptr && data_->kind == kind; }

template <detail::type_family Type>
Type any_type::as() const noexcept {
    if constexpr (std::is_same_v<Type, any_type>) {
        return *this;
    } else {
        using as_data_ptr = typename detail::type_traits<Type>::data_ptr;
        return is<Type>() ? Type{static_cast<as_data_ptr>(data_)} : Type{};
    }
}

inline bool any_type::is_array() const noexcept { return is<array_type>(); }

inline bool any_type::is_class() const noexcept { return is<class_type>(); }

inline bool any_type::is_constructor() const noexcept { return is<constructor_type>(); }

inline bool any_type::is_destructor() const noexcept { return is<destructor_type>(); }

inline bool any_type::is_enum() const noexcept { return is<enum_type>(); }

inline bool any_type::is_function() const noexcept { return is<function_type>(); }

inline bool any_type::is_member() const noexcept { return is<member_type>(); }

inline bool any_type::is_method() const noexcept { return is<method_type>(); }

inline bool any_type::is_nullptr() const noexcept { return is<nullptr_type>(); }

inline bool any_type::is_number() const noexcept { return is<number_type>(); }

inline bool any_type::is_pointer() const noexcept { return is<pointer_type>(); }

inline bool any_type::is_reference() const noexcept { return is<reference_type>(); }

inline bool any_type::is_void() const noexcept { return is<void_type>(); }

inline array_type any_type::as_array() const noexcept { return as<array_type>(); }

inline class_type any_type::as_class() const noexcept { return as<class_type>(); }

inline constructor_type any_type::as_constructor() const noexcept { return as<constructor_type>(); }

inline destructor_type any_type::as_destructor() const noexcept { return as<destructor_type>(); }

inline enum_type any_type::as_enum() const noexcept { return as<enum_type>(); }

inline function_type any_type::as_function() const noexcept { return as<function_type>(); }

inline member_type any_type::as_member() const noexcept { return as<member_type>(); }

inline method_type any_type::as_method() const noexcept { return as<method_type>(); }

inline nullptr_type any_type::as_nullptr() const noexcept { return as<nullptr_type>(); }

inline number_type any_type::as_number() const noexcept { return as<number_type>(); }

inline pointer_type any_type::as_pointer() const noexcept { return as<pointer_type>(); }

inline reference_type any_type::as_reference() const noexcept { return as<reference_type>(); }

inline void_type any_type::as_void() const noexcept { return as<void_type>(); }
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <array_kind Array>
array_type_data::array_type_data(type_list<Array>)
    : type_data_base{type_kind::array_}, flags{array_traits<Array>::make_flags()}, extent{array_traits<Array>::extent}, data_type{resolve_type<typename array_traits<Array>::data_type>()} {}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline array_bitflags array_type::get_flags() const noexcept { return data_->flags; }

inline std::size_t array_type::get_extent() const noexcept { return data_->extent; }

inline any_type array_type::get_data_type() const noexcept { return data_->data_type; }
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <nullptr_kind Nullptr>
nullptr_type_data::nullptr_type_data(type_list<Nullptr>) : type_data_base{type_kind::nullptr_} {}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <number_kind Number>
number_type_data::number_type_data(type_list<Number>)
    : type_data_base{type_kind::number_}, flags{number_traits<Number>::make_flags()}, size{number_traits<Number>::size}, align{number_traits<Number>::align} {}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline number_bitflags number_type::get_flags() const noexcept { return data_->flags; }

inline std::size_t number_type::get_size() const noexcept { return data_->size; }

inline std::size_t number_type::get_align() const noexcept { return data_->align; }
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <reference_kind Reference>
reference_type_data::reference_type_data(type_list<Reference>)
    : type_data_base{type_kind::reference_}, flags{reference_traits<Reference>::make_flags()}, data_type{resolve_type<typename reference_traits<Reference>::data_type>()} {}
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
inline reference_bitflags reference_type::get_flags() const noexcept { return data_->flags; }

inline any_type reference_type::get_data_type() const noexcept { return data_->data_type; }
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <void_kind Void>
void_type_data::void_type_data(type_list<Void>) : type_data_base{type_kind::void_} {}
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename To, typename From, typename ToDT = std::remove_pointer_t<To>,
          typename FromDT = std::remove_pointer_t<From>>
concept pointer_ucast_kind                                                               //
        = (std::is_pointer_v<From> && std::is_class_v<FromDT>)                           //
          && (std::is_pointer_v<To> && (std::is_class_v<ToDT> || std::is_void_v<ToDT>))  //
          && (!std::is_const_v<FromDT> || std::is_const_v<ToDT>)                         //
          && (!std::is_volatile_v<FromDT> || std::is_volatile_v<ToDT>);                  //

template <typename To, typename From, typename ToDT = std::remove_reference_t<To>,
          typename FromDT = std::remove_reference_t<From>>
concept lvalue_reference_ucast_kind                                      //
        = (std::is_lvalue_reference_v<From> && std::is_class_v<FromDT>)  //
          && (std::is_lvalue_reference_v<To> && std::is_class_v<ToDT>)   //
          && (!std::is_const_v<FromDT> || std::is_const_v<ToDT>)         //
          && (!std::is_volatile_v<FromDT> || std::is_volatile_v<ToDT>);  //
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
template <typename To, typename From>
    requires detail::pointer_ucast_kind<To, From>
To ucast(From from);

template <typename To, typename From>
    requires detail::lvalue_reference_ucast_kind<To, From>
To ucast(From&& from);
}  // namespace ME::meta::r

namespace ME::meta::r {
template <typename To, typename From>
    requires detail::pointer_ucast_kind<To, From>
To ucast(From from) {
    using from_data_type = std::remove_pointer_t<From>;
    using to_data_type = std::remove_pointer_t<To>;

    static_assert(detail::check_poly_info_enabled<from_data_type>, "The type doesn't support ucasts. Use the META_HPP_ENABLE_POLY_INFO macro to fix it.");

    if (from == nullptr) {
        return nullptr;
    }

    if constexpr (std::is_same_v<std::remove_cv_t<from_data_type>, std::remove_cv_t<to_data_type>>) {
        return from;
    } else {
        detail::type_registry& registry{detail::type_registry::instance()};
        const detail::poly_info& meta_info{from->get_most_derived_poly_info(registry)};

        // NOLINTNEXTLINE(*-const-cast)
        void* most_derived_object_ptr = const_cast<void*>(meta_info.ptr);

        if constexpr (std::is_void_v<to_data_type>) {
            return most_derived_object_ptr;
        } else {
            const class_type& to_class_type = registry.resolve_class_type<to_data_type>();
            return static_cast<To>(detail::pointer_upcast(most_derived_object_ptr, meta_info.type, to_class_type));
        }
    }
}

template <typename To, typename From>
    requires detail::lvalue_reference_ucast_kind<To, From>
To ucast(From&& from) {
    using from_data_type = std::remove_reference_t<From>;
    using to_data_type = std::remove_reference_t<To>;

    static_assert(detail::check_poly_info_enabled<from_data_type>, "The type doesn't support ucasts. Use the META_HPP_ENABLE_POLY_INFO macro to fix it.");

    if (to_data_type* ptr = ucast<to_data_type*>(std::addressof(from))) {
        return *ptr;
    }

    throw_exception(error_code::bad_cast);
}
}  // namespace ME::meta::r

namespace ME::meta::r::detail {
template <typename T>
struct deref_traits;

template <typename T>
concept has_deref_traits  //
        = requires(const T& v) {
              { deref_traits<T>{}(v) } -> std::convertible_to<uvalue>;
          };
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename T>
    requires std::is_copy_constructible_v<T>
struct deref_traits<T*> {
    uvalue operator()(T* v) const { return v != nullptr ? uvalue{*v} : uvalue{}; }
};

template <typename T>
    requires std::is_copy_constructible_v<T>
struct deref_traits<const T*> {
    uvalue operator()(const T* v) const { return v != nullptr ? uvalue{*v} : uvalue{}; }
};

template <typename T>
    requires std::is_copy_constructible_v<T>
struct deref_traits<std::shared_ptr<T>> {
    uvalue operator()(const std::shared_ptr<T>& v) const { return v != nullptr ? uvalue{*v} : uvalue{}; }
};

template <typename T>
    requires std::is_copy_constructible_v<T>
struct deref_traits<std::unique_ptr<T>> {
    uvalue operator()(const std::unique_ptr<T>& v) const { return v != nullptr ? uvalue{*v} : uvalue{}; }
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename T>
struct index_traits;

template <typename T>
concept has_index_traits  //
        = requires(const T& v, std::size_t i) {
              { index_traits<T>{}(v, i) } -> std::convertible_to<uvalue>;
          };
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename T>
    requires std::is_copy_constructible_v<T>
struct index_traits<T*> {
    uvalue operator()(T* v, std::size_t i) const {
        // NOLINTNEXTLINE(*-pointer-arithmetic)
        return v != nullptr ? uvalue{v[i]} : uvalue{};
    }
};

template <typename T>
    requires std::is_copy_constructible_v<T>
struct index_traits<const T*> {
    uvalue operator()(const T* v, std::size_t i) const {
        // NOLINTNEXTLINE(*-pointer-arithmetic)
        return v != nullptr ? uvalue{v[i]} : uvalue{};
    }
};

template <typename T, std::size_t Size>
    requires std::is_copy_constructible_v<T>
struct index_traits<std::array<T, Size>> {
    uvalue operator()(const std::array<T, Size>& v, std::size_t i) const { return i < v.size() ? uvalue{v[i]} : uvalue{}; }
};

template <typename T, typename Allocator>
    requires std::is_copy_constructible_v<T>
struct index_traits<std::deque<T, Allocator>> {
    uvalue operator()(const std::deque<T, Allocator>& v, std::size_t i) { return i < v.size() ? uvalue{v[i]} : uvalue{}; }
};

template <typename T, std::size_t Extent>
    requires std::is_copy_constructible_v<T>
struct index_traits<std::span<T, Extent>> {
    uvalue operator()(const std::span<T, Extent>& v, std::size_t i) const { return i < v.size() ? uvalue{v[i]} : uvalue{}; }
};

template <typename T, typename Traits, typename Allocator>
    requires std::is_copy_constructible_v<T>
struct index_traits<std::basic_string<T, Traits, Allocator>> {
    uvalue operator()(const std::basic_string<T, Traits, Allocator>& v, std::size_t i) const { return i < v.size() ? uvalue{v[i]} : uvalue{}; }
};

template <typename T, typename Allocator>
    requires std::is_copy_constructible_v<T>
struct index_traits<std::vector<T, Allocator>> {
    uvalue operator()(const std::vector<T, Allocator>& v, std::size_t i) { return i < v.size() ? uvalue{v[i]} : uvalue{}; }
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename T>
struct unmap_traits;

template <typename T>
concept has_unmap_traits  //
        = requires(const T& v) {
              { unmap_traits<T>{}(v) } -> std::convertible_to<uvalue>;
          };
}  // namespace ME::meta::r::detail

namespace ME::meta::r::detail {
template <typename T>
struct unmap_traits<std::shared_ptr<T>> {
    uvalue operator()(const std::shared_ptr<T>& v) const { return uvalue{v.get()}; }
};

template <typename T, typename D>
struct unmap_traits<std::unique_ptr<T, D>> {
    uvalue operator()(const std::unique_ptr<T, D>& v) const { return uvalue{v.get()}; }
};

template <typename T>
struct unmap_traits<std::reference_wrapper<T>> {
    uvalue operator()(const std::reference_wrapper<T>& v) const { return uvalue{std::addressof(v.get())}; }
};
}  // namespace ME::meta::r::detail

namespace ME::meta::r {
struct uvalue::vtable_t final {
    // NOLINTBEGIN(*-avoid-const-or-ref-data-members)
    const any_type type;

    void (*const move)(uvalue&& self, uvalue& to) noexcept;
    void (*const copy)(const uvalue& self, uvalue& to);
    void (*const reset)(uvalue& self) noexcept;

    uvalue (*const deref)(const storage_u& self);
    uvalue (*const index)(const storage_u& self, std::size_t i);
    uvalue (*const unmap)(const storage_u& self);
    // NOLINTEND(*-avoid-const-or-ref-data-members)

    template <typename T>
    inline static constexpr bool in_internal_v =  //
            (sizeof(T) <= sizeof(internal_storage_t)) && (alignof(internal_storage_t) % alignof(T) == 0) && std::is_nothrow_destructible_v<T> && std::is_nothrow_move_constructible_v<T>;

    template <typename T>
    inline static constexpr bool in_trivial_internal_v =  //
            in_internal_v<T> && std::is_trivially_copyable_v<T>;

    static std::pair<storage_e, const vtable_t*> unpack_vtag(const uvalue& self) noexcept {
        constexpr std::uintptr_t tag_mask{0b11};
        const std::uintptr_t vtag{self.storage_.vtag};
        return std::make_pair(static_cast<storage_e>(vtag & tag_mask),
                              // NOLINTNEXTLINE(*-no-int-to-ptr, *-reinterpret-cast)
                              reinterpret_cast<const vtable_t*>(vtag & ~tag_mask));
    }

    template <typename T>
    static T* storage_cast(storage_u& storage) noexcept {
        if constexpr (in_internal_v<T>) {
            // NOLINTNEXTLINE(*-union-access, *-reinterpret-cast)
            return std::launder(reinterpret_cast<T*>(storage.internal.data));
        } else {
            // NOLINTNEXTLINE(*-union-access)
            return static_cast<T*>(storage.external.ptr);
        }
    }

    template <typename T>
    static const T* storage_cast(const storage_u& storage) noexcept {
        if constexpr (in_internal_v<T>) {
            // NOLINTNEXTLINE(*-union-access, *-reinterpret-cast)
            return std::launder(reinterpret_cast<const T*>(storage.internal.data));
        } else {
            // NOLINTNEXTLINE(*-union-access)
            return static_cast<const T*>(storage.external.ptr);
        }
    }

    template <typename T, typename... Args, typename Tp = std::decay_t<T>>
    static Tp& do_ctor(uvalue& dst, Args&&... args) {
        META_HPP_DEV_ASSERT(!dst);

        if constexpr (in_internal_v<Tp>) {
            std::construct_at(storage_cast<Tp>(dst.storage_), std::forward<Args>(args)...);
            dst.storage_.vtag = in_trivial_internal_v<Tp> ? detail::to_underlying(storage_e::trivial) : detail::to_underlying(storage_e::internal);
        } else {
            // NOLINTNEXTLINE(*-union-access, *-owning-memory)
            dst.storage_.external.ptr = new Tp(std::forward<Args>(args)...);
            dst.storage_.vtag = detail::to_underlying(storage_e::external);
        }

        // NOLINTNEXTLINE(*-reinterpret-cast)
        dst.storage_.vtag |= reinterpret_cast<std::uintptr_t>(vtable_t::get<Tp>());
        return *storage_cast<Tp>(dst.storage_);
    }

    static void do_move(uvalue&& self, uvalue& to) noexcept {
        META_HPP_DEV_ASSERT(!to);

        auto&& [tag, vtable] = unpack_vtag(self);

        switch (tag) {
            case storage_e::nothing:
                break;
            case storage_e::trivial:
                to.storage_ = self.storage_;
                self.storage_.vtag = 0;
                break;
            case storage_e::internal:
            case storage_e::external:
                vtable->move(std::move(self), to);
                break;
        }
    }

    static void do_copy(const uvalue& self, uvalue& to) noexcept {
        META_HPP_DEV_ASSERT(!to);

        auto&& [tag, vtable] = unpack_vtag(self);

        switch (tag) {
            case storage_e::nothing:
                break;
            case storage_e::trivial:
                to.storage_ = self.storage_;
                break;
            case storage_e::internal:
            case storage_e::external:
                vtable->copy(self, to);
                break;
        }
    }

    static void do_reset(uvalue& self) noexcept {
        auto&& [tag, vtable] = unpack_vtag(self);

        switch (tag) {
            case storage_e::nothing:
                break;
            case storage_e::trivial:
                self.storage_.vtag = 0;
                break;
            case storage_e::internal:
            case storage_e::external:
                vtable->reset(self);
                break;
        }
    }

    static void do_swap(uvalue& l, uvalue& r) noexcept {
        if ((&l == &r) || (!l && !r)) {
            return;
        }

        if (l && r) {
            if (unpack_vtag(l).first == storage_e::external) {
                r = std::exchange(l, std::move(r));
            } else {
                l = std::exchange(r, std::move(l));
            }
        } else {
            if (l) {
                r = std::move(l);
            } else {
                l = std::move(r);
            }
        }
    }

    template <typename Tp>
    // NOLINTNEXTLINE(*-cognitive-complexity)
    static vtable_t* get() {
        static_assert(std::is_same_v<Tp, std::decay_t<Tp>>);

        static vtable_t table{
                .type = resolve_type<Tp>(),

                .move{[](uvalue&& self, uvalue& to) noexcept {
                    META_HPP_DEV_ASSERT(!to);
                    META_HPP_DEV_ASSERT(self);

                    Tp* src = storage_cast<Tp>(self.storage_);

                    if constexpr (in_internal_v<Tp>) {
                        do_ctor<Tp>(to, std::move(*src));
                        do_reset(self);
                    } else {
                        // NOLINTNEXTLINE(*-union-access)
                        to.storage_.external.ptr = src;
                        std::swap(to.storage_.vtag, self.storage_.vtag);
                    }
                }},

                .copy{[](const uvalue& self, uvalue& to) {
                    META_HPP_DEV_ASSERT(!to);
                    META_HPP_DEV_ASSERT(self);

                    const Tp* src = storage_cast<Tp>(self.storage_);

                    if constexpr (in_internal_v<Tp>) {
                        do_ctor<Tp>(to, *src);
                    } else {
                        // NOLINTNEXTLINE(*-union-access, *-owning-memory)
                        to.storage_.external.ptr = new Tp(*src);
                        to.storage_.vtag = self.storage_.vtag;
                    }
                }},

                .reset{[](uvalue& self) noexcept {
                    META_HPP_DEV_ASSERT(self);

                    Tp* src = storage_cast<Tp>(self.storage_);

                    if constexpr (in_internal_v<Tp>) {
                        std::destroy_at(src);
                    } else {
                        // NOLINTNEXTLINE(*-owning-memory)
                        delete src;
                    }

                    self.storage_.vtag = 0;
                }},

                .deref{[]() {
                    if constexpr (detail::has_deref_traits<Tp>) {
                        return +[](const storage_u& self) -> uvalue { return detail::deref_traits<Tp>{}(*storage_cast<Tp>(self)); };
                    } else {
                        return nullptr;
                    }
                }()},

                .index{[]() {
                    if constexpr (detail::has_index_traits<Tp>) {
                        return +[](const storage_u& self, std::size_t i) -> uvalue { return detail::index_traits<Tp>{}(*storage_cast<Tp>(self), i); };
                    } else {
                        return nullptr;
                    }
                }()},

                .unmap{[]() {
                    if constexpr (detail::has_unmap_traits<Tp>) {
                        return +[](const storage_u& self) -> uvalue { return detail::unmap_traits<Tp>{}(*storage_cast<Tp>(self)); };
                    } else {
                        return nullptr;
                    }
                }()},
        };

        return &table;
    }
};
}  // namespace ME::meta::r

namespace ME::meta::r {
inline uvalue::~uvalue() noexcept { reset(); }

inline uvalue::uvalue(uvalue&& other) noexcept { vtable_t::do_move(std::move(other), *this); }

inline uvalue::uvalue(const uvalue& other) { vtable_t::do_copy(other, *this); }

inline uvalue& uvalue::operator=(uvalue&& other) noexcept {
    if (this != &other) {
        vtable_t::do_reset(*this);
        vtable_t::do_move(std::move(other), *this);
    }
    return *this;
}

inline uvalue& uvalue::operator=(const uvalue& other) {
    if (this != &other) {
        vtable_t::do_reset(*this);
        vtable_t::do_copy(other, *this);
    }
    return *this;
}

template <typename T, typename Tp, typename>
uvalue::uvalue(T&& val) {
    vtable_t::do_ctor<T>(*this, std::forward<T>(val));
}

template <typename T, typename Tp, typename>
uvalue& uvalue::operator=(T&& val) {
    vtable_t::do_reset(*this);
    vtable_t::do_ctor<T>(*this, std::forward<T>(val));
    return *this;
}

template <typename T, typename... Args, typename Tp>
    requires std::is_copy_constructible_v<Tp>  //
             && std::is_constructible_v<Tp, Args...>
uvalue::uvalue(std::in_place_type_t<T>, Args&&... args) {
    vtable_t::do_ctor<T>(*this, std::forward<Args>(args)...);
}

template <typename T, typename U, typename... Args, typename Tp>
    requires std::is_copy_constructible_v<Tp>  //
             && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...>
uvalue::uvalue(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args) {
    vtable_t::do_ctor<T>(*this, ilist, std::forward<Args>(args)...);
}

template <typename T, typename... Args, typename Tp>
    requires std::is_copy_constructible_v<Tp>  //
             && std::is_constructible_v<Tp, Args...>
Tp& uvalue::emplace(Args&&... args) {
    vtable_t::do_reset(*this);
    return vtable_t::do_ctor<T>(*this, std::forward<Args>(args)...);
}

template <typename T, typename U, typename... Args, typename Tp>
    requires std::is_copy_constructible_v<Tp>  //
             && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...>
Tp& uvalue::emplace(std::initializer_list<U> ilist, Args&&... args) {
    vtable_t::do_reset(*this);
    return vtable_t::do_ctor<T>(*this, ilist, std::forward<Args>(args)...);
}

inline bool uvalue::has_value() const noexcept { return storage_.vtag != 0; }

inline uvalue::operator bool() const noexcept { return has_value(); }

inline void uvalue::reset() noexcept { vtable_t::do_reset(*this); }

inline void uvalue::swap(uvalue& other) noexcept { vtable_t::do_swap(*this, other); }

inline any_type uvalue::get_type() const noexcept {
    auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
    return tag == storage_e::nothing ? any_type{} : vtable->type;
}

inline void* uvalue::get_data() noexcept {
    switch (vtable_t::unpack_vtag(*this).first) {
        case storage_e::nothing:
            return nullptr;
        case storage_e::trivial:
        case storage_e::internal:
            // NOLINTNEXTLINE(*-union-access)
            return static_cast<void*>(storage_.internal.data);
        case storage_e::external:
            // NOLINTNEXTLINE(*-union-access)
            return storage_.external.ptr;
    }

    META_HPP_ASSERT(false);
    return nullptr;
}

inline const void* uvalue::get_data() const noexcept {
    switch (vtable_t::unpack_vtag(*this).first) {
        case storage_e::nothing:
            return nullptr;
        case storage_e::trivial:
        case storage_e::internal:
            // NOLINTNEXTLINE(*-union-access)
            return static_cast<const void*>(storage_.internal.data);
        case storage_e::external:
            // NOLINTNEXTLINE(*-union-access)
            return storage_.external.ptr;
    }

    META_HPP_ASSERT(false);
    return nullptr;
}

inline const void* uvalue::get_cdata() const noexcept {
    switch (vtable_t::unpack_vtag(*this).first) {
        case storage_e::nothing:
            return nullptr;
        case storage_e::trivial:
        case storage_e::internal:
            // NOLINTNEXTLINE(*-union-access)
            return static_cast<const void*>(storage_.internal.data);
        case storage_e::external:
            // NOLINTNEXTLINE(*-union-access)
            return storage_.external.ptr;
    }

    META_HPP_ASSERT(false);
    return nullptr;
}

inline uvalue uvalue::operator*() const {
    auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
    return tag != storage_e::nothing && vtable->deref != nullptr  //
                   ? vtable->deref(storage_)
                   : uvalue{};
}

inline bool uvalue::has_deref_op() const noexcept {
    auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
    return tag != storage_e::nothing && vtable->deref != nullptr;
}

inline uvalue uvalue::operator[](std::size_t index) const {
    auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
    return tag != storage_e::nothing && vtable->index != nullptr  //
                   ? vtable->index(storage_, index)
                   : uvalue{};
}

inline bool uvalue::has_index_op() const noexcept {
    auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
    return tag != storage_e::nothing && vtable->index != nullptr;
}

inline uvalue uvalue::unmap() const {
    auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
    return tag != storage_e::nothing && vtable->unmap != nullptr  //
                   ? vtable->unmap(storage_)
                   : uvalue{};
}

inline bool uvalue::has_unmap_op() const noexcept {
    auto&& [tag, vtable] = vtable_t::unpack_vtag(*this);
    return tag != storage_e::nothing && vtable->unmap != nullptr;
}

template <typename T>
bool uvalue::is() const noexcept {
    static_assert(std::is_same_v<T, std::decay_t<T>>);
    return detail::is_a(resolve_type<T>(), get_type());
}

template <detail::pointer_kind T>
T uvalue::as() {
    static_assert(std::is_same_v<T, std::decay_t<T>>);

    if (T ptr = try_as<T>(); ptr || get_type().is_nullptr()) {
        return ptr;
    }

    throw_exception(error_code::bad_uvalue_access);
}

template <detail::pointer_kind T>
T uvalue::as() const {
    static_assert(std::is_same_v<T, std::decay_t<T>>);

    if (T ptr = try_as<T>(); ptr || get_type().is_nullptr()) {
        return ptr;
    }

    throw_exception(error_code::bad_uvalue_access);
}

template <detail::non_pointer_kind T>
T uvalue::as() && {
    static_assert(std::is_same_v<T, std::decay_t<T>>);

    if (T* ptr = try_as<T>()) {
        return std::move(*ptr);
    }

    throw_exception(error_code::bad_uvalue_access);
}

template <detail::non_pointer_kind T>
T& uvalue::as() & {
    static_assert(std::is_same_v<T, std::decay_t<T>>);

    if (T* ptr = try_as<T>()) {
        return *ptr;
    }

    throw_exception(error_code::bad_uvalue_access);
}

template <detail::non_pointer_kind T>
const T& uvalue::as() const& {
    static_assert(std::is_same_v<T, std::decay_t<T>>);

    if (const T* ptr = try_as<T>()) {
        return *ptr;
    }

    throw_exception(error_code::bad_uvalue_access);
}

template <detail::non_pointer_kind T>
const T&& uvalue::as() const&& {
    static_assert(std::is_same_v<T, std::decay_t<T>>);

    if (const T* ptr = try_as<T>()) {
        return std::move(*ptr);
    }

    throw_exception(error_code::bad_uvalue_access);
}

template <detail::pointer_kind T>
T uvalue::try_as() noexcept {
    static_assert(std::is_same_v<T, std::decay_t<T>>);

    using namespace detail;
    type_registry& registry{type_registry::instance()};

    if (const uarg varg{registry, *this}; varg.can_cast_to<T>(registry)) {
        return varg.cast<T>(registry);
    }

    return nullptr;
}

template <detail::pointer_kind T>
T uvalue::try_as() const noexcept {
    static_assert(std::is_same_v<T, std::decay_t<T>>);

    using namespace detail;
    type_registry& registry{type_registry::instance()};

    if (const uarg varg{registry, *this}; varg.can_cast_to<T>(registry)) {
        return varg.cast<T>(registry);
    }

    return nullptr;
}

template <detail::non_pointer_kind T>
T* uvalue::try_as() noexcept {
    static_assert(std::is_same_v<T, std::decay_t<T>>);

    using namespace detail;
    type_registry& registry{type_registry::instance()};

    if (const uarg varg{registry, *this}; varg.can_cast_to<T&>(registry)) {
        return std::addressof(varg.cast<T&>(registry));
    }

    return nullptr;
}

template <detail::non_pointer_kind T>
const T* uvalue::try_as() const noexcept {
    static_assert(std::is_same_v<T, std::decay_t<T>>);

    using namespace detail;
    type_registry& registry{type_registry::instance()};

    if (const uarg varg{registry, *this}; varg.can_cast_to<const T&>(registry)) {
        return std::addressof(varg.cast<const T&>(registry));
    }

    return nullptr;
}
}  // namespace ME::meta::r

namespace ME::meta::r {
inline uerror::uerror(error_code error) noexcept : error_{error} {}

inline uerror& uerror::operator=(error_code error) noexcept {
    error_ = error;
    return *this;
}

inline bool uerror::has_error() const noexcept { return error_ != error_code::no_error; }

inline uerror::operator bool() const noexcept { return has_error(); }

inline error_code uerror::operator*() const noexcept { return error_; }

inline error_code uerror::get_error() const noexcept { return error_; }

inline void uerror::reset() noexcept { error_ = error_code::no_error; }

inline void uerror::swap(uerror& other) noexcept {
    using std::swap;
    swap(error_, other.error_);
}

inline std::size_t uerror::get_hash() const noexcept { return detail::hash_combiner{}(error_); }
}  // namespace ME::meta::r

namespace ME::meta::r {
inline uresult::uresult(uerror error) noexcept : error_{error.get_error()} {}

inline uresult::uresult(uvalue value) noexcept : value_{std::move(value)} {}

inline uresult& uresult::operator=(uerror error) noexcept {
    value_ = uvalue{};
    error_ = error.get_error();
    return *this;
}

inline uresult& uresult::operator=(uvalue value) noexcept {
    value_ = std::move(value);
    error_ = error_code::no_error;
    return *this;
}

template <typename T, typename Tp, typename>
uresult::uresult(T&& val) : value_{std::forward<T>(val)} {}

template <typename T, typename Tp, typename>
uresult& uresult::operator=(T&& val) {
    value_ = std::forward<T>(val);
    error_ = error_code::no_error;
    return *this;
}

template <typename T, typename... Args, typename Tp>
    requires std::is_copy_constructible_v<Tp>         //
             && std::is_constructible_v<Tp, Args...>  //
uresult::uresult(std::in_place_type_t<T>, Args&&... args) : value_{std::in_place_type<T>, std::forward<Args>(args)...} {}

template <typename T, typename U, typename... Args, typename Tp>
    requires std::is_copy_constructible_v<Tp>                                    //
             && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...>  //
uresult::uresult(std::in_place_type_t<T>, std::initializer_list<U> ilist, Args&&... args) : value_{std::in_place_type<T>, ilist, std::forward<Args>(args)...} {}

template <typename T, typename... Args, typename Tp>
    requires std::is_copy_constructible_v<Tp>         //
             && std::is_constructible_v<Tp, Args...>  //
Tp& uresult::emplace(Args&&... args) {
    Tp& val{value_.emplace<Tp>(std::forward<Args>(args)...)};
    error_ = error_code::no_error;
    return val;
}

template <typename T, typename U, typename... Args, typename Tp>
    requires std::is_copy_constructible_v<Tp>                                    //
             && std::is_constructible_v<Tp, std::initializer_list<U>&, Args...>  //
Tp& uresult::emplace(std::initializer_list<U> ilist, Args&&... args) {
    Tp& val{value_.emplace<Tp>(ilist, std::forward<Args>(args)...)};
    error_ = error_code::no_error;
    return val;
}

inline bool uresult::has_error() const noexcept { return error_ != error_code::no_error; }

inline bool uresult::has_value() const noexcept { return error_ == error_code::no_error; }

inline uresult::operator bool() const noexcept { return has_value(); }

inline uvalue* uresult::operator->() noexcept { return std::addressof(value_); }

inline const uvalue* uresult::operator->() const noexcept { return std::addressof(value_); }

inline uvalue& uresult::operator*() & noexcept { return value_; }

inline uvalue&& uresult::operator*() && noexcept { return std::move(value_); }

inline const uvalue& uresult::operator*() const& noexcept { return value_; }

inline const uvalue&& uresult::operator*() const&& noexcept {
    // NOLINTNEXTLINE(*-move-const-arg)
    return std::move(value_);
}

inline uvalue& uresult::get_value() & noexcept { return value_; }

inline uvalue&& uresult::get_value() && noexcept { return std::move(value_); }

inline const uvalue& uresult::get_value() const& noexcept { return value_; }

inline const uvalue&& uresult::get_value() const&& noexcept {
    // NOLINTNEXTLINE(*-move-const-arg)
    return std::move(value_);
}

inline error_code uresult::get_error() const noexcept { return error_; }

inline void uresult::reset() noexcept {
    value_ = uvalue{};
    error_ = error_code::no_error;
}

inline void uresult::swap(uresult& other) noexcept {
    using std::swap;
    swap(value_, other.value_);
    swap(error_, other.error_);
}
}  // namespace ME::meta::r
