/* vvvv generic header  vvvv */
#include <cassert>
#include <functional>
#include <stdexcept>
namespace folly {
template <typename T> constexpr T constexpr_max(T b...) { return b; }
template <typename T> constexpr T constexpr_min(T a, T b) {
  return b < a ? b : a;
}
constexpr auto kIsDebug = 1;
template <typename, typename Args> void throw_exception(Args);
template <class Iter> struct Range {
  constexpr Range(Iter start, size_t size) : b_(start), e_(start + size) {}
  template <class Container>
  constexpr Range(Container &container)
      : Range(container.data(), container.size()) {}
  constexpr Iter begin() const { return b_; }
  constexpr Iter end() const { return e_; }
  Iter b_;
  Iter e_;
};
typedef Range<const char *> StringPiece;
enum ordering { lt = -1, eq, gt };
} // namespace folly

#define TEST(x, y) void x ## y (void)
#define EXPECT_STREQ(x, y) 
#define EXPECT_THROW(x, y)
#define EXPECT_EQ(x, y)
#define EXPECT_FALSE(x)
#define EXPECT_TRUE(x)
/* ^^^^ generic header ^^^^ */


/* vvvv folly/CPortability.h vvvv */
/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* #pragma once */

/* These definitions are in a separate file so that they
 * may be included from C- as well as C++-based projects. */

/* #include <folly/portability/Config.h> */

/**
 * Portable version check.
 */
#ifndef __GNUC_PREREQ
#if defined __GNUC__ && defined __GNUC_MINOR__
/* nolint */
#define __GNUC_PREREQ(maj, min) \
  ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
/* nolint */
#define __GNUC_PREREQ(maj, min) 0
#endif
#endif

// portable version check for clang
#ifndef __CLANG_PREREQ
#if defined __clang__ && defined __clang_major__ && defined __clang_minor__
/* nolint */
#define __CLANG_PREREQ(maj, min) \
  ((__clang_major__ << 16) + __clang_minor__ >= ((maj) << 16) + (min))
#else
/* nolint */
#define __CLANG_PREREQ(maj, min) 0
#endif
#endif

#if defined(__has_builtin)
#define FOLLY_HAS_BUILTIN(...) __has_builtin(__VA_ARGS__)
#else
#define FOLLY_HAS_BUILTIN(...) 0
#endif

#if defined(__has_feature)
#define FOLLY_HAS_FEATURE(...) __has_feature(__VA_ARGS__)
#else
#define FOLLY_HAS_FEATURE(...) 0
#endif

/* FOLLY_SANITIZE_ADDRESS is defined to 1 if the current compilation unit
 * is being compiled with ASAN enabled.
 *
 * Beware when using this macro in a header file: this macro may change values
 * across compilation units if some libraries are built with ASAN enabled
 * and some built with ASAN disabled.  For instance, this may occur, if folly
 * itself was compiled without ASAN but a downstream project that uses folly is
 * compiling with ASAN enabled.
 *
 * Use FOLLY_LIBRARY_SANITIZE_ADDRESS (defined in folly-config.h) to check if
 * folly itself was compiled with ASAN enabled.
 */
#ifndef FOLLY_SANITIZE_ADDRESS
#if FOLLY_HAS_FEATURE(address_sanitizer) || __SANITIZE_ADDRESS__
#define FOLLY_SANITIZE_ADDRESS 1
#endif
#endif

/* Define attribute wrapper for function attribute used to disable
 * address sanitizer instrumentation. Unfortunately, this attribute
 * has issues when inlining is used, so disable that as well. */
#ifdef FOLLY_SANITIZE_ADDRESS
#if defined(__clang__)
#if __has_attribute(__no_sanitize__)
#define FOLLY_DISABLE_ADDRESS_SANITIZER \
  __attribute__((__no_sanitize__("address"), __noinline__))
#elif __has_attribute(__no_address_safety_analysis__)
#define FOLLY_DISABLE_ADDRESS_SANITIZER \
  __attribute__((__no_address_safety_analysis__, __noinline__))
#elif __has_attribute(__no_sanitize_address__)
#define FOLLY_DISABLE_ADDRESS_SANITIZER \
  __attribute__((__no_sanitize_address__, __noinline__))
#endif
#elif defined(__GNUC__)
#define FOLLY_DISABLE_ADDRESS_SANITIZER \
  __attribute__((__no_address_safety_analysis__, __noinline__))
#endif
#endif
#ifndef FOLLY_DISABLE_ADDRESS_SANITIZER
#define FOLLY_DISABLE_ADDRESS_SANITIZER
#endif

/* Define a convenience macro to test when thread sanitizer is being used
 * across the different compilers (e.g. clang, gcc) */
#ifndef FOLLY_SANITIZE_THREAD
#if FOLLY_HAS_FEATURE(thread_sanitizer) || __SANITIZE_THREAD__
#define FOLLY_SANITIZE_THREAD 1
#endif
#endif

#if FOLLY_SANITIZE_THREAD
#define FOLLY_DISABLE_THREAD_SANITIZER \
  __attribute__((no_sanitize_thread, noinline))
#else
#define FOLLY_DISABLE_THREAD_SANITIZER
#endif

/**
 * Define a convenience macro to test when memory sanitizer is being used
 * across the different compilers (e.g. clang, gcc)
 */
#ifndef FOLLY_SANITIZE_MEMORY
#if FOLLY_HAS_FEATURE(memory_sanitizer) || __SANITIZE_MEMORY__
#define FOLLY_SANITIZE_MEMORY 1
#endif
#endif

#if FOLLY_SANITIZE_MEMORY
#define FOLLY_DISABLE_MEMORY_SANITIZER \
  __attribute__((no_sanitize_memory, noinline))
#else
#define FOLLY_DISABLE_MEMORY_SANITIZER
#endif

/**
 * Define a convenience macro to test when ASAN, UBSAN, TSAN or MSAN sanitizer
 * are being used
 */
#ifndef FOLLY_SANITIZE
#if defined(FOLLY_SANITIZE_ADDRESS) || defined(FOLLY_SANITIZE_THREAD) || \
    defined(FOLLY_SANITIZE_MEMORY)
#define FOLLY_SANITIZE 1
#endif
#endif

#if FOLLY_SANITIZE
#define FOLLY_DISABLE_UNDEFINED_BEHAVIOR_SANITIZER(...) \
  __attribute__((no_sanitize(__VA_ARGS__)))
#else
#define FOLLY_DISABLE_UNDEFINED_BEHAVIOR_SANITIZER(...)
#endif // FOLLY_SANITIZE

#define FOLLY_DISABLE_SANITIZERS                                 \
  FOLLY_DISABLE_ADDRESS_SANITIZER FOLLY_DISABLE_THREAD_SANITIZER \
      FOLLY_DISABLE_UNDEFINED_BEHAVIOR_SANITIZER("undefined")

/**
 * Macro for marking functions as having public visibility.
 */
#if defined(__GNUC__)
#define FOLLY_EXPORT __attribute__((__visibility__("default")))
#else
#define FOLLY_EXPORT
#endif

// noinline
#ifdef _MSC_VER
#define FOLLY_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
#define FOLLY_NOINLINE __attribute__((__noinline__))
#else
#define FOLLY_NOINLINE
#endif

// always inline
#ifdef _MSC_VER
#define FOLLY_ALWAYS_INLINE __forceinline
#elif defined(__GNUC__)
#define FOLLY_ALWAYS_INLINE inline __attribute__((__always_inline__))
#else
#define FOLLY_ALWAYS_INLINE inline
#endif

// attribute hidden
#if defined(_MSC_VER)
#define FOLLY_ATTR_VISIBILITY_HIDDEN
#elif defined(__GNUC__)
#define FOLLY_ATTR_VISIBILITY_HIDDEN __attribute__((__visibility__("hidden")))
#else
#define FOLLY_ATTR_VISIBILITY_HIDDEN
#endif

// An attribute for marking symbols as weak, if supported
#if FOLLY_HAVE_WEAK_SYMBOLS
#define FOLLY_ATTR_WEAK __attribute__((__weak__))
#else
#define FOLLY_ATTR_WEAK
#endif

// Microsoft ABI version (can be overridden manually if necessary)
#ifndef FOLLY_MICROSOFT_ABI_VER
#ifdef _MSC_VER
#define FOLLY_MICROSOFT_ABI_VER _MSC_VER
#endif
#endif

//  FOLLY_ERASE
//
//  A conceptual attribute/syntax combo for erasing a function from the build
//  artifacts and forcing all call-sites to inline the callee, at least as far
//  as each compiler supports.
//
//  Semantically includes the inline specifier.
#define FOLLY_ERASE FOLLY_ALWAYS_INLINE FOLLY_ATTR_VISIBILITY_HIDDEN

//  FOLLY_ERASE_HACK_GCC
//
//  Equivalent to FOLLY_ERASE, but without hiding under gcc. Useful when applied
//  to a function which may sometimes be hidden separately, for example by being
//  declared in an anonymous namespace, since in such cases with -Wattributes
//  enabled, gcc would emit: 'visibility' attribute ignored.
//
//  Semantically includes the inline specifier.
#if defined(__GNUC__) && !defined(__clang__)
#define FOLLY_ERASE_HACK_GCC FOLLY_ALWAYS_INLINE
#else
#define FOLLY_ERASE_HACK_GCC FOLLY_ERASE
#endif

//  FOLLY_ERASE_TRYCATCH
//
//  Equivalent to FOLLY_ERASE, but for code which might contain explicit
//  exception handling. Has the effect of FOLLY_ERASE, except under MSVC which
//  warns about __forceinline when functions contain exception handling.
//
//  Semantically includes the inline specifier.
#ifdef _MSC_VER
#define FOLLY_ERASE_TRYCATCH inline
#else
#define FOLLY_ERASE_TRYCATCH FOLLY_ERASE
#endif
/* ^^^^ folly/CPortability.h ^^^^ */


/* vvvv folly/portability/Constexpr.h vvvv */
/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* #pragma once */

/* #include <folly/CPortability.h> */

/* #include <cstdint> */
/* #include <cstring> */

namespace folly {

namespace detail {

#if FOLLY_HAS_FEATURE(cxx_constexpr_string_builtins) || \
    FOLLY_HAS_BUILTIN(__builtin_strlen) || defined(_MSC_VER)
#define FOLLY_DETAIL_STRLEN __builtin_strlen
#else
#define FOLLY_DETAIL_STRLEN ::std::strlen
#endif

#if FOLLY_HAS_FEATURE(cxx_constexpr_string_builtins) || \
    FOLLY_HAS_BUILTIN(__builtin_strcmp)
#define FOLLY_DETAIL_STRCMP __builtin_strcmp
#else
#define FOLLY_DETAIL_STRCMP ::std::strcmp
#endif

// This overload is preferred if Char is char and if FOLLY_DETAIL_STRLEN
// yields a compile-time constant.
template <
    typename Char,
    std::size_t = FOLLY_DETAIL_STRLEN(static_cast<const Char*>(""))>
constexpr std::size_t constexpr_strlen_internal(const Char* s, int) noexcept {
  return FOLLY_DETAIL_STRLEN(s);
}
template <typename Char>
constexpr std::size_t constexpr_strlen_internal(
    const Char* s, unsigned) noexcept {
  std::size_t ret = 0;
  while (*s++) {
    ++ret;
  }
  return ret;
}

template <typename Char>
constexpr std::size_t constexpr_strlen_fallback(const Char* s) noexcept {
  return constexpr_strlen_internal(s, 0u);
}

static_assert(
    constexpr_strlen_fallback("123456789") == 9,
    "Someone appears to have broken constexpr_strlen...");

// This overload is preferred if Char is char and if FOLLY_DETAIL_STRCMP
// yields a compile-time constant.
template <
    typename Char,
    int = FOLLY_DETAIL_STRCMP(static_cast<const Char*>(""), "")>
constexpr int constexpr_strcmp_internal(
    const Char* s1, const Char* s2, int) noexcept {
  return FOLLY_DETAIL_STRCMP(s1, s2);
}
template <typename Char>
constexpr int constexpr_strcmp_internal(
    const Char* s1, const Char* s2, unsigned) noexcept {
  while (*s1 && *s1 == *s2) {
    ++s1, ++s2;
  }
  // NOTE: `int(*s1 - *s2)` may cause signed arithmetics overflow which is UB.
  return int(*s2 < *s1) - int(*s1 < *s2);
}

template <typename Char>
constexpr int constexpr_strcmp_fallback(
    const Char* s1, const Char* s2) noexcept {
  return constexpr_strcmp_internal(s1, s2, 0u);
}

#undef FOLLY_DETAIL_STRCMP
#undef FOLLY_DETAIL_STRLEN

} // namespace detail

template <typename Char>
constexpr std::size_t constexpr_strlen(const Char* s) noexcept {
  return detail::constexpr_strlen_internal(s, 0);
}

template <typename Char>
constexpr int constexpr_strcmp(const Char* s1, const Char* s2) noexcept {
  return detail::constexpr_strcmp_internal(s1, s2, 0);
}
} // namespace folly
/* ^^^^ folly/portability/Constexpr.h ^^^^ */


/* vvvv folly/FixedString.h vvvv */
/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// @author: Eric Niebler (eniebler)
// Fixed-size string type, for constexpr string handling.

/* #pragma once */

/* #include <cassert> */
/* #include <cstddef> */
/* #include <initializer_list> */
/* #include <iosfwd> */
/* #include <stdexcept> */
/* #include <string> */
/* #include <type_traits> */
/* #include <utility> */

/* #include <folly/ConstexprMath.h> */
/* #include <folly/Portability.h> */
/* #include <folly/Range.h> */
/* #include <folly/Utility.h> */
/* #include <folly/lang/Exception.h> */
/* #include <folly/lang/Ordering.h> */
/* #include <folly/portability/Constexpr.h> */

namespace folly {

template <class Char, std::size_t N>
class BasicFixedString;

template <std::size_t N>
using FixedString = BasicFixedString<char, N>;

namespace detail {
namespace fixedstring {

// This is a template so that the class static npos can be defined in the
// header.
template <class = void>
struct FixedStringBase_ {
  static constexpr std::size_t npos = static_cast<std::size_t>(-1);
};

template <class Void>
constexpr std::size_t FixedStringBase_<Void>::npos;

using FixedStringBase = FixedStringBase_<>;

// Intentionally NOT constexpr. By making this not constexpr, we make
// checkOverflow below ill-formed in a constexpr context when the condition
// it's testing for fails. In this way, precondition violations are reported
// at compile-time instead of at runtime.
[[noreturn]] inline void assertOutOfBounds() {
  assert(!"Array index out of bounds in BasicFixedString");
  throw_exception<std::out_of_range>(
      "Array index out of bounds in BasicFixedString");
}

constexpr std::size_t checkOverflow(std::size_t i, std::size_t max) {
  return i <= max ? i : (void(assertOutOfBounds()), max);
}

constexpr std::size_t checkOverflowOrNpos(std::size_t i, std::size_t max) {
  return i == FixedStringBase::npos
      ? max
      : (i <= max ? i : (void(assertOutOfBounds()), max));
}

constexpr std::size_t checkOverflowIfDebug(std::size_t i, std::size_t size) {
  return kIsDebug ? checkOverflow(i, size) : i;
}

// Intentionally NOT constexpr. See note above for assertOutOfBounds
[[noreturn]] inline void assertNotNullTerminated() noexcept {
  assert(!"Non-null terminated string used to initialize a BasicFixedString");
  std::terminate(); // Fail hard, fail fast.
}

// Parsing help for human readers: the following is a constexpr noexcept
// function that accepts a reference to an array as a parameter and returns
// a reference to the same array.
template <class Char, std::size_t N>
constexpr const Char (&checkNullTerminated(const Char (&a)[N]) noexcept)[N] {
  // Strange decltype(a)(a) used to make MSVC happy.
  return a[N - 1u] == Char(0)
          // In Debug mode, guard against embedded nulls:
          && (!kIsDebug || N - 1u == folly::constexpr_strlen(a))
      ? decltype(a)(a)
      : (assertNotNullTerminated(), decltype(a)(a));
}

template <class Left, class Right>
constexpr ordering compare_(
    const Left& left,
    std::size_t left_pos,
    std::size_t left_size,
    const Right& right,
    std::size_t right_pos,
    std::size_t right_size) noexcept {
  return left_pos == left_size
      ? (right_pos == right_size ? ordering::eq : ordering::lt)
      : (right_pos == right_size ? ordering::gt
                                 : (left[left_pos] < right[right_pos]
                                        ? ordering::lt
                                        : (left[left_pos] > right[right_pos]
                                               ? ordering::gt
                                               : fixedstring::compare_(
                                                     left,
                                                     left_pos + 1u,
                                                     left_size,
                                                     right,
                                                     right_pos + 1u,
                                                     right_size))));
}

template <class Left, class Right>
constexpr bool equal_(
    const Left& left,
    std::size_t left_size,
    const Right& right,
    std::size_t right_size) noexcept {
  return left_size == right_size &&
      ordering::eq == compare_(left, 0u, left_size, right, 0u, right_size);
}

template <class Char, class Left, class Right>
constexpr Char char_at_(
    const Left& left,
    std::size_t left_count,
    const Right& right,
    std::size_t right_count,
    std::size_t i) noexcept {
  return i < left_count                ? left[i]
      : i < (left_count + right_count) ? right[i - left_count]
                                       : Char(0);
}

template <class Char, class Left, class Right>
constexpr Char char_at_(
    const Left& left,
    std::size_t left_size,
    std::size_t left_pos,
    std::size_t left_count,
    const Right& right,
    std::size_t right_pos,
    std::size_t right_count,
    std::size_t i) noexcept {
  return i < left_pos
      ? left[i]
      : (i < right_count + left_pos ? right[i - left_pos + right_pos]
                                    : (i < left_size - left_count + right_count
                                           ? left[i - right_count + left_count]
                                           : Char(0)));
}

template <class Left, class Right>
constexpr bool find_at_(
    const Left& left,
    const Right& right,
    std::size_t pos,
    std::size_t count) noexcept {
  return 0u == count ||
      (left[pos + count - 1u] == right[count - 1u] &&
       find_at_(left, right, pos, count - 1u));
}

template <class Char, class Right>
constexpr bool find_one_of_at_(
    Char ch, const Right& right, std::size_t pos) noexcept {
  return 0u != pos &&
      (ch == right[pos - 1u] || find_one_of_at_(ch, right, pos - 1u));
}

template <class Left, class Right>
constexpr std::size_t find_(
    const Left& left,
    std::size_t left_size,
    const Right& right,
    std::size_t pos,
    std::size_t count) noexcept {
  return find_at_(left, right, pos, count) ? pos
      : left_size <= pos + count
      ? FixedStringBase::npos
      : find_(left, left_size, right, pos + 1u, count);
}

template <class Left, class Right>
constexpr std::size_t rfind_(
    const Left& left,
    const Right& right,
    std::size_t pos,
    std::size_t count) noexcept {
  return find_at_(left, right, pos, count) ? pos
      : 0u == pos                          ? FixedStringBase::npos
                  : rfind_(left, right, pos - 1u, count);
}

template <class Left, class Right>
constexpr std::size_t find_first_of_(
    const Left& left,
    std::size_t left_size,
    const Right& right,
    std::size_t pos,
    std::size_t count) noexcept {
  return find_one_of_at_(left[pos], right, count) ? pos
      : left_size <= pos + 1u
      ? FixedStringBase::npos
      : find_first_of_(left, left_size, right, pos + 1u, count);
}

template <class Left, class Right>
constexpr std::size_t find_first_not_of_(
    const Left& left,
    std::size_t left_size,
    const Right& right,
    std::size_t pos,
    std::size_t count) noexcept {
  return !find_one_of_at_(left[pos], right, count) ? pos
      : left_size <= pos + 1u
      ? FixedStringBase::npos
      : find_first_not_of_(left, left_size, right, pos + 1u, count);
}

template <class Left, class Right>
constexpr std::size_t find_last_of_(
    const Left& left,
    const Right& right,
    std::size_t pos,
    std::size_t count) noexcept {
  return find_one_of_at_(left[pos], right, count) ? pos
      : 0u == pos                                 ? FixedStringBase::npos
                  : find_last_of_(left, right, pos - 1u, count);
}

template <class Left, class Right>
constexpr std::size_t find_last_not_of_(
    const Left& left,
    const Right& right,
    std::size_t pos,
    std::size_t count) noexcept {
  return !find_one_of_at_(left[pos], right, count) ? pos
      : 0u == pos                                  ? FixedStringBase::npos
                  : find_last_not_of_(left, right, pos - 1u, count);
}

struct Helper {
  template <class Char, class Left, class Right, std::size_t... Is>
  static constexpr BasicFixedString<Char, sizeof...(Is)> concat_(
      const Left& left,
      std::size_t left_count,
      const Right& right,
      std::size_t right_count,
      std::index_sequence<Is...> is) noexcept {
    return {left, left_count, right, right_count, is};
  }

  template <class Char, class Left, class Right, std::size_t... Is>
  static constexpr BasicFixedString<Char, sizeof...(Is)> replace_(
      const Left& left,
      std::size_t left_size,
      std::size_t left_pos,
      std::size_t left_count,
      const Right& right,
      std::size_t right_pos,
      std::size_t right_count,
      std::index_sequence<Is...> is) noexcept {
    return {
        left,
        left_size,
        left_pos,
        left_count,
        right,
        right_pos,
        right_count,
        is};
  }

  template <class Char, std::size_t N>
  static constexpr const Char (
      &data_(const BasicFixedString<Char, N>& that) noexcept)[N + 1u] {
    return that.data_;
  }
};

template <class T>
constexpr void constexpr_swap(T& a, T& b) noexcept(
    noexcept(a = T(std::move(a)))) {
  T tmp((std::move(a)));
  a = std::move(b);
  b = std::move(tmp);
}

// For constexpr reverse iteration over a BasicFixedString
template <class T>
struct ReverseIterator {
 private:
  T* p_ = nullptr;
  struct dummy_ {
    T* p_ = nullptr;
  };
  using other = typename std::conditional<
      std::is_const<T>::value,
      ReverseIterator<typename std::remove_const<T>::type>,
      dummy_>::type;

 public:
  using value_type = typename std::remove_const<T>::type;
  using reference = T&;
  using pointer = T*;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::random_access_iterator_tag;

  constexpr ReverseIterator() = default;
  constexpr ReverseIterator(const ReverseIterator&) = default;
  constexpr ReverseIterator& operator=(const ReverseIterator&) = default;
  constexpr explicit ReverseIterator(T* p) noexcept : p_(p) {}
  constexpr /* implicit */ ReverseIterator(const other& that) noexcept
      : p_(that.p_) {}
  friend constexpr bool operator==(
      ReverseIterator a, ReverseIterator b) noexcept {
    return a.p_ == b.p_;
  }
  friend constexpr bool operator!=(
      ReverseIterator a, ReverseIterator b) noexcept {
    return !(a == b);
  }
  constexpr reference operator*() const { return *(p_ - 1); }
  constexpr ReverseIterator& operator++() noexcept {
    --p_;
    return *this;
  }
  constexpr ReverseIterator operator++(int) noexcept {
    auto tmp(*this);
    --p_;
    return tmp;
  }
  constexpr ReverseIterator& operator--() noexcept {
    ++p_;
    return *this;
  }
  constexpr ReverseIterator operator--(int) noexcept {
    auto tmp(*this);
    ++p_;
    return tmp;
  }
  constexpr ReverseIterator& operator+=(std::ptrdiff_t i) noexcept {
    p_ -= i;
    return *this;
  }
  friend constexpr ReverseIterator operator+(
      std::ptrdiff_t i, ReverseIterator that) noexcept {
    return ReverseIterator{that.p_ - i};
  }
  friend constexpr ReverseIterator operator+(
      ReverseIterator that, std::ptrdiff_t i) noexcept {
    return ReverseIterator{that.p_ - i};
  }
  constexpr ReverseIterator& operator-=(std::ptrdiff_t i) noexcept {
    p_ += i;
    return *this;
  }
  friend constexpr ReverseIterator operator-(
      ReverseIterator that, std::ptrdiff_t i) noexcept {
    return ReverseIterator{that.p_ + i};
  }
  friend constexpr std::ptrdiff_t operator-(
      ReverseIterator a, ReverseIterator b) noexcept {
    return b.p_ - a.p_;
  }
  constexpr reference operator[](std::ptrdiff_t i) const noexcept {
    return *(*this + i);
  }
};

} // namespace fixedstring
} // namespace detail

// Defined in folly/hash/Hash.h
std::uint32_t hsieh_hash32_buf(const void* buf, std::size_t len);

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** *
 * \class BasicFixedString
 *
 * \tparam Char The character type. Must be a scalar type.
 * \tparam N The capacity and max size of string instances of this type.
 *
 * \brief A class for holding up to `N` characters of type `Char` that is
 *        amenable to `constexpr` string manipulation. It is guaranteed to not
 *        perform any dynamic allocation.
 *
 * `BasicFixedString` is a `std::string` work-alike that stores characters in an
 * internal buffer. It has minor interface differences that make it easy to work
 * with strings in a `constexpr` context.
 *
 * \par Example:
 * \par
 * \code
 * constexpr auto hello = makeFixedString("hello");         // a FixedString<5>
 * constexpr auto world = makeFixedString("world");         // a FixedString<5>
 * constexpr auto hello_world = hello + ' ' + world + '!';  // a FixedString<12>
 * static_assert(hello_world == "hello world!", "neato!");
 * \endcode
 * \par
 * `FixedString<N>` is an alias for `BasicFixedString<char, N>`.
 *
 * \par Constexpr and In-place Mutation
 * \par
 * On a C++14 compiler, `BasicFixedString` supports the full `std::string`
 * interface as `constexpr` member functions. On a C++11 compiler, the mutating
 * members are not `constexpr`, but non-mutating alternatives, which create a
 * new string, can be used instead. For example, instead of this:
 * \par
 * \code
 * constexpr FixedString<10> replace_example_cpp14() {
 *   FixedString<10> test{"****"};
 *   test.replace(1, 2, "!!!!");
 *   return test; // returns "*!!!!*"
 * }
 * \endcode
 * \par
 * You might write this instead:
 * \par
 * \code
 * constexpr FixedString<10> replace_example_cpp11() {
 *   // GNU compilers have an extension that make it possible to create
 *   // FixedString objects with a `""_fs` user-defined literal.
 *   using namespace folly;
 *   return makeFixedString("****").creplace(1, 2, "!!!!"); // "*!!!!*"
 * }
 * \endcode
 *
 * \par User-defined Literals
 * Instead of using the `folly::makeFixedString` helper function, you can use
 * a user-defined literal to make `FixedString` instances. The UDL feature of
 * C++ has some limitations that make this less than ideal; you must tell the
 * compiler roughly how many characters are in the string. The suffixes `_fs4`,
 * `_fs8`, `_fs16`, `_fs32`, `_fs64`, and `_fs128` exist to create instances
 * of types `FixedString<4>`, `FixedString<8>`, etc. For example:
 * \par
 * \code
 * using namespace folly::string_literals;
 * constexpr auto hello = "hello"_fs8; // A FixedString<8> containing "hello"
 * \endcode
 * \par
 * See Error Handling below for what to expect when you try to exceed the
 * capacity of a `FixedString` by storing too many characters in it.
 * \par
 * If your compiler supports GNU extensions, there is one additional suffix you
 * can use: `_fs`. This suffix always creates `FixedString` objects of exactly
 * the right size. For example:
 * \par
 * \code
 * using namespace folly::string_literals;
 * // NOTE: Only works on compilers with GNU extensions enabled. Clang and
 * // gcc support this (-Wgnu-string-literal-operator-template):
 * constexpr auto hello = "hello"_fs; // A FixedString<5> containing "hello"
 * \endcode
 *
 * \par Error Handling:
 * The capacity of a `BasicFixedString` is set at compile time. When the user
 * asks the string to exceed its capacity, one of three things will happen,
 * depending on the context:
 *\par
 *  -# If the attempt is made while evaluating a constant expression, the
 *     program will fail to compile.
 *  -# Otherwise, if the program is being run in debug mode, it will `assert`.
 *  -# Otherwise, the failed operation will throw a `std::out_of_range`
 *     exception.
 *\par
 * This is also the case if an invalid offset is passed to any member function,
 * or if `pop_back` or `cpop_back` is called on an empty `BasicFixedString`.
 *
 * Member functions documented as having preconditions will assert in Debug
 * mode (`!defined(NDEBUG)`) on precondition failures. Those documented with
 * \b Throws clauses will throw the specified exception on failure. Those with
 * both a precondition and a \b Throws clause will assert in Debug and throw
 * in Release mode.
 */
template <class Char, std::size_t N>
class BasicFixedString : private detail::fixedstring::FixedStringBase {
 private:
  template <class, std::size_t>
  friend class BasicFixedString;
  friend struct detail::fixedstring::Helper;

  // FUTURE: use constexpr_log2 to fold instantiations of BasicFixedString
  // together. All BasicFixedString<C, N> instantiations could share the
  // implementation of BasicFixedString<C, M>, where M is the next highest power
  // of 2 after N.
  //
  // Also, because of alignment of the data_ and size_ members, N should never
  // be smaller than `(alignof(std::size_t)/sizeof(C))-1` (-1 because of the
  // null terminator). OR, create a specialization for BasicFixedString<C, 0u>
  // that does not have a size_ member, since it is unnecessary.
  Char data_[N + 1u]; // +1 for the null terminator
  std::size_t size_; // Nbr of chars, not incl. null terminator. size_ <= N.

  using Indices = std::make_index_sequence<N>;

  template <class That, std::size_t... Is>
  constexpr BasicFixedString(
      const That& that,
      std::size_t size,
      std::index_sequence<Is...>,
      std::size_t pos = 0,
      std::size_t count = npos) noexcept
      : data_{(Is < (size - pos) && Is < count ? that[Is + pos] : Char(0))..., Char(0)},
        size_{folly::constexpr_min(size - pos, count)} {}

  template <std::size_t... Is>
  constexpr BasicFixedString(
      std::size_t count, Char ch, std::index_sequence<Is...>) noexcept
      : data_{((Is < count) ? ch : Char(0))..., Char(0)}, size_{count} {}

  // Concatenation constructor
  template <class Left, class Right, std::size_t... Is>
  constexpr BasicFixedString(
      const Left& left,
      std::size_t left_size,
      const Right& right,
      std::size_t right_size,
      std::index_sequence<Is...>) noexcept
      : data_{detail::fixedstring::char_at_<Char>(left, left_size, right, right_size, Is)..., Char(0)},
        size_{left_size + right_size} {}

  // Replace constructor
  template <class Left, class Right, std::size_t... Is>
  constexpr BasicFixedString(
      const Left& left,
      std::size_t left_size,
      std::size_t left_pos,
      std::size_t left_count,
      const Right& right,
      std::size_t right_pos,
      std::size_t right_count,
      std::index_sequence<Is...>) noexcept
      : data_{detail::fixedstring::char_at_<Char>(left, left_size, left_pos, left_count, right, right_pos, right_count, Is)..., Char(0)},
        size_{left_size - left_count + right_count} {}

 public:
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = Char&;
  using const_reference = const Char&;
  using pointer = Char*;
  using const_pointer = const Char*;
  using iterator = Char*;
  using const_iterator = const Char*;
  using reverse_iterator = detail::fixedstring::ReverseIterator<Char>;
  using const_reverse_iterator =
      detail::fixedstring::ReverseIterator<const Char>;

  using detail::fixedstring::FixedStringBase::npos;

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Default construct
   * \post `size() == 0`
   * \post `at(0) == Char(0)`
   */
  constexpr BasicFixedString() : data_{}, size_{} {}

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Copy construct
   * \post `size() == that.size()`
   * \post `0 == strncmp(data(), that.data(), size())`
   * \post `at(size()) == Char(0)`
   */
  constexpr BasicFixedString(const BasicFixedString& /*that*/) = default;

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Construct from a differently-sized BasicFixedString
   * \pre `that.size() <= N`
   * \post `size() == that.size()`
   * \post `0 == strncmp(data(), that.data(), size())`
   * \post `at(size()) == Char(0)`
   * \throw std::out_of_range when that.size() > N. When M <= N, this
   *   constructor will never throw.
   * \note Conversions from larger-capacity BasicFixedString objects to smaller
   *   ones (`M > N`) are allowed as long as the *size()* of the source string
   *   is small enough.
   */
  template <std::size_t M>
  constexpr /* implicit */ BasicFixedString(
      const BasicFixedString<Char, M>& that) noexcept(M <= N)
      : BasicFixedString{that, 0u, that.size_} {}

  // Why is this deleted? To avoid confusion with the constructor that takes
  // a const Char* and a count.
  template <std::size_t M>
  constexpr BasicFixedString(
      const BasicFixedString<Char, M>& that,
      std::size_t pos) noexcept(false) = delete;

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Construct from an BasicFixedString, an offset, and a count
   * \param that The source string
   * \param pos The starting position in `that`
   * \param count The number of characters to copy. If `npos`, `count` is taken
   *              to be `that.size()-pos`.
   * \pre `pos <= that.size()`
   * \pre `count <= that.size()-pos && count <= N`
   * \post `size() == count`
   * \post `0 == strncmp(data(), that.data()+pos, size())`
   * \post `at(size()) == Char(0)`
   * \throw std::out_of_range when pos+count > that.size(), or when
   *        `count > N`
   */
  template <std::size_t M>
  constexpr BasicFixedString(
      const BasicFixedString<Char, M>& that,
      std::size_t pos,
      std::size_t count) noexcept(false)
      : BasicFixedString{
            that.data_,
            that.size_,
            std::make_index_sequence<(M < N ? M : N)>{},
            pos,
            detail::fixedstring::checkOverflow(
                detail::fixedstring::checkOverflowOrNpos(
                    count,
                    that.size_ -
                        detail::fixedstring::checkOverflow(pos, that.size_)),
                N)} {}

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Construct from a string literal
   * \pre `M-1 <= N`
   * \pre `that[M-1] == Char(0)`
   * \post `0 == strncmp(data(), that, M-1)`
   * \post `size() == M-1`
   * \post `at(size()) == Char(0)`
   */
  template <std::size_t M, class = typename std::enable_if<(M - 1u <= N)>::type>
  constexpr /* implicit */ BasicFixedString(const Char (&that)[M]) noexcept
      : BasicFixedString{
            detail::fixedstring::checkNullTerminated(that),
            M - 1u,
            std::make_index_sequence<M - 1u>{}} {}

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Construct from a `const Char*` and count
   * \pre `that` points to an array of at least `count` characters.
   * \pre `count <= N`
   * \post `size() == count`
   * \post `0 == strncmp(data(), that, size())`
   * \post `at(size()) == Char(0)`
   * \throw std::out_of_range when count > N
   */
  constexpr BasicFixedString(const Char* that, std::size_t count) noexcept(
      false)
      : BasicFixedString{
            that, detail::fixedstring::checkOverflow(count, N), Indices{}} {}

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Construct an BasicFixedString that contains `count` characters, all
   *   of which are `ch`.
   * \pre `count <= N`
   * \post `size() == count`
   * \post `npos == find_first_not_of(ch)`
   * \post `at(size()) == Char(0)`
   * \throw std::out_of_range when count > N
   */
  constexpr BasicFixedString(std::size_t count, Char ch) noexcept(false)
      : BasicFixedString{
            detail::fixedstring::checkOverflow(count, N), ch, Indices{}} {}

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Construct an BasicFixedString from a `std::initializer_list` of
   *   characters.
   * \pre `il.size() <= N`
   * \post `size() == count`
   * \post `0 == strncmp(data(), il.begin(), size())`
   * \post `at(size()) == Char(0)`
   * \throw std::out_of_range when il.size() > N
   */
  constexpr BasicFixedString(std::initializer_list<Char> il) noexcept(false)
      : BasicFixedString{il.begin(), il.size()} {}

  constexpr BasicFixedString& operator=(const BasicFixedString&) noexcept =
      default;

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Assign from a `BasicFixedString<Char, M>`.
   * \pre `that.size() <= N`
   * \post `size() == that.size()`
   * \post `0 == strncmp(data(), that.begin(), size())`
   * \post `at(size()) == Char(0)`
   * \throw std::out_of_range when that.size() > N. When M <= N, this
   *   assignment operator will never throw.
   * \note Assignments from larger-capacity BasicFixedString objects to smaller
   *   ones (`M > N`) are allowed as long as the *size* of the source string is
   *   small enough.
   * \return `*this`
   */
  template <std::size_t M>
  constexpr BasicFixedString& operator=(
      const BasicFixedString<Char, M>& that) noexcept(M <= N) {
    detail::fixedstring::checkOverflow(that.size_, N);
    size_ = that.copy(data_, that.size_);
    data_[size_] = Char(0);
    return *this;
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Assign from a null-terminated array of characters.
   * \pre `M < N`
   * \pre `that` has no embedded null characters
   * \pre `that[M-1]==Char(0)`
   * \post `size() == M-1`
   * \post `0 == strncmp(data(), that, size())`
   * \post `at(size()) == Char(0)`
   * \return `*this`
   */
  template <std::size_t M, class = typename std::enable_if<(M - 1u <= N)>::type>
  constexpr BasicFixedString& operator=(const Char (&that)[M]) noexcept {
    return assign(detail::fixedstring::checkNullTerminated(that), M - 1u);
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Assign from an `initializer_list` of characters.
   * \pre `il.size() <= N`
   * \post `size() == il.size()`
   * \post `0 == strncmp(data(), il.begin(), size())`
   * \post `at(size()) == Char(0)`
   * \throw std::out_of_range when il.size() > N
   * \return `*this`
   */
  constexpr BasicFixedString& operator=(
      std::initializer_list<Char> il) noexcept(false) {
    detail::fixedstring::checkOverflow(il.size(), N);
    for (std::size_t i = 0u; i < il.size(); ++i) {
      data_[i] = il.begin()[i];
    }
    size_ = il.size();
    data_[size_] = Char(0);
    return *this;
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Conversion to folly::Range
   * \return `Range<Char*>{begin(), end()}`
   */
  constexpr Range<Char*> toRange() noexcept { return {begin(), end()}; }

  /**
   * \overload
   */
  constexpr Range<const Char*> toRange() const noexcept {
    return {begin(), end()};
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Conversion to std::basic_string<Char>
   * \return `std::basic_string<Char>{begin(), end()}`
   */
  /* implicit */ operator std::basic_string<Char>() const noexcept(false) {
    return std::basic_string<Char>{begin(), end()};
  }

  std::basic_string<Char> toStdString() const noexcept(false) {
    return std::basic_string<Char>{begin(), end()};
  }

  // Think hard about whether this is a good idea. It's certainly better than
  // an implicit conversion to `const Char*` since `delete "hi"_fs` will fail
  // to compile. But it creates ambiguities when passing a FixedString to an
  // API that has overloads for `const char*` and `folly::Range`, for instance.
  // using ArrayType = Char[N];
  // constexpr /* implicit */ operator ArrayType&() noexcept {
  //   return data_;
  // }

  // using ConstArrayType = const Char[N];
  // constexpr /* implicit */ operator ConstArrayType&() const noexcept {
  //   return data_;
  // }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Assigns a sequence of `count` characters of value `ch`.
   * \param count The count of characters.
   * \param ch
   * \pre `count <= N`
   * \post `size() == count`
   * \post `npos == find_first_not_of(ch)`
   * \post `at(size()) == Char(0)`
   * \throw std::out_of_range when count > N
   * \return `*this`
   */
  constexpr BasicFixedString& assign(std::size_t count, Char ch) noexcept(
      false) {
    detail::fixedstring::checkOverflow(count, N);
    for (std::size_t i = 0u; i < count; ++i) {
      data_[i] = ch;
    }
    size_ = count;
    data_[size_] = Char(0);
    return *this;
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Assigns characters from an `BasicFixedString` to this object.
   * \note Equivalent to `assign(that, 0, that.size())`
   */
  template <std::size_t M>
  constexpr BasicFixedString& assign(
      const BasicFixedString<Char, M>& that) noexcept(M <= N) {
    return *this = that;
  }

  // Why is this overload deleted? So users aren't confused by the difference
  // between str.assign("foo", N) and str.assign("foo"_fs, N). In the former,
  // N is a count of characters. In the latter, it would be a position, which
  // totally changes the meaning of the code.
  template <std::size_t M>
  constexpr BasicFixedString& assign(
      const BasicFixedString<Char, M>& that,
      std::size_t pos) noexcept(false) = delete;

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Assigns `count` characters from an `BasicFixedString` to this object,
   *   starting at position `pos` in the source object.
   * \param that The source string.
   * \param pos The starting position in the source string.
   * \param count The number of characters to copy. If `npos`, `count` is taken
   *              to be `that.size()-pos`.
   * \pre `pos <= that.size()`
   * \pre `count <= that.size()-pos`
   * \pre `count <= N`
   * \post `size() == count`
   * \post `0 == strncmp(data(), that.begin() + pos, count)`
   * \post `at(size()) == Char(0)`
   * \throw std::out_of_range when pos > that.size() or count > that.size()-pos
   *        or count > N.
   * \return `*this`
   */
  template <std::size_t M>
  constexpr BasicFixedString& assign(
      const BasicFixedString<Char, M>& that,
      std::size_t pos,
      std::size_t count) noexcept(false) {
    detail::fixedstring::checkOverflow(pos, that.size_);
    return assign(
        that.data_ + pos,
        detail::fixedstring::checkOverflowOrNpos(count, that.size_ - pos));
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Assigns characters from an `BasicFixedString` to this object.
   * \pre `that` contains no embedded nulls.
   * \pre `that[M-1] == Char(0)`
   * \note Equivalent to `assign(that, M - 1)`
   */
  template <std::size_t M, class = typename std::enable_if<(M - 1u <= N)>::type>
  constexpr BasicFixedString& assign(const Char (&that)[M]) noexcept {
    return assign(detail::fixedstring::checkNullTerminated(that), M - 1u);
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Assigns `count` characters from a range of characters to this object.
   * \param that A pointer to a range of characters.
   * \param count The number of characters to copy.
   * \pre `that` points to at least `count` characters.
   * \pre `count <= N`
   * \post `size() == count`
   * \post `0 == strncmp(data(), that, count)`
   * \post `at(size()) == Char(0)`
   * \throw std::out_of_range when count > N
   * \return `*this`
   */
  constexpr BasicFixedString& assign(
      const Char* that, std::size_t count) noexcept(false) {
    detail::fixedstring::checkOverflow(count, N);
    for (std::size_t i = 0u; i < count; ++i) {
      data_[i] = that[i];
    }
    size_ = count;
    data_[size_] = Char(0);
    return *this;
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Swap the contents of this string with `that`.
   */
  constexpr void swap(BasicFixedString& that) noexcept {
    // less-than-or-equal here to copy the null terminator:
    for (std::size_t i = 0u; i <= folly::constexpr_max(size_, that.size_);
         ++i) {
      detail::fixedstring::constexpr_swap(data_[i], that.data_[i]);
    }
    detail::fixedstring::constexpr_swap(size_, that.size_);
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Return a pointer to a range of `size()+1` characters, the last of which
   * is `Char(0)`.
   */
  constexpr Char* data() noexcept { return data_; }

  /**
   * \overload
   */
  constexpr const Char* data() const noexcept { return data_; }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * \return `data()`.
   */
  constexpr const Char* c_str() const noexcept { return data_; }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * \return `data()`.
   */
  constexpr Char* begin() noexcept { return data_; }

  /**
   * \overload
   */
  constexpr const Char* begin() const noexcept { return data_; }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * \return `data()`.
   */
  constexpr const Char* cbegin() const noexcept { return begin(); }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * \return `data() + size()`.
   */
  constexpr Char* end() noexcept { return data_ + size_; }

  /**
   * \overload
   */
  constexpr const Char* end() const noexcept { return data_ + size_; }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * \return `data() + size()`.
   */
  constexpr const Char* cend() const noexcept { return end(); }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Returns a reverse iterator to the first character of the reversed string.
   * It corresponds to the last + 1 character of the non-reversed string.
   */
  constexpr reverse_iterator rbegin() noexcept {
    return reverse_iterator{data_ + size_};
  }

  /**
   * \overload
   */
  constexpr const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator{data_ + size_};
  }

  /**
   * \note Equivalent to `rbegin()` on a const-qualified reference to `*this`.
   */
  constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Returns a reverse iterator to the last + 1 character of the reversed
   * string. It corresponds to the first character of the non-reversed string.
   */
  constexpr reverse_iterator rend() noexcept { return reverse_iterator{data_}; }

  /**
   * \overload
   */
  constexpr const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator{data_};
  }

  /**
   * \note Equivalent to `rend()` on a const-qualified reference to `*this`.
   */
  constexpr const_reverse_iterator crend() const noexcept { return rend(); }

  /**
   * \return The number of `Char` elements in the string.
   */
  constexpr std::size_t size() const noexcept { return size_; }

  /**
   * \return The number of `Char` elements in the string.
   */
  constexpr std::size_t length() const noexcept { return size_; }

  /**
   * \return True if and only if `size() == 0`.
   */
  constexpr bool empty() const noexcept { return 0u == size_; }

  /**
   * \return `N`.
   */
  static constexpr std::size_t capacity() noexcept { return N; }

  /**
   * \return `N`.
   */
  static constexpr std::size_t max_size() noexcept { return N; }

  // We would need to reimplement folly::Hash to make this
  // constexpr. :-(
  std::uint32_t hash() const noexcept {
    return folly::hsieh_hash32_buf(data_, size_);
  }

  /**
   * \note `at(size())` is allowed will return `Char(0)`.
   * \return `*(data() + i)`
   * \throw std::out_of_range when i > size()
   */
  constexpr Char& at(std::size_t i) noexcept(false) {
    return i <= size_ ? data_[i]
                      : (throw_exception<std::out_of_range>(
                             "Out of range in BasicFixedString::at"),
                         data_[size_]);
  }

  /**
   * \overload
   */
  constexpr const Char& at(std::size_t i) const noexcept(false) {
    return i <= size_ ? data_[i]
                      : (throw_exception<std::out_of_range>(
                             "Out of range in BasicFixedString::at"),
                         data_[size_]);
  }

  /**
   * \pre `i <= size()`
   * \note `(*this)[size()]` is allowed will return `Char(0)`.
   * \return `*(data() + i)`
   */
  constexpr Char& operator[](std::size_t i) noexcept {
    return data_[detail::fixedstring::checkOverflowIfDebug(i, size_)];
  }

  /**
   * \overload
   */
  constexpr const Char& operator[](std::size_t i) const noexcept {
    return data_[detail::fixedstring::checkOverflowIfDebug(i, size_)];
  }

  /**
   * \note Equivalent to `(*this)[0]`
   */
  constexpr Char& front() noexcept { return (*this)[0u]; }

  /**
   * \overload
   */
  constexpr const Char& front() const noexcept { return (*this)[0u]; }

  /**
   * \note Equivalent to `at(size()-1)`
   * \pre `!empty()`
   */
  constexpr Char& back() noexcept {
    return data_[size_ - detail::fixedstring::checkOverflowIfDebug(1u, size_)];
  }

  /**
   * \overload
   */
  constexpr const Char& back() const noexcept {
    return data_[size_ - detail::fixedstring::checkOverflowIfDebug(1u, size_)];
  }

  /**
   * Clears the contents of this string.
   * \post `size() == 0u`
   * \post `at(size()) == Char(0)`
   */
  constexpr void clear() noexcept {
    data_[0u] = Char(0);
    size_ = 0u;
  }

  /**
   * \note Equivalent to `append(1u, ch)`.
   */
  constexpr void push_back(Char ch) noexcept(false) {
    detail::fixedstring::checkOverflow(1u, N - size_);
    data_[size_] = ch;
    data_[++size_] = Char(0);
  }

  /**
   * \note Equivalent to `cappend(1u, ch)`.
   */
  constexpr BasicFixedString<Char, N + 1u> cpush_back(Char ch) const noexcept {
    return cappend(ch);
  }

  /**
   * Removes the last character from the string.
   * \pre `!empty()`
   * \post `size()` is one fewer than before calling `pop_back()`.
   * \post `at(size()) == Char(0)`
   * \post The characters in the half-open range `[0,size()-1)` are unmodified.
   * \throw std::out_of_range if empty().
   */
  constexpr void pop_back() noexcept(false) {
    detail::fixedstring::checkOverflow(1u, size_);
    --size_;
    data_[size_] = Char(0);
  }

  /**
   * Returns a new string with the first `size()-1` characters from this string.
   * \pre `!empty()`
   * \note Equivalent to `BasicFixedString<Char, N-1u>{*this, 0u, size()-1u}`
   * \throw std::out_of_range if empty().
   */
  constexpr BasicFixedString<Char, N - 1u> cpop_back() const noexcept(false) {
    return {*this, 0u, size_ - detail::fixedstring::checkOverflow(1u, size_)};
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Appends `count` copies of `ch` to this string.
   * \pre `count + old_size <= N`
   * \post The first `old_size` characters of the string are unmodified.
   * \post `size() == old_size + count`
   * \throw std::out_of_range if count > N - size().
   */
  constexpr BasicFixedString& append(std::size_t count, Char ch) noexcept(
      false) {
    detail::fixedstring::checkOverflow(count, N - size_);
    for (std::size_t i = 0u; i < count; ++i) {
      data_[size_ + i] = ch;
    }
    size_ += count;
    data_[size_] = Char(0);
    return *this;
  }

  /**
   * \note Equivalent to `append(*this, 0, that.size())`.
   */
  template <std::size_t M>
  constexpr BasicFixedString& append(
      const BasicFixedString<Char, M>& that) noexcept(false) {
    return append(that, 0u, that.size_);
  }

  // Why is this overload deleted? So as not to get confused with
  // append("null-terminated", N), where N would be a count instead
  // of a position.
  template <std::size_t M>
  constexpr BasicFixedString& append(
      const BasicFixedString<Char, M>& that,
      std::size_t pos) noexcept(false) = delete;

  /**
   * Appends `count` characters from another string to this one, starting at a
   * given offset, `pos`.
   * \param that The source string.
   * \param pos The starting position in the source string.
   * \param count The number of characters to append. If `npos`, `count` is
   *              taken to be `that.size()-pos`.
   * \pre `pos <= that.size()`
   * \pre `count <= that.size() - pos`
   * \pre `old_size + count <= N`
   * \post The first `old_size` characters of the string are unmodified.
   * \post `size() == old_size + count`
   * \post `at(size()) == Char(0)`
   * \throw std::out_of_range if pos + count > that.size() or if
   *        `old_size + count > N`.
   */
  template <std::size_t M>
  constexpr BasicFixedString& append(
      const BasicFixedString<Char, M>& that,
      std::size_t pos,
      std::size_t count) noexcept(false) {
    detail::fixedstring::checkOverflow(pos, that.size_);
    count = detail::fixedstring::checkOverflowOrNpos(count, that.size_ - pos);
    detail::fixedstring::checkOverflow(count, N - size_);
    for (std::size_t i = 0u; i < count; ++i) {
      data_[size_ + i] = that.data_[pos + i];
    }
    size_ += count;
    data_[size_] = Char(0);
    return *this;
  }

  /**
   * \note Equivalent to `append(that, strlen(that))`.
   */
  constexpr BasicFixedString& append(const Char* that) noexcept(false) {
    return append(that, folly::constexpr_strlen(that));
  }

  /**
   * Appends `count` characters from the specified character array.
   * \pre `that` points to a range of at least `count` characters.
   * \pre `count + old_size <= N`
   * \post The first `old_size` characters of the string are unmodified.
   * \post `size() == old_size + count`
   * \post `at(size()) == Char(0)`
   * \throw std::out_of_range if old_size + count > N.
   */
  constexpr BasicFixedString& append(
      const Char* that, std::size_t count) noexcept(false) {
    detail::fixedstring::checkOverflow(count, N - size_);
    for (std::size_t i = 0u; i < count; ++i) {
      data_[size_ + i] = that[i];
    }
    size_ += count;
    data_[size_] = Char(0);
    return *this;
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Creates a new string by appending a character to an existing string, which
   *   is left unmodified.
   * \note Equivalent to `*this + ch`
   */
  constexpr BasicFixedString<Char, N + 1u> cappend(Char ch) const noexcept {
    return *this + ch;
  }

  /**
   * Creates a new string by appending a string to an existing string, which
   *   is left unmodified.
   * \note Equivalent to `*this + ch`
   */
  template <std::size_t M>
  constexpr BasicFixedString<Char, N + M> cappend(
      const BasicFixedString<Char, M>& that) const noexcept {
    return *this + that;
  }

  // Deleted to avoid confusion with append("char*", N), where N is a count
  // instead of a position.
  template <std::size_t M>
  constexpr BasicFixedString<Char, N + M> cappend(
      const BasicFixedString<Char, M>& that, std::size_t pos) const
      noexcept(false) = delete;

  /**
   * Creates a new string by appending characters from one string to another,
   *   which is left unmodified.
   * \note Equivalent to `*this + that.substr(pos, count)`
   */
  template <std::size_t M>
  constexpr BasicFixedString<Char, N + M> cappend(
      const BasicFixedString<Char, M>& that,
      std::size_t pos,
      std::size_t count) const noexcept(false) {
    return creplace(size_, 0u, that, pos, count);
  }

  /**
   * Creates a new string by appending a string literal to a string,
   *   which is left unmodified.
   * \note Equivalent to `*this + that`
   */
  template <std::size_t M>
  constexpr BasicFixedString<Char, N + M - 1u> cappend(
      const Char (&that)[M]) const noexcept {
    return creplace(size_, 0u, that);
  }

  // Deleted to avoid confusion with append("char*", N), where N is a count
  // instead of a position
  template <std::size_t M>
  constexpr BasicFixedString<Char, N + M - 1u> cappend(
      const Char (&that)[M], std::size_t pos) const noexcept(false) = delete;

  /**
   * Creates a new string by appending characters from one string to another,
   *   which is left unmodified.
   * \note Equivalent to `*this + makeFixedString(that).substr(pos, count)`
   */
  template <std::size_t M>
  constexpr BasicFixedString<Char, N + M - 1u> cappend(
      const Char (&that)[M], std::size_t pos, std::size_t count) const
      noexcept(false) {
    return creplace(size_, 0u, that, pos, count);
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Appends characters from a null-terminated string literal to this string.
   * \note Equivalent to `append(that)`.
   */
  constexpr BasicFixedString& operator+=(const Char* that) noexcept(false) {
    return append(that);
  }

  /**
   * Appends characters from another string to this one.
   * \note Equivalent to `append(that)`.
   */
  template <std::size_t M>
  constexpr BasicFixedString& operator+=(
      const BasicFixedString<Char, M>& that) noexcept(false) {
    return append(that, 0u, that.size_);
  }

  /**
   * Appends a character to this string.
   * \note Equivalent to `push_back(ch)`.
   */
  constexpr BasicFixedString& operator+=(Char ch) noexcept(false) {
    push_back(ch);
    return *this;
  }

  /**
   * Appends characters from an `initializer_list` to this string.
   * \note Equivalent to `append(il.begin(), il.size())`.
   */
  constexpr BasicFixedString& operator+=(
      std::initializer_list<Char> il) noexcept(false) {
    return append(il.begin(), il.size());
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Erase all characters from this string.
   * \note Equivalent to `clear()`
   * \return *this;
   */
  constexpr BasicFixedString& erase() noexcept {
    clear();
    return *this;
  }

  /**
   * Erases `count` characters from position `pos`. If `count` is `npos`,
   *   erases from `pos` to the end of the string.
   * \pre `pos <= size()`
   * \pre `count <= size() - pos || count == npos`
   * \post `size() == old_size - min(count, old_size - pos)`
   * \post `at(size()) == Char(0)`
   * \return *this;
   * \throw std::out_of_range when pos > size().
   */
  constexpr BasicFixedString& erase(
      std::size_t pos, std::size_t count = npos) noexcept(false) {
    using A = const Char[1];
    constexpr A a{Char(0)};
    return replace(
        pos,
        detail::fixedstring::checkOverflowOrNpos(
            count, size_ - detail::fixedstring::checkOverflow(pos, size_)),
        a,
        0u);
  }

  /**
   * \note Equivalent to `erase(first - data(), 1)`
   * \return A pointer to the first character after the erased character.
   */
  constexpr Char* erase(const Char* first) noexcept(false) {
    erase(first - data_, 1u);
    return data_ + (first - data_);
  }

  /**
   * \note Equivalent to `erase(first - data(), last - first)`
   * \return A pointer to the first character after the erased characters.
   */
  constexpr Char* erase(const Char* first, const Char* last) noexcept(false) {
    erase(first - data_, last - first);
    return data_ + (first - data_);
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Create a new string by erasing all the characters from this string.
   * \note Equivalent to `BasicFixedString<Char, 0>{}`
   */
  constexpr BasicFixedString<Char, 0u> cerase() const noexcept { return {}; }

  /**
   * Create a new string by erasing all the characters after position `pos` from
   *   this string.
   * \note Equivalent to `creplace(pos, min(count, pos - size()), "")`
   */
  constexpr BasicFixedString cerase(
      std::size_t pos, std::size_t count = npos) const noexcept(false) {
    using A = const Char[1];
    return creplace(
        pos,
        detail::fixedstring::checkOverflowOrNpos(
            count, size_ - detail::fixedstring::checkOverflow(pos, size_)),
        A{Char(0)});
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Compare two strings for lexicographical ordering.
   * \note Equivalent to
   * `compare(0, size(), that.data(), that.size())`
   */
  template <std::size_t M>
  constexpr int compare(const BasicFixedString<Char, M>& that) const noexcept {
    return compare(0u, size_, that, 0u, that.size_);
  }

  /**
   * Compare two strings for lexicographical ordering.
   * \note Equivalent to
   * `compare(this_pos, this_count, that.data(), that.size())`
   */
  template <std::size_t M>
  constexpr int compare(
      std::size_t this_pos,
      std::size_t this_count,
      const BasicFixedString<Char, M>& that) const noexcept(false) {
    return compare(this_pos, this_count, that, 0u, that.size_);
  }

  /**
   * Compare two strings for lexicographical ordering.
   * \note Equivalent to
   * `compare(this_pos, this_count, that.data() + that_pos, that_count)`
   */
  template <std::size_t M>
  constexpr int compare(
      std::size_t this_pos,
      std::size_t this_count,
      const BasicFixedString<Char, M>& that,
      std::size_t that_pos,
      std::size_t that_count) const noexcept(false) {
    return static_cast<int>(detail::fixedstring::compare_(
        data_,
        detail::fixedstring::checkOverflow(this_pos, size_),
        detail::fixedstring::checkOverflow(this_count, size_ - this_pos) +
            this_pos,
        that.data_,
        detail::fixedstring::checkOverflow(that_pos, that.size_),
        detail::fixedstring::checkOverflow(that_count, that.size_ - that_pos) +
            that_pos));
  }

  /**
   * Compare two strings for lexicographical ordering.
   * \note Equivalent to `compare(0, size(), that, strlen(that))`
   */
  constexpr int compare(const Char* that) const noexcept {
    return compare(0u, size_, that, folly::constexpr_strlen(that));
  }

  /**
   * \overload
   */
  constexpr int compare(Range<const Char*> that) const noexcept {
    return compare(0u, size_, that.begin(), that.size());
  }

  /**
   * Compare two strings for lexicographical ordering.
   * \note Equivalent to
   *   `compare(this_pos, this_count, that, strlen(that))`
   */
  constexpr int compare(
      std::size_t this_pos, std::size_t this_count, const Char* that) const
      noexcept(false) {
    return compare(this_pos, this_count, that, folly::constexpr_strlen(that));
  }

  /**
   * \overload
   */
  constexpr int compare(
      std::size_t this_pos,
      std::size_t this_count,
      Range<const Char*> that) const noexcept(false) {
    return compare(this_pos, this_count, that.begin(), that.size());
  }

  /**
   * Compare two strings for lexicographical ordering.
   *
   * Let `A` be the
   *   character sequence {`(*this)[this_pos]`, ...
   *   `(*this)[this_pos + this_count - 1]`}. Let `B` be the character sequence
   *   {`that[0]`, ...`that[count - 1]`}. Then...
   *
   * \return
   *   - `< 0` if `A` is ordered before the `B`
   *   - `> 0` if `B` is ordered before `A`
   *   - `0` if `A` equals `B`.
   *
   * \throw std::out_of_range if this_pos + this_count > size().
   */
  constexpr int compare(
      std::size_t this_pos,
      std::size_t this_count,
      const Char* that,
      std::size_t that_count) const noexcept(false) {
    return static_cast<int>(detail::fixedstring::compare_(
        data_,
        detail::fixedstring::checkOverflow(this_pos, size_),
        detail::fixedstring::checkOverflowOrNpos(this_count, size_ - this_pos) +
            this_pos,
        that,
        0u,
        that_count));
  }

  constexpr int compare(
      std::size_t this_pos,
      std::size_t this_count,
      Range<const Char*> that,
      std::size_t that_count) const noexcept(false) {
    return compare(
        this_pos,
        this_count,
        that.begin(),
        detail::fixedstring::checkOverflow(that_count, that.size()));
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Return a substring from `pos` to the end of the string.
   * \note Equivalent to `BasicFixedString{*this, pos}`
   */
  constexpr BasicFixedString substr(std::size_t pos) const noexcept(false) {
    return {*this, pos};
  }

  /**
   * Return a substring from `pos` to the end of the string.
   * \note Equivalent to `BasicFixedString{*this, pos, count}`
   */
  constexpr BasicFixedString substr(std::size_t pos, std::size_t count) const
      noexcept(false) {
    return {*this, pos, count};
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Replace the characters in the range denoted by the half-open range
   *   [`first`, `last`) with the string `that`.
   * \pre `first` and `last` point to characters within this string (including
   *   the terminating null).
   * \note Equivalent to
   *   `replace(first - data(), last - first, that.data(), that.size())`
   */
  template <std::size_t M>
  constexpr BasicFixedString& replace(
      const Char* first,
      const Char* last,
      const BasicFixedString<Char, M>& that) noexcept(false) {
    return replace(first - data_, last - first, that, 0u, that.size_);
  }

  /**
   * Replace `this_count` characters starting from position `this_pos` with the
   *   characters from string `that` starting at position `that_pos`.
   * \pre `that_pos <= that.size()`
   * \note Equivalent to
   *   <tt>replace(this_pos, this_count, that.data() + that_pos,
   *   that.size() - that_pos)</tt>
   */
  template <std::size_t M>
  constexpr BasicFixedString& replace(
      std::size_t this_pos,
      std::size_t this_count,
      const BasicFixedString<Char, M>& that,
      std::size_t that_pos = 0u) noexcept(false) {
    return replace(this_pos, this_count, that, that_pos, that.size_ - that_pos);
  }

  /**
   * Replace `this_count` characters starting from position `this_pos` with
   *   `that_count` characters from string `that` starting at position
   *   `that_pos`.
   * \pre `that_pos <= that.size() && that_count <= that.size() - that_pos`
   * \note Equivalent to
   *   `replace(this_pos, this_count, that.data() + that_pos, that_count)`
   */
  template <std::size_t M>
  constexpr BasicFixedString& replace(
      std::size_t this_pos,
      std::size_t this_count,
      const BasicFixedString<Char, M>& that,
      std::size_t that_pos,
      std::size_t that_count) noexcept(false) {
    return *this = creplace(this_pos, this_count, that, that_pos, that_count);
  }

  /**
   * Replace `this_count` characters starting from position `this_pos` with
   *   the characters from the string literal `that`.
   * \note Equivalent to
   *   `replace(this_pos, this_count, that, strlen(that))`
   */
  constexpr BasicFixedString& replace(
      std::size_t this_pos,
      std::size_t this_count,
      const Char* that) noexcept(false) {
    return replace(this_pos, this_count, that, folly::constexpr_strlen(that));
  }

  /**
   * Replace the characters denoted by the half-open range [`first`,`last`) with
   *   the characters from the string literal `that`.
   * \pre `first` and `last` point to characters within this string (including
   *   the terminating null).
   * \note Equivalent to
   *   `replace(first - data(), last - first, that, strlen(that))`
   */
  constexpr BasicFixedString& replace(
      const Char* first, const Char* last, const Char* that) noexcept(false) {
    return replace(
        first - data_, last - first, that, folly::constexpr_strlen(that));
  }

  /**
   * Replace `this_count` characters starting from position `this_pos` with
   *   `that_count` characters from the character sequence pointed to by `that`.
   * \param this_pos The starting offset within `*this` of the first character
   *   to be replaced.
   * \param this_count The number of characters to be replaced. If `npos`,
   *   it is treated as if `this_count` were `size() - this_pos`.
   * \param that A pointer to the replacement string.
   * \param that_count The number of characters in the replacement string.
   * \pre `this_pos <= size() && this_count <= size() - this_pos`
   * \pre `that` points to a contiguous sequence of at least `that_count`
   *   characters
   * \throw std::out_of_range on any of the following conditions:
   *   - `this_pos > size()`
   *   - `this_count > size() - this_pos`
   *   - `size() - this_count + that_count > N`
   */
  constexpr BasicFixedString& replace(
      std::size_t this_pos,
      std::size_t this_count,
      const Char* that,
      std::size_t that_count) noexcept(false) {
    return *this = detail::fixedstring::Helper::replace_<Char>(
               data_,
               size_,
               detail::fixedstring::checkOverflow(this_pos, size_),
               detail::fixedstring::checkOverflowOrNpos(
                   this_count, size_ - this_pos),
               that,
               0u,
               that_count,
               Indices{});
  }

  /**
   * Replace `this_count` characters starting from position `this_pos` with
   *   `that_count` characters `ch`.
   * \note Equivalent to
   *   `replace(this_pos, this_count, BasicFixedString{that_count, ch})`
   */
  constexpr BasicFixedString& replace(
      std::size_t this_pos,
      std::size_t this_count,
      std::size_t that_count,
      Char ch) noexcept(false) {
    return replace(this_pos, this_count, BasicFixedString{that_count, ch});
  }

  /**
   * Replace the characters denoted by the half-open range [`first`,`last`)
   *   with `that_count` characters `ch`.
   * \note Equivalent to
   *   `replace(first - data(), last - first, BasicFixedString{that_count, ch})`
   */
  constexpr BasicFixedString& replace(
      const Char* first,
      const Char* last,
      std::size_t that_count,
      Char ch) noexcept(false) {
    return replace(
        first - data_, last - first, BasicFixedString{that_count, ch});
  }

  /**
   * Replace the characters denoted by the half-open range [`first`,`last`) with
   *   the characters from the string literal `that`.
   * \pre `first` and `last` point to characters within this string (including
   *   the terminating null).
   * \note Equivalent to
   *   `replace(this_pos, this_count, il.begin(), il.size())`
   */
  constexpr BasicFixedString& replace(
      const Char* first,
      const Char* last,
      std::initializer_list<Char> il) noexcept(false) {
    return replace(first - data_, last - first, il.begin(), il.size());
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Construct a new string by replacing `this_count` characters starting from
   *   position `this_pos` within this string with the characters from string
   *   `that` starting at position `that_pos`.
   * \pre `that_pos <= that.size()`
   * \note Equivalent to
   *   <tt>creplace(this_pos, this_count, that, that_pos,
   *   that.size() - that_pos)</tt>
   */
  template <std::size_t M>
  constexpr BasicFixedString<Char, N + M> creplace(
      std::size_t this_pos,
      std::size_t this_count,
      const BasicFixedString<Char, M>& that,
      std::size_t that_pos = 0u) const noexcept(false) {
    return creplace(
        this_pos,
        this_count,
        that,
        that_pos,
        that.size_ - detail::fixedstring::checkOverflow(that_pos, that.size_));
  }

  /**
   * Construct a new string by replacing `this_count` characters starting from
   *   position `this_pos` within this string with `that_count` characters from
   *   string `that` starting at position `that_pos`.
   * \param this_pos The starting offset within `*this` of the first character
   *   to be replaced.
   * \param this_count The number of characters to be replaced. If `npos`,
   *   it is treated as if `this_count` were `size() - this_pos`.
   * \param that A string that contains the replacement string.
   * \param that_pos The offset to the first character in the replacement
   *   string.
   * \param that_count The number of characters in the replacement string.
   * \pre `this_pos <= size() && this_count <= size() - this_pos`
   * \pre `that_pos <= that.size() && that_count <= that.size() - that_pos`
   * \post The size of the returned string is `size() - this_count + that_count`
   * \note Equivalent to <tt>BasicFixedString<Char, N + M>{substr(0, this_pos) +
   *    that.substr(that_pos, that_count) + substr(this_pos + this_count)}</tt>
   * \throw std::out_of_range on any of the following conditions:
   *   - `this_pos > size()`
   *   - `this_count > size() - this_pos`
   *   - `that_pos > that.size()`
   *   - `that_count > that.size() - that_pos`
   */
  template <std::size_t M>
  constexpr BasicFixedString<Char, N + M> creplace(
      std::size_t this_pos,
      std::size_t this_count,
      const BasicFixedString<Char, M>& that,
      std::size_t that_pos,
      std::size_t that_count) const noexcept(false) {
    return detail::fixedstring::Helper::replace_<Char>(
        data_,
        size_,
        detail::fixedstring::checkOverflow(this_pos, size_),
        detail::fixedstring::checkOverflowOrNpos(this_count, size_ - this_pos),
        that.data_,
        detail::fixedstring::checkOverflow(that_pos, that.size_),
        detail::fixedstring::checkOverflowOrNpos(
            that_count, that.size_ - that_pos),
        std::make_index_sequence<N + M>{});
  }

  /**
   * Construct a new string by replacing the characters denoted by the half-open
   *   range [`first`,`last`) within this string with the characters from string
   *   `that` starting at position `that_pos`.
   * \pre `that_pos <= that.size()`
   * \note Equivalent to
   *   <tt>creplace(first - data(), last - first, that, that_pos,
   *   that.size() - that_pos)</tt>
   */
  template <std::size_t M>
  constexpr BasicFixedString<Char, N + M> creplace(
      const Char* first,
      const Char* last,
      const BasicFixedString<Char, M>& that,
      std::size_t that_pos = 0u) const noexcept(false) {
    return creplace(
        first - data_,
        last - first,
        that,
        that_pos,
        that.size_ - detail::fixedstring::checkOverflow(that_pos, that.size_));
  }

  /**
   * Construct a new string by replacing the characters denoted by the half-open
   *   range [`first`,`last`) within this string with the `that_count`
   *   characters from string `that` starting at position `that_pos`.
   * \note Equivalent to
   *   <tt>creplace(first - data(), last - first, that, that_pos,
   *   that_count)</tt>
   */
  template <std::size_t M>
  constexpr BasicFixedString<Char, N + M> creplace(
      const Char* first,
      const Char* last,
      const BasicFixedString<Char, M>& that,
      std::size_t that_pos,
      std::size_t that_count) const noexcept(false) {
    return creplace(first - data_, last - first, that, that_pos, that_count);
  }

  /**
   * Construct a new string by replacing `this_count` characters starting from
   *   position `this_pos` within this string with `M-1` characters from
   *   character array `that`.
   * \pre `strlen(that) == M-1`
   * \note Equivalent to
   *   <tt>creplace(this_pos, this_count, that, 0, M - 1)</tt>
   */
  template <std::size_t M>
  constexpr BasicFixedString<Char, N + M - 1u> creplace(
      std::size_t this_pos, std::size_t this_count, const Char (&that)[M]) const
      noexcept(false) {
    return creplace(this_pos, this_count, that, 0u, M - 1u);
  }

  /**
   * Replace `this_count` characters starting from position `this_pos` with
   *   `that_count` characters from the character array `that` starting at
   *   position `that_pos`.
   * \param this_pos The starting offset within `*this` of the first character
   *   to be replaced.
   * \param this_count The number of characters to be replaced. If `npos`,
   *   it is treated as if `this_count` were `size() - this_pos`.
   * \param that An array of characters containing the replacement string.
   * \param that_pos The starting offset of the replacement string.
   * \param that_count The number of characters in the replacement string.  If
   *   `npos`, it is treated as if `that_count` were `M - 1 - that_pos`
   * \pre `this_pos <= size() && this_count <= size() - this_pos`
   * \pre `that_pos <= M - 1 && that_count <= M - 1 - that_pos`
   * \post The size of the returned string is `size() - this_count + that_count`
   * \note Equivalent to <tt>BasicFixedString<Char, N + M - 1>{
   *    substr(0, this_pos) +
   *    makeFixedString(that).substr(that_pos, that_count) +
   *    substr(this_pos + this_count)}</tt>
   * \throw std::out_of_range on any of the following conditions:
   *   - `this_pos > size()`
   *   - `this_count > size() - this_pos`
   *   - `that_pos >= M`
   *   - `that_count >= M - that_pos`
   */
  template <std::size_t M>
  constexpr BasicFixedString<Char, N + M - 1u> creplace(
      std::size_t this_pos,
      std::size_t this_count,
      const Char (&that)[M],
      std::size_t that_pos,
      std::size_t that_count) const noexcept(false) {
    return detail::fixedstring::Helper::replace_<Char>(
        data_,
        size_,
        detail::fixedstring::checkOverflow(this_pos, size_),
        detail::fixedstring::checkOverflowOrNpos(this_count, size_ - this_pos),
        detail::fixedstring::checkNullTerminated(that),
        detail::fixedstring::checkOverflow(that_pos, M - 1u),
        detail::fixedstring::checkOverflowOrNpos(that_count, M - 1u - that_pos),
        std::make_index_sequence<N + M - 1u>{});
  }

  /**
   * Construct a new string by replacing the characters denoted by the half-open
   *   range [`first`,`last`) within this string with the first `M-1`
   *   characters from the character array `that`.
   * \pre `strlen(that) == M-1`
   * \note Equivalent to
   *   <tt>creplace(first - data(), last - first, that, 0, M-1)</tt>
   */
  template <std::size_t M>
  constexpr BasicFixedString<Char, N + M - 1u> creplace(
      const Char* first, const Char* last, const Char (&that)[M]) const
      noexcept(false) {
    return creplace(first - data_, last - first, that, 0u, M - 1u);
  }

  /**
   * Construct a new string by replacing the characters denoted by the half-open
   *   range [`first`,`last`) within this string with the `that_count`
   *   characters from the character array `that` starting at position
   *   `that_pos`.
   * \pre `strlen(that) == M-1`
   * \note Equivalent to
   *   `creplace(first - data(), last - first, that, that_pos, that_count)`
   */
  template <std::size_t M>
  constexpr BasicFixedString<Char, N + M - 1u> creplace(
      const Char* first,
      const Char* last,
      const Char (&that)[M],
      std::size_t that_pos,
      std::size_t that_count) const noexcept(false) {
    return creplace(first - data_, last - first, that, that_pos, that_count);
  }

  /**
   * Copies `min(count, size())` characters starting from offset `0`
   *   from this string into the buffer pointed to by `dest`.
   * \return The number of characters copied.
   */
  constexpr std::size_t copy(Char* dest, std::size_t count) const noexcept {
    return copy(dest, count, 0u);
  }

  /**
   * Copies `min(count, size() - pos)` characters starting from offset `pos`
   *   from this string into the buffer pointed to by `dest`.
   * \pre `pos <= size()`
   * \return The number of characters copied.
   * \throw std::out_of_range if `pos > size()`
   */
  constexpr std::size_t copy(
      Char* dest, std::size_t count, std::size_t pos) const noexcept(false) {
    detail::fixedstring::checkOverflow(pos, size_);
    for (std::size_t i = 0u; i < count; ++i) {
      if (i + pos == size_) {
        return size_;
      }
      dest[i] = data_[i + pos];
    }
    return count;
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Resizes the current string.
   * \note Equivalent to `resize(count, Char(0))`
   */
  constexpr void resize(std::size_t count) noexcept(false) {
    resize(count, Char(0));
  }

  /**
   * Resizes the current string by setting the size to `count` and setting
   *   `data()[count]` to `Char(0)`. If `count > old_size`, the characters
   *   in the range [`old_size`,`count`) are set to `ch`.
   */
  constexpr void resize(std::size_t count, Char ch) noexcept(false) {
    detail::fixedstring::checkOverflow(count, N);
    if (count == size_) {
    } else if (count < size_) {
      size_ = count;
      data_[size_] = Char(0);
    } else {
      for (; size_ < count; ++size_) {
        data_[size_] = ch;
      }
      data_[size_] = Char(0);
    }
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Finds the first occurrence of the character sequence `that` in this string.
   * \note Equivalent to `find(that.data(), 0, that.size())`
   */
  template <std::size_t M>
  constexpr std::size_t find(
      const BasicFixedString<Char, M>& that) const noexcept {
    return find(that, 0u);
  }

  /**
   * Finds the first occurrence of the character sequence `that` in this string,
   *   starting at offset `pos`.
   * \pre `pos <= size()`
   * \note Equivalent to `find(that.data(), pos, that.size())`
   */
  template <std::size_t M>
  constexpr std::size_t find(
      const BasicFixedString<Char, M>& that, std::size_t pos) const
      noexcept(false) {
    return that.size_ <= size_ - detail::fixedstring::checkOverflow(pos, size_)
        ? detail::fixedstring::find_(data_, size_, that.data_, pos, that.size_)
        : npos;
  }

  /**
   * Finds the first occurrence of the character sequence `that` in this string.
   * \note Equivalent to `find(that.data(), 0, strlen(that))`
   */
  constexpr std::size_t find(const Char* that) const noexcept {
    return find(that, 0u, folly::constexpr_strlen(that));
  }

  /**
   * Finds the first occurrence of the character sequence `that` in this string,
   *   starting at offset `pos`.
   * \pre `pos <= size()`
   * \note Equivalent to `find(that.data(), pos, strlen(that))`
   */
  constexpr std::size_t find(const Char* that, std::size_t pos) const
      noexcept(false) {
    return find(that, pos, folly::constexpr_strlen(that));
  }

  /**
   * Finds the first occurrence of the first `count` characters in the buffer
   *   pointed to by `that` in this string, starting at offset `pos`.
   * \pre `pos <= size()`
   * \pre `that` points to a buffer containing at least `count` contiguous
   *   characters.
   * \return The lowest offset `i` such that `i >= pos` and
   *   `0 == strncmp(data() + i, that, count)`; or `npos` if there is no such
   *   offset `i`.
   * \throw std::out_of_range when `pos > size()`
   */
  constexpr std::size_t find(
      const Char* that, std::size_t pos, std::size_t count) const
      noexcept(false) {
    return count <= size_ - detail::fixedstring::checkOverflow(pos, size_)
        ? detail::fixedstring::find_(data_, size_, that, pos, count)
        : npos;
  }

  /**
   * Finds the first occurrence of the character `ch` in this string.
   * \note Equivalent to `find(&ch, 0, 1)`
   */
  constexpr std::size_t find(Char ch) const noexcept { return find(ch, 0u); }

  /**
   * Finds the first occurrence of the character character `c` in this string,
   *   starting at offset `pos`.
   * \pre `pos <= size()`
   * \note Equivalent to `find(&ch, pos, 1)`
   */
  constexpr std::size_t find(Char ch, std::size_t pos) const noexcept(false) {
    using A = const Char[1u];
    return 0u == size_ - detail::fixedstring::checkOverflow(pos, size_)
        ? npos
        : detail::fixedstring::find_(data_, size_, A{ch}, pos, 1u);
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Finds the last occurrence of characters in the string
   *   `that` in this string.
   * \note Equivalent to `rfind(that.data(), size(), that.size())`
   */
  template <std::size_t M>
  constexpr std::size_t rfind(
      const BasicFixedString<Char, M>& that) const noexcept {
    return rfind(that, size_);
  }

  /**
   * Finds the last occurrence of characters in the string
   *   `that` in this string, starting at offset `pos`.
   * \note Equivalent to `rfind(that.data(), pos, that.size())`
   */
  template <std::size_t M>
  constexpr std::size_t rfind(
      const BasicFixedString<Char, M>& that, std::size_t pos) const
      noexcept(false) {
    return that.size_ <= size_
        ? detail::fixedstring::rfind_(
              data_,
              that.data_,
              folly::constexpr_min(
                  detail::fixedstring::checkOverflow(pos, size_),
                  size_ - that.size_),
              that.size_)
        : npos;
  }

  /**
   * Finds the last occurrence of characters in the buffer
   *   pointed to by `that` in this string.
   * \note Equivalent to `rfind(that, size(), strlen(that))`
   */
  constexpr std::size_t rfind(const Char* that) const noexcept {
    return rfind(that, size_, folly::constexpr_strlen(that));
  }

  /**
   * Finds the last occurrence of characters in the buffer
   *   pointed to by `that` in this string, starting at offset `pos`.
   * \note Equivalent to `rfind(that, pos, strlen(that))`
   */
  constexpr std::size_t rfind(const Char* that, std::size_t pos) const
      noexcept(false) {
    return rfind(that, pos, folly::constexpr_strlen(that));
  }

  /**
   * Finds the last occurrence of the first `count` characters in the buffer
   *   pointed to by `that` in this string, starting at offset `pos`.
   * \pre `pos <= size()`
   * \pre `that` points to a buffer containing at least `count` contiguous
   *   characters.
   * \return The largest offset `i` such that `i <= pos` and
   *   `i + count <= size()` and `0 == strncmp(data() + i, that, count)`; or
   *   `npos` if there is no such offset `i`.
   * \throw std::out_of_range when `pos > size()`
   */
  constexpr std::size_t rfind(
      const Char* that, std::size_t pos, std::size_t count) const
      noexcept(false) {
    return count <= size_
        ? detail::fixedstring::rfind_(
              data_,
              that,
              folly::constexpr_min(
                  detail::fixedstring::checkOverflow(pos, size_),
                  size_ - count),
              count)
        : npos;
  }

  /**
   * Finds the last occurrence of the character character `ch` in this string.
   * \note Equivalent to `rfind(&ch, size(), 1)`
   */
  constexpr std::size_t rfind(Char ch) const noexcept {
    return rfind(ch, size_);
  }

  /**
   * Finds the last occurrence of the character character `ch` in this string,
   *   starting at offset `pos`.
   * \pre `pos <= size()`
   * \note Equivalent to `rfind(&ch, pos, 1)`
   */
  constexpr std::size_t rfind(Char ch, std::size_t pos) const noexcept(false) {
    using A = const Char[1u];
    return 0u == size_
        ? npos
        : detail::fixedstring::rfind_(
              data_,
              A{ch},
              folly::constexpr_min(
                  detail::fixedstring::checkOverflow(pos, size_), size_ - 1u),
              1u);
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Finds the first occurrence of any character in `that` in this string.
   * \note Equivalent to `find_first_of(that.data(), 0, that.size())`
   */
  template <std::size_t M>
  constexpr std::size_t find_first_of(
      const BasicFixedString<Char, M>& that) const noexcept {
    return find_first_of(that, 0u);
  }

  /**
   * Finds the first occurrence of any character in `that` in this string,
   *   starting at offset `pos`
   * \note Equivalent to `find_first_of(that.data(), pos, that.size())`
   */
  template <std::size_t M>
  constexpr std::size_t find_first_of(
      const BasicFixedString<Char, M>& that, std::size_t pos) const
      noexcept(false) {
    return size_ == detail::fixedstring::checkOverflow(pos, size_)
        ? npos
        : detail::fixedstring::find_first_of_(
              data_, size_, that.data_, pos, that.size_);
  }

  /**
   * Finds the first occurrence of any character in the null-terminated
   *   character sequence pointed to by `that` in this string.
   * \note Equivalent to `find_first_of(that, 0, strlen(that))`
   */
  constexpr std::size_t find_first_of(const Char* that) const noexcept {
    return find_first_of(that, 0u, folly::constexpr_strlen(that));
  }

  /**
   * Finds the first occurrence of any character in the null-terminated
   *   character sequence pointed to by `that` in this string,
   *   starting at offset `pos`
   * \note Equivalent to `find_first_of(that, pos, strlen(that))`
   */
  constexpr std::size_t find_first_of(const Char* that, std::size_t pos) const
      noexcept(false) {
    return find_first_of(that, pos, folly::constexpr_strlen(that));
  }

  /**
   * Finds the first occurrence of any character in the first `count` characters
   *   in the buffer pointed to by `that` in this string, starting at offset
   *  `pos`.
   * \pre `pos <= size()`
   * \pre `that` points to a buffer containing at least `count` contiguous
   *   characters.
   * \return The smallest offset `i` such that `i >= pos` and
   *   `std::find(that, that+count, at(i)) != that+count`; or
   *   `npos` if there is no such offset `i`.
   * \throw std::out_of_range when `pos > size()`
   */
  constexpr std::size_t find_first_of(
      const Char* that, std::size_t pos, std::size_t count) const
      noexcept(false) {
    return size_ == detail::fixedstring::checkOverflow(pos, size_)
        ? npos
        : detail::fixedstring::find_first_of_(data_, size_, that, pos, count);
  }

  /**
   * Finds the first occurrence of `ch` in this string.
   * \note Equivalent to `find_first_of(&ch, 0, 1)`
   */
  constexpr std::size_t find_first_of(Char ch) const noexcept {
    return find_first_of(ch, 0u);
  }

  /**
   * Finds the first occurrence of `ch` in this string,
   *   starting at offset `pos`.
   * \note Equivalent to `find_first_of(&ch, pos, 1)`
   */
  constexpr std::size_t find_first_of(Char ch, std::size_t pos) const
      noexcept(false) {
    using A = const Char[1u];
    return size_ == detail::fixedstring::checkOverflow(pos, size_)
        ? npos
        : detail::fixedstring::find_first_of_(data_, size_, A{ch}, pos, 1u);
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Finds the first occurrence of any character not in `that` in this string.
   * \note Equivalent to `find_first_not_of(that.data(), 0, that.size())`
   */
  template <std::size_t M>
  constexpr std::size_t find_first_not_of(
      const BasicFixedString<Char, M>& that) const noexcept {
    return find_first_not_of(that, 0u);
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Finds the first occurrence of any character not in `that` in this string.
   * \note Equivalent to `find_first_not_of(that.data(), 0, that.size())`
   */
  template <std::size_t M>
  constexpr std::size_t find_first_not_of(
      const BasicFixedString<Char, M>& that, std::size_t pos) const
      noexcept(false) {
    return size_ == detail::fixedstring::checkOverflow(pos, size_)
        ? npos
        : detail::fixedstring::find_first_not_of_(
              data_, size_, that.data_, pos, that.size_);
  }

  /**
   * Finds the first occurrence of any character not in the null-terminated
   *   character sequence pointed to by `that` in this string.
   * \note Equivalent to `find_first_not_of(that, 0, strlen(that))`
   */
  constexpr std::size_t find_first_not_of(const Char* that) const noexcept {
    return find_first_not_of(that, 0u, folly::constexpr_strlen(that));
  }

  /**
   * Finds the first occurrence of any character not in the null-terminated
   *   character sequence pointed to by `that` in this string,
   *   starting at offset `pos`
   * \note Equivalent to `find_first_not_of(that, pos, strlen(that))`
   */
  constexpr std::size_t find_first_not_of(
      const Char* that, std::size_t pos) const noexcept(false) {
    return find_first_not_of(that, pos, folly::constexpr_strlen(that));
  }

  /**
   * Finds the first occurrence of any character not in the first `count`
   *   characters in the buffer pointed to by `that` in this string, starting at
   *   offset `pos`.
   * \pre `pos <= size()`
   * \pre `that` points to a buffer containing at least `count` contiguous
   *   characters.
   * \return The smallest offset `i` such that `i >= pos` and
   *   `std::find(that, that+count, at(i)) == that+count`; or
   *   `npos` if there is no such offset `i`.
   * \throw std::out_of_range when `pos > size()`
   */
  constexpr std::size_t find_first_not_of(
      const Char* that, std::size_t pos, std::size_t count) const
      noexcept(false) {
    return size_ == detail::fixedstring::checkOverflow(pos, size_)
        ? npos
        : detail::fixedstring::find_first_not_of_(
              data_, size_, that, pos, count);
  }

  /**
   * Finds the first occurrence of any character other than `ch` in this string.
   * \note Equivalent to `find_first_not_of(&ch, 0, 1)`
   */
  constexpr std::size_t find_first_not_of(Char ch) const noexcept {
    return find_first_not_of(ch, 0u);
  }

  /**
   * Finds the first occurrence of any character other than `ch` in this string,
   *   starting at offset `pos`.
   * \note Equivalent to `find_first_not_of(&ch, pos, 1)`
   */
  constexpr std::size_t find_first_not_of(Char ch, std::size_t pos) const
      noexcept(false) {
    using A = const Char[1u];
    return 1u <= size_ - detail::fixedstring::checkOverflow(pos, size_)
        ? detail::fixedstring::find_first_not_of_(data_, size_, A{ch}, pos, 1u)
        : npos;
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Finds the last occurrence of any character in `that` in this string.
   * \note Equivalent to `find_last_of(that.data(), size(), that.size())`
   */
  template <std::size_t M>
  constexpr std::size_t find_last_of(
      const BasicFixedString<Char, M>& that) const noexcept {
    return find_last_of(that, size_);
  }

  /**
   * Finds the last occurrence of any character in `that` in this string,
   *   starting at offset `pos`
   * \note Equivalent to `find_last_of(that.data(), pos, that.size())`
   */
  template <std::size_t M>
  constexpr std::size_t find_last_of(
      const BasicFixedString<Char, M>& that, std::size_t pos) const
      noexcept(false) {
    return 0u == size_
        ? npos
        : detail::fixedstring::find_last_of_(
              data_,
              that.data_,
              folly::constexpr_min(
                  detail::fixedstring::checkOverflow(pos, size_), size_ - 1u),
              that.size_);
  }

  /**
   * Finds the last occurrence of any character in the null-terminated
   *   character sequence pointed to by `that` in this string.
   * \note Equivalent to `find_last_of(that, size(), strlen(that))`
   */
  constexpr std::size_t find_last_of(const Char* that) const noexcept {
    return find_last_of(that, size_, folly::constexpr_strlen(that));
  }

  /**
   * Finds the last occurrence of any character in the null-terminated
   *   character sequence pointed to by `that` in this string,
   *   starting at offset `pos`
   * \note Equivalent to `find_last_of(that, pos, strlen(that))`
   */
  constexpr std::size_t find_last_of(const Char* that, std::size_t pos) const
      noexcept(false) {
    return find_last_of(that, pos, folly::constexpr_strlen(that));
  }

  /**
   * Finds the last occurrence of any character in the first `count` characters
   *   in the buffer pointed to by `that` in this string, starting at offset
   *  `pos`.
   * \pre `pos <= size()`
   * \pre `that` points to a buffer containing at least `count` contiguous
   *   characters.
   * \return The largest offset `i` such that `i <= pos` and
   *   `i < size()` and `std::find(that, that+count, at(i)) != that+count`; or
   *   `npos` if there is no such offset `i`.
   * \throw std::out_of_range when `pos > size()`
   */
  constexpr std::size_t find_last_of(
      const Char* that, std::size_t pos, std::size_t count) const
      noexcept(false) {
    return 0u == size_
        ? npos
        : detail::fixedstring::find_last_of_(
              data_,
              that,
              folly::constexpr_min(
                  detail::fixedstring::checkOverflow(pos, size_), size_ - 1u),
              count);
  }

  /**
   * Finds the last occurrence of `ch` in this string.
   * \note Equivalent to `find_last_of(&ch, size(), 1)`
   */
  constexpr std::size_t find_last_of(Char ch) const noexcept {
    return find_last_of(ch, size_);
  }

  /**
   * Finds the last occurrence of `ch` in this string,
   *   starting at offset `pos`.
   * \note Equivalent to `find_last_of(&ch, pos, 1)`
   */
  constexpr std::size_t find_last_of(Char ch, std::size_t pos) const
      noexcept(false) {
    using A = const Char[1u];
    return 0u == size_
        ? npos
        : detail::fixedstring::find_last_of_(
              data_,
              A{ch},
              folly::constexpr_min(
                  detail::fixedstring::checkOverflow(pos, size_), size_ - 1u),
              1u);
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Finds the last occurrence of any character not in `that` in this string.
   * \note Equivalent to `find_last_not_of(that.data(), size(), that.size())`
   */
  template <std::size_t M>
  constexpr std::size_t find_last_not_of(
      const BasicFixedString<Char, M>& that) const noexcept {
    return find_last_not_of(that, size_);
  }

  /**
   * Finds the last occurrence of any character not in `that` in this string,
   *   starting at offset `pos`
   * \note Equivalent to `find_last_not_of(that.data(), pos, that.size())`
   */
  template <std::size_t M>
  constexpr std::size_t find_last_not_of(
      const BasicFixedString<Char, M>& that, std::size_t pos) const
      noexcept(false) {
    return 0u == size_
        ? npos
        : detail::fixedstring::find_last_not_of_(
              data_,
              that.data_,
              folly::constexpr_min(
                  detail::fixedstring::checkOverflow(pos, size_), size_ - 1u),
              that.size_);
  }

  /**
   * Finds the last occurrence of any character not in the null-terminated
   *   character sequence pointed to by `that` in this string.
   * \note Equivalent to `find_last_not_of(that, size(), strlen(that))`
   */
  constexpr std::size_t find_last_not_of(const Char* that) const noexcept {
    return find_last_not_of(that, size_, folly::constexpr_strlen(that));
  }

  /**
   * Finds the last occurrence of any character not in the null-terminated
   *   character sequence pointed to by `that` in this string,
   *   starting at offset `pos`
   * \note Equivalent to `find_last_not_of(that, pos, strlen(that))`
   */
  constexpr std::size_t find_last_not_of(
      const Char* that, std::size_t pos) const noexcept(false) {
    return find_last_not_of(that, pos, folly::constexpr_strlen(that));
  }

  /**
   * Finds the last occurrence of any character not in the first `count`
   *   characters in the buffer pointed to by `that` in this string, starting at
   *   offset `pos`.
   * \pre `pos <= size()`
   * \pre `that` points to a buffer containing at least `count` contiguous
   *   characters.
   * \return The largest offset `i` such that `i <= pos` and
   *   `i < size()` and `std::find(that, that+count, at(i)) == that+count`; or
   *   `npos` if there is no such offset `i`.
   * \throw std::out_of_range when `pos > size()`
   */
  constexpr std::size_t find_last_not_of(
      const Char* that, std::size_t pos, std::size_t count) const
      noexcept(false) {
    return 0u == size_
        ? npos
        : detail::fixedstring::find_last_not_of_(
              data_,
              that,
              folly::constexpr_min(
                  detail::fixedstring::checkOverflow(pos, size_), size_ - 1u),
              count);
  }

  /**
   * Finds the last occurrence of any character other than `ch` in this string.
   * \note Equivalent to `find_last_not_of(&ch, size(), 1)`
   */
  constexpr std::size_t find_last_not_of(Char ch) const noexcept {
    return find_last_not_of(ch, size_);
  }

  /**
   * Finds the last occurrence of any character other than `ch` in this string,
   *   starting at offset `pos`.
   * \note Equivalent to `find_last_not_of(&ch, pos, 1)`
   */
  constexpr std::size_t find_last_not_of(Char ch, std::size_t pos) const
      noexcept(false) {
    using A = const Char[1u];
    return 0u == size_
        ? npos
        : detail::fixedstring::find_last_not_of_(
              data_,
              A{ch},
              folly::constexpr_min(
                  detail::fixedstring::checkOverflow(pos, size_), size_ - 1u),
              1u);
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Asymmetric relational operators
   */
  friend constexpr bool operator==(
      const Char* a, const BasicFixedString& b) noexcept {
    return detail::fixedstring::equal_(
        a, folly::constexpr_strlen(a), b.data_, b.size_);
  }

  /**
   * \overload
   */
  friend constexpr bool operator==(
      const BasicFixedString& a, const Char* b) noexcept {
    return b == a;
  }

  /**
   * \overload
   */
  friend constexpr bool operator==(
      Range<const Char*> a, const BasicFixedString& b) noexcept {
    return detail::fixedstring::equal_(a.begin(), a.size(), b.data_, b.size_);
  }

  /**
   * \overload
   */
  friend constexpr bool operator==(
      const BasicFixedString& a, Range<const Char*> b) noexcept {
    return b == a;
  }

  friend constexpr bool operator!=(
      const Char* a, const BasicFixedString& b) noexcept {
    return !(a == b);
  }

  /**
   * \overload
   */
  friend constexpr bool operator!=(
      const BasicFixedString& a, const Char* b) noexcept {
    return !(b == a);
  }

  /**
   * \overload
   */
  friend constexpr bool operator!=(
      Range<const Char*> a, const BasicFixedString& b) noexcept {
    return !(a == b);
  }

  /**
   * \overload
   */
  friend constexpr bool operator!=(
      const BasicFixedString& a, Range<const Char*> b) noexcept {
    return !(a == b);
  }

  friend constexpr bool operator<(
      const Char* a, const BasicFixedString& b) noexcept {
    return ordering::lt ==
        detail::fixedstring::compare_(
               a, 0u, folly::constexpr_strlen(a), b.data_, 0u, b.size_);
  }

  /**
   * \overload
   */
  friend constexpr bool operator<(
      const BasicFixedString& a, const Char* b) noexcept {
    return ordering::lt ==
        detail::fixedstring::compare_(
               a.data_, 0u, a.size_, b, 0u, folly::constexpr_strlen(b));
  }

  /**
   * \overload
   */
  friend constexpr bool operator<(
      Range<const Char*> a, const BasicFixedString& b) noexcept {
    return ordering::lt ==
        detail::fixedstring::compare_(
               a.begin(), 0u, a.size(), b.data_, 0u, b.size_);
  }

  /**
   * \overload
   */
  friend constexpr bool operator<(
      const BasicFixedString& a, Range<const Char*> b) noexcept {
    return ordering::lt ==
        detail::fixedstring::compare_(
               a.data_, 0u, a.size_, b.begin(), 0u, b.size());
  }

  friend constexpr bool operator>(
      const Char* a, const BasicFixedString& b) noexcept {
    return b < a;
  }

  /**
   * \overload
   */
  friend constexpr bool operator>(
      const BasicFixedString& a, const Char* b) noexcept {
    return b < a;
  }

  /**
   * \overload
   */
  friend constexpr bool operator>(
      Range<const Char*> a, const BasicFixedString& b) noexcept {
    return b < a;
  }

  /**
   * \overload
   */
  friend constexpr bool operator>(
      const BasicFixedString& a, Range<const Char*> b) noexcept {
    return b < a;
  }

  friend constexpr bool operator<=(
      const Char* a, const BasicFixedString& b) noexcept {
    return !(b < a);
  }

  /**
   * \overload
   */
  friend constexpr bool operator<=(
      const BasicFixedString& a, const Char* b) noexcept {
    return !(b < a);
  }

  /**
   * \overload
   */
  friend constexpr bool operator<=(
      Range<const Char*> const& a, const BasicFixedString& b) noexcept {
    return !(b < a);
  }

  /**
   * \overload
   */
  friend constexpr bool operator<=(
      const BasicFixedString& a, Range<const Char*> b) noexcept {
    return !(b < a);
  }

  friend constexpr bool operator>=(
      const Char* a, const BasicFixedString& b) noexcept {
    return !(a < b);
  }

  /**
   * \overload
   */
  friend constexpr bool operator>=(
      const BasicFixedString& a, const Char* b) noexcept {
    return !(a < b);
  }

  /**
   * \overload
   */
  friend constexpr bool operator>=(
      Range<const Char*> a, const BasicFixedString& b) noexcept {
    return !(a < b);
  }

  /**
   * \overload
   */
  friend constexpr bool operator>=(
      const BasicFixedString& a, Range<const Char*> const& b) noexcept {
    return !(a < b);
  }

  /** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
   * Asymmetric concatenation
   */
  template <std::size_t M>
  friend constexpr BasicFixedString<Char, N + M - 1u> operator+(
      const Char (&a)[M], const BasicFixedString& b) noexcept {
    return detail::fixedstring::Helper::concat_<Char>(
        detail::fixedstring::checkNullTerminated(a),
        M - 1u,
        b.data_,
        b.size_,
        std::make_index_sequence<N + M - 1u>{});
  }

  /**
   * \overload
   */
  template <std::size_t M>
  friend constexpr BasicFixedString<Char, N + M - 1u> operator+(
      const BasicFixedString& a, const Char (&b)[M]) noexcept {
    return detail::fixedstring::Helper::concat_<Char>(
        a.data_,
        a.size_,
        detail::fixedstring::checkNullTerminated(b),
        M - 1u,
        std::make_index_sequence<N + M - 1u>{});
  }

  /**
   * \overload
   */
  friend constexpr BasicFixedString<Char, N + 1u> operator+(
      Char a, const BasicFixedString& b) noexcept {
    using A = const Char[2u];
    return detail::fixedstring::Helper::concat_<Char>(
        A{a, Char(0)},
        1u,
        b.data_,
        b.size_,
        std::make_index_sequence<N + 1u>{});
  }

  /**
   * \overload
   */
  friend constexpr BasicFixedString<Char, N + 1u> operator+(
      const BasicFixedString& a, Char b) noexcept {
    using A = const Char[2u];
    return detail::fixedstring::Helper::concat_<Char>(
        a.data_,
        a.size_,
        A{b, Char(0)},
        1u,
        std::make_index_sequence<N + 1u>{});
  }
};

template <class C, std::size_t N>
inline std::basic_ostream<C>& operator<<(
    std::basic_ostream<C>& os, const BasicFixedString<C, N>& string) {
  using StreamSize = decltype(os.width());
  os.write(string.begin(), static_cast<StreamSize>(string.size()));
  return os;
}

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 * Symmetric relational operators
 */
template <class Char, std::size_t A, std::size_t B>
constexpr bool operator==(
    const BasicFixedString<Char, A>& a,
    const BasicFixedString<Char, B>& b) noexcept {
  return detail::fixedstring::equal_(
      detail::fixedstring::Helper::data_(a),
      a.size(),
      detail::fixedstring::Helper::data_(b),
      b.size());
}

template <class Char, std::size_t A, std::size_t B>
constexpr bool operator!=(
    const BasicFixedString<Char, A>& a, const BasicFixedString<Char, B>& b) {
  return !(a == b);
}

template <class Char, std::size_t A, std::size_t B>
constexpr bool operator<(
    const BasicFixedString<Char, A>& a,
    const BasicFixedString<Char, B>& b) noexcept {
  return ordering::lt ==
      detail::fixedstring::compare_(
             detail::fixedstring::Helper::data_(a),
             0u,
             a.size(),
             detail::fixedstring::Helper::data_(b),
             0u,
             b.size());
}

template <class Char, std::size_t A, std::size_t B>
constexpr bool operator>(
    const BasicFixedString<Char, A>& a,
    const BasicFixedString<Char, B>& b) noexcept {
  return b < a;
}

template <class Char, std::size_t A, std::size_t B>
constexpr bool operator<=(
    const BasicFixedString<Char, A>& a,
    const BasicFixedString<Char, B>& b) noexcept {
  return !(b < a);
}

template <class Char, std::size_t A, std::size_t B>
constexpr bool operator>=(
    const BasicFixedString<Char, A>& a,
    const BasicFixedString<Char, B>& b) noexcept {
  return !(a < b);
}

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 * Symmetric concatenation
 */
template <class Char, std::size_t N, std::size_t M>
constexpr BasicFixedString<Char, N + M> operator+(
    const BasicFixedString<Char, N>& a,
    const BasicFixedString<Char, M>& b) noexcept {
  return detail::fixedstring::Helper::concat_<Char>(
      detail::fixedstring::Helper::data_(a),
      a.size(),
      detail::fixedstring::Helper::data_(b),
      b.size(),
      std::make_index_sequence<N + M>{});
}

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 * Construct a `BasicFixedString` object from a null-terminated array of
 * characters. The capacity and size of the string will be equal to one less
 * than the size of the array.
 * \pre `a` contains no embedded null characters.
 * \pre `a[N-1] == Char(0)`
 * \post For a returned string `s`, `s[i]==a[i]` for every `i` in [`0`,`N-1`].
 */
template <class Char, std::size_t N>
constexpr BasicFixedString<Char, N - 1u> makeFixedString(
    const Char (&a)[N]) noexcept {
  return {a};
}

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 * Swap function
 */
template <class Char, std::size_t N>
constexpr void swap(
    BasicFixedString<Char, N>& a, BasicFixedString<Char, N>& b) noexcept {
  a.swap(b);
}

inline namespace literals {
inline namespace string_literals {
inline namespace {
// "const std::size_t&" is so that folly::npos has the same address in every
// translation unit. This is to avoid potential violations of the ODR.
constexpr const std::size_t& npos = detail::fixedstring::FixedStringBase::npos;
} // namespace

#if defined(__GNUC__) && !defined(__ICC)
/* #pragma GCC diagnostic push */
/* #pragma GCC diagnostic ignored "-Wpragmas" */
/* #pragma GCC diagnostic ignored "-Wgnu-string-literal-operator-template" */

/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** *
 * User-defined literals for creating FixedString objects from string literals
 * on the compilers that support it.
 *
 * \par Example:
 * \par
 * \code
 * using namespace folly::string_literals;
 * constexpr auto hello = "hello world!"_fs;
 * \endcode
 *
 * \note This requires a GNU compiler extension
 *   (-Wgnu-string-literal-operator-template) supported by clang and gcc,
 *   proposed for standardization in
 *   <http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0424r0.pdf>.
 *   \par
 *   For portable code, prefer the suffixes `_fs4`, `_fs8`, `_fs16`, `_fs32`,
 *   `_fs64`, and `_fs128` for creating instances of types `FixedString<4>`,
 *   `FixedString<8>`, `FixedString<16>`, etc.
 */
template <class Char, Char... Cs>
constexpr BasicFixedString<Char, sizeof...(Cs)> operator"" _fs() noexcept {
  const Char a[] = {Cs..., Char(0)};
  return {+a, sizeof...(Cs)};
}

/* #pragma GCC diagnostic pop */
#endif

#define FOLLY_DEFINE_FIXED_STRING_UDL(N)                     \
  constexpr FixedString<N> operator"" _fs##N(                \
      const char* that, std::size_t count) noexcept(false) { \
    return {that, count};                                    \
  }                                                          \
/**/

// Define UDLs _fs4, _fs8, _fs16, etc for FixedString<[4, 8, 16, ...]>
FOLLY_DEFINE_FIXED_STRING_UDL(4)
FOLLY_DEFINE_FIXED_STRING_UDL(8)
FOLLY_DEFINE_FIXED_STRING_UDL(16)
FOLLY_DEFINE_FIXED_STRING_UDL(32)
FOLLY_DEFINE_FIXED_STRING_UDL(64)
FOLLY_DEFINE_FIXED_STRING_UDL(128)

#undef FOLLY_DEFINE_FIXED_STRING_UDL
} // namespace string_literals
} // namespace literals

// TODO:
// // numeric conversions:
// template <std::size_t N>
// constexpr int stoi(const FixedString<N>& str, int base = 10);
// template <std::size_t N>
// constexpr unsigned stou(const FixedString<N>& str, int base = 10);
// template <std::size_t N>
// constexpr long stol(const FixedString<N>& str, int base = 10);
// template <std::size_t N>
// constexpr unsigned long stoul(const FixedString<N>& str, int base = 10;
// template <std::size_t N>
// constexpr long long stoll(const FixedString<N>& str, int base = 10);
// template <std::size_t N>
// constexpr unsigned long long stoull(const FixedString<N>& str,
// int base = 10);
// template <std::size_t N>
// constexpr float stof(const FixedString<N>& str);
// template <std::size_t N>
// constexpr double stod(const FixedString<N>& str);
// template <std::size_t N>
// constexpr long double stold(const FixedString<N>& str);
// template <int val>
// constexpr FixedString</*...*/> to_fixed_string_i() noexcept;
// template <unsigned val>
// constexpr FixedString</*...*/> to_fixed_string_u() noexcept;
// template <long val>
// constexpr FixedString</*...*/> to_fixed_string_l() noexcept;
// template <unsigned long val>
// constexpr FixedString</*...*/> to_fixed_string_ul() noexcept;
// template <long long val>
// constexpr FixedString</*...*/> to_fixed_string_ll() noexcept
// template <unsigned long long val>
// constexpr FixedString</*...*/> to_fixed_string_ull() noexcept;
} // namespace folly
/* ^^^^ folly/FixedString.h ^^^^ */


/* vvvv folly/test/FixedStringTest.cpp vvvv */
/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//
// Author: eniebler@fb.com

/* #include <folly/FixedString.h> */

/* #include <folly/portability/GTest.h> */

#define FS(x) ::folly::makeFixedString(x)
using namespace folly::string_literals;

TEST(FixedStringExamples, Examples) {
  // Example from the docs:
  using namespace folly;
  constexpr auto hello = makeFixedString("hello"); // a FixedString<5>
  constexpr auto world = makeFixedString("world"); // another FixedString<5>
  constexpr auto hello_world = hello + ' ' + world + '!';
  static_assert(hello_world == "hello world!", "w00t");
  EXPECT_STREQ("hello world!", hello_world.c_str());

  FixedString<10> test{"****"};
  test.replace(1, 2, "!!!!");
  EXPECT_STREQ("*!!!!*", test.c_str());
  static_assert(makeFixedString("****").creplace(1, 2, "!!!!") == "*!!!!*", "");
}

TEST(FixedStringCtorTest, Default) {
  constexpr folly::FixedString<42> s{};
  static_assert(s[0] == '\0', "");
  static_assert(s.size() == 0u, "");

  constexpr auto s2 = s;
  static_assert(s2[0] == '\0', "");
  static_assert(s2.size() == 0u, "");
}

TEST(FixedStringCtorTest, FromLiterals) {
  constexpr folly::FixedString<42> s{"hello world"};
  static_assert(s[0] == 'h', "");
  constexpr folly::FixedString<11> s2{"hello world"};
  static_assert(s2[0] == 'h', "");
  static_assert(s2[10] == 'd', "");
  static_assert(s2[11] == '\0', "");

  // Does not compile, hurray! :-)
  // constexpr char a[1] = {'a'};
  // constexpr folly::FixedString<10> s3(a);
}

TEST(FixedStringCtorTest, FromPtrAndLength) {
  constexpr folly::FixedString<11> s{"hello world", 11};
  static_assert(s[0] == 'h', "");
  static_assert(s[10] == 'd', "");
  static_assert(s[11] == '\0', "");
  static_assert(s.size() == 11u, "");

  constexpr folly::FixedString<5> s2{"hello world", 5};
  static_assert(s2[0] == 'h', "");
  static_assert(s2[4] == 'o', "");
  static_assert(s2[5] == '\0', "");
  static_assert(s2.size() == 5u, "");

  constexpr folly::FixedString<20> s3{"hello world", 5};
  static_assert(s2[0] == 'h', "");
  static_assert(s2[4] == 'o', "");
  static_assert(s2[5] == '\0', "");
  static_assert(s2.size() == 5u, "");

  static_assert("hello" == s3, "");
  static_assert(s3 == "hello", "");
  static_assert(s3 == s2, "");
  static_assert("hell" != s3, "");
  static_assert(s3 != "helloooo", "");
  static_assert(!(s3 != s2), "");
}

TEST(FixedStringCtorTest, FromStringAndOffset) {
  constexpr folly::FixedString<11> s{"hello world"};
  constexpr folly::FixedString<5> s2{s, 6u, npos};
  static_assert(s2 == "world", "");
  constexpr folly::FixedString<0> s3{s, 11u, npos};
  static_assert(s3 == "", "");
  // Out of bounds offset, does not compile
  // constexpr folly::FixedString<0> s4{s, 12};
}

TEST(FixedStringCtorTest, FromStringOffsetAndCount) {
  constexpr folly::FixedString<11> s{"hello world"};
  constexpr folly::FixedString<4> s2{s, 6u, 4u};
  static_assert(s2 == "worl", "");
  constexpr folly::FixedString<5> s3{s, 6u, 5u};
  static_assert(s3 == "world", "");
  // Out of bounds count, does not compile:
  // constexpr folly::FixedString<5> s4{s, 6, 6};
}

TEST(FixedStringCtorTest, FromInitializerList) {
  constexpr folly::FixedString<11> s{
      'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};
  static_assert(s == "hello world", "");
  // Out of bounds count, does not compile:
  // constexpr folly::FixedString<10> s{
  //     {'h','e','l','l','o',' ','w','o','r','l','d'}};
}

TEST(FixedStringCtorTest, FromUDL) {
  using namespace folly::literals;
#if defined(__GNUC__)
  constexpr auto x = "hello"_fs;
  static_assert(
      std::is_same<decltype(x), const folly::FixedString<5>>::value, "");
  static_assert(x[0] == 'h', "");
  static_assert(x[1] == 'e', "");
  static_assert(x[2] == 'l', "");
  static_assert(x[3] == 'l', "");
  static_assert(x[4] == 'o', "");
  static_assert(x[5] == '\0', "");
  static_assert(x.size() == 5u, "");
#endif

  constexpr auto y = "goodbye"_fs8;
  static_assert(
      std::is_same<decltype(y), const folly::FixedString<8>>::value, "");
  static_assert(y.size() == 7u, "");
  static_assert(y == "goodbye", "");

  constexpr auto z = "now is the time for all good llamas"_fs64;
  static_assert(
      std::is_same<decltype(z), const folly::FixedString<64>>::value, "");
  static_assert(z.size() == 35u, "");
  static_assert(z == "now is the time for all good llamas", "");
}

TEST(FixedStringConcatTest, FromStringAndLiteral) {
  constexpr folly::FixedString<42> s{"hello world"};
  constexpr auto res = s + "!!!";
  static_assert(res.size() == 14u, "");
  static_assert(res == "hello world!!!", "");
}

TEST(FixedStringConcatTest, FromTwoStrings) {
  constexpr folly::FixedString<42> s{"hello world"};
  constexpr auto res = s + "!!!";
  static_assert(res.size() == 14u, "");
  static_assert(res == "hello world!!!", "");
}

constexpr folly::FixedString<20> constexpr_swap_test() {
  folly::FixedString<10> tmp1{"hello"}, tmp2{"world!"};
  tmp2.swap(tmp1);
  return tmp1 + tmp2;
}

TEST(FixedStringSwapTest, ConstexprSwap) {
  static_assert(constexpr_swap_test() == "world!hello", "");
}

TEST(FixedStringSwapTest, RuntimeSwap) {
  folly::FixedString<10> tmp1{"hello"}, tmp2{"world!"};
  tmp2.swap(tmp1);
  EXPECT_STREQ((tmp1 + tmp2).c_str(), "world!hello");
}

constexpr folly::FixedString<10> constexpr_assign_string_test_1() {
  folly::FixedString<10> tmp1, tmp2{"world!"};
  tmp1 = tmp2;
  return tmp1;
}
constexpr folly::FixedString<10> constexpr_assign_string_test_2() {
  folly::FixedString<10> tmp{"aaaaaaaaaa"};
  tmp.assign("hello"_fs8);
  return tmp;
}
constexpr folly::FixedString<10> constexpr_assign_string_test_3() {
  folly::FixedString<10> tmp{"aaaaaaaaaa"};
  tmp.assign("goodbye"_fs8, 3u, 2u);
  return tmp;
}
constexpr folly::FixedString<10> constexpr_assign_string_test_4() {
  folly::FixedString<10> tmp{"aaaaaaaaaa"};
  tmp.assign("goodbye"_fs8, 3u, npos);
  return tmp;
}

TEST(FixedStringAssignTest, ConstexprAssignString) {
  static_assert(constexpr_assign_string_test_1() == "world!", "");
  static_assert(constexpr_assign_string_test_2() == "hello", "");
  static_assert(constexpr_assign_string_test_3() == "db", "");
  static_assert(constexpr_assign_string_test_4() == "dbye", "");
}

TEST(FixedStringAssignTest, RuntimeAssignString) {
  folly::FixedString<10> tmp1, tmp2{"world!"};
  tmp1 = tmp2;
  EXPECT_STREQ(tmp1.c_str(), "world!");
  tmp1.assign("goodbye"_fs8);
  EXPECT_STREQ("goodbye", tmp1.c_str());
  tmp1.assign("goodbye"_fs8, 3u, npos);
  EXPECT_STREQ("dbye", tmp1.c_str());
  tmp1.assign("goodbye"_fs8, 3u, 3u);
  EXPECT_STREQ("dby", tmp1.c_str());
}

constexpr folly::FixedString<10> constexpr_assign_literal_test_1() {
  folly::FixedString<10> tmp{"aaaaaaaaaa"};
  tmp = "hello";
  // Not null-terminated, does not compile:
  // using C = const char[1];
  // tmp = C{'a'};
  return tmp;
}
constexpr folly::FixedString<10> constexpr_assign_literal_test_2() {
  folly::FixedString<10> tmp{"aaaaaaaaaa"};
  tmp.assign("hello");
  return tmp;
}
constexpr folly::FixedString<10> constexpr_assign_literal_test_3() {
  folly::FixedString<10> tmp{"aaaaaaaaaa"};
  tmp.assign("goodbye", 4u);
  return tmp;
}

TEST(FixedStringAssignTest, ConstexprAssignLiteral) {
  static_assert(constexpr_assign_literal_test_1() == "hello", "");
  static_assert(constexpr_assign_literal_test_2() == "hello", "");
  static_assert(constexpr_assign_literal_test_3() == "good", "");
}

TEST(FixedStringAssignTest, RuntimeAssignLiteral) {
  folly::FixedString<10> tmp{"aaaaaaaaaa"};
  tmp = "hello";
  EXPECT_STREQ("hello", tmp.c_str());
  tmp.assign("goodbye");
  EXPECT_STREQ("goodbye", tmp.c_str());
  tmp.assign("goodbye", 4u);
  EXPECT_STREQ("good", tmp.c_str());
}

TEST(FixedStringIndexTest, Index) {
  constexpr folly::FixedString<11> digits{"0123456789"};
  static_assert(digits[0] == '0', "");
  static_assert(digits[1] == '1', "");
  static_assert(digits[2] == '2', "");
  static_assert(digits[9] == '9', "");
  static_assert(digits[10] == '\0', "");
#ifdef NDEBUG
  // This should be allowed and work in constexpr mode since the internal array
  // is actually big enough and op[] does no parameter validation:
  static_assert(digits[11] == '\0', "");
#endif

  static_assert(digits.at(0) == '0', "");
  static_assert(digits.at(1) == '1', "");
  static_assert(digits.at(2) == '2', "");
  static_assert(digits.at(9) == '9', "");
  static_assert(digits.at(10) == '\0', "");
  EXPECT_THROW(digits.at(11), std::out_of_range);
}

TEST(FixedStringCompareTest, Compare) {
  constexpr folly::FixedString<10> tmp1{"aaaaaaaaaa"};
  constexpr folly::FixedString<12> tmp2{"aaaaaaaaaba"};
  static_assert(-1 == tmp1.compare(tmp2), "");
  static_assert(1 == tmp2.compare(tmp1), "");
  static_assert(0 == tmp2.compare(tmp2), "");
  static_assert(tmp1 < tmp2, "");
  static_assert(tmp1 <= tmp2, "");
  static_assert(tmp2 > tmp1, "");
  static_assert(tmp2 >= tmp1, "");
  static_assert(tmp2 == tmp2, "");
  static_assert(tmp2 <= tmp2, "");
  static_assert(tmp2 >= tmp2, "");
  static_assert(!(tmp2 < tmp2), "");
  static_assert(!(tmp2 > tmp2), "");

  constexpr folly::FixedString<10> tmp3{"aaa"};
  constexpr folly::FixedString<12> tmp4{"aaaa"};
  static_assert(-1 == tmp3.compare(tmp4), "");
  static_assert(1 == tmp4.compare(tmp3), "");
  static_assert(tmp3 < tmp4, "");
  static_assert(tmp3 <= tmp4, "");
  static_assert(tmp4 > tmp3, "");
  static_assert(tmp4 >= tmp3, "");
  static_assert(tmp3 < "aaaa", "");
  static_assert(tmp3 <= "aaaa", "");
  static_assert(!(tmp3 == tmp4), "");
  static_assert(tmp3 != tmp4, "");
  static_assert("aaaa" > tmp3, "");
  static_assert("aaaa" >= tmp3, "");
  static_assert("aaaa" != tmp3, "");
  static_assert("aaa" == tmp3, "");
  static_assert(tmp3 != "aaaa", "");
  static_assert(tmp3 == "aaa", "");
}

TEST(FixedStringCompareTest, CompareStdString) {
  constexpr folly::FixedString<10> tmp1{"aaaaaaaaaa"};
  std::string const tmp2{"aaaaaaaaaba"};
  EXPECT_EQ(-1, tmp1.compare(tmp2));
  // These are specifically testing the operators, and so we can't rely
  // on whever the implementation details of EXPECT_<OP> might be.
  EXPECT_FALSE(tmp1 == tmp2);
  EXPECT_FALSE(tmp2 == tmp1);
  EXPECT_TRUE(tmp1 != tmp2);
  EXPECT_TRUE(tmp2 != tmp1);
  EXPECT_TRUE(tmp1 < tmp2);
  EXPECT_FALSE(tmp2 < tmp1);
  EXPECT_TRUE(tmp1 <= tmp2);
  EXPECT_FALSE(tmp2 <= tmp1);
  EXPECT_FALSE(tmp1 > tmp2);
  EXPECT_TRUE(tmp2 > tmp1);
  EXPECT_FALSE(tmp1 >= tmp2);
  EXPECT_TRUE(tmp2 >= tmp1);
}

constexpr folly::FixedString<20> constexpr_append_string_test() {
  folly::FixedString<20> a{"hello"}, b{"X world!"};
  a.append(1u, ' ');
  a.append(b, 2u, 5u);
  a.append(b, 7u, 1u);
  return a;
}

TEST(FixedStringAssignTest, ConstexprAppendString) {
  static_assert(constexpr_append_string_test() == "hello world!", "");
}

TEST(FixedStringAssignTest, RuntimeAppendString) {
  folly::FixedString<20> a{"hello"}, b{"X world!"};
  a.append(1u, ' ');
  a.append(b, 2u, 5u);
  a.append(b, 7u, 1u);
  EXPECT_STREQ("hello world!", a.c_str());
}

constexpr folly::FixedString<20> constexpr_append_literal_test() {
  folly::FixedString<20> a{"hello"};
  a.append(1u, ' ');
  a.append("world foo bar baz", 5u);
  a.append("!");
  return a;
}

TEST(FixedStringAssignTest, ConstexprAppendLiteral) {
  static_assert(constexpr_append_literal_test() == "hello world!", "");
}

TEST(FixedStringAssignTest, RuntimeAppendLiteral) {
  folly::FixedString<20> a{"hello"};
  a.append(1u, ' ');
  constexpr char s[] = "X world!";
  a.append(&s[2u], 5u);
  a.append(&s[7u]);
  EXPECT_STREQ("hello world!", a.c_str());
}

TEST(FixedStringCAppendTest, CAppendString) {
  constexpr folly::FixedString<10> a{"hello"}, b{"X world!"};
  constexpr auto tmp1 = a.cappend(' ');
  constexpr auto tmp2 = tmp1.cappend(b, 2u, 5u);
  constexpr auto tmp3 = tmp2.cappend(b, 7u, 1u);
  static_assert(tmp3 == "hello world!", "");
}

TEST(FixedStringCAppendTest, CAppendLiteral) {
  constexpr folly::FixedString<10> a{"hello"};
  constexpr auto tmp1 = a.cappend(' ');
  constexpr auto tmp2 = tmp1.cappend("X world!", 2u, 5u);
  constexpr auto tmp3 = tmp2.cappend("X world!", 7u, 1u);
  static_assert(tmp3 == "hello world!", "");
}

constexpr folly::FixedString<10> constexpr_replace_string_test() {
  folly::FixedString<10> tmp{"abcdefghij"};
  tmp.replace(1, 5, FS("XX"));
  return tmp;
}

TEST(FixedStringReplaceTest, ConstexprReplaceString) {
  static_assert(constexpr_replace_string_test().size() == 7u, "");
  static_assert(constexpr_replace_string_test() == "aXXghij", "");
}

TEST(FixedStringReplaceTest, RuntimeReplaceString) {
  folly::FixedString<10> tmp{"abcdefghij"};
  tmp.replace(1, 5, FS("XX"));
  EXPECT_EQ(7u, tmp.size());
  EXPECT_STREQ("aXXghij", tmp.c_str());
}

TEST(FixedStringEraseTest, RuntimeEraseTest) {
  auto x = FS("abcdefghijklmnopqrstuvwxyz"), y = x;
  x.erase(x.size());
  EXPECT_EQ(26u, x.size());
  EXPECT_STREQ(y.c_str(), x.c_str());
  x.erase(25u).erase(24u);
  EXPECT_EQ(24u, x.size());
  EXPECT_STREQ("abcdefghijklmnopqrstuvwx", x.c_str());
  x.erase(1u, x.size() - 2u);
  EXPECT_EQ(2u, x.size());
  EXPECT_STREQ("ax", x.c_str());
}

TEST(FixedStringEraseTest, CEraseTest) {
  constexpr auto x = FS("abcdefghijklmnopqrstuvwxyz"), y = x;
  constexpr auto tmp0 = x.cerase(x.size());
  static_assert(26u == tmp0.size(), "");
  static_assert(y == tmp0, "");
  constexpr auto tmp1 = tmp0.cerase(25u).cerase(24u);
  static_assert(24u == tmp1.size(), "");
  static_assert("abcdefghijklmnopqrstuvwx" == tmp1, "");
  constexpr auto tmp2 = tmp1.cerase(1u, tmp1.size() - 2u);
  static_assert(2u == tmp2.size(), "");
  static_assert("ax" == tmp2, "");
  constexpr auto tmp3 = tmp2.cerase();
  static_assert("" == tmp3, "");
}

TEST(FixedStringFindTest, FindString) {
  constexpr folly::FixedString<10> tmp{"hijdefghij"};
  static_assert(tmp.find(FS("hij")) == 0u, "");
  static_assert(tmp.find(FS("hij"), 1u) == 7u, "");
  static_assert(tmp.find(FS("hijdefghij")) == 0u, "");
  static_assert(tmp.find(FS("")) == 0u, "");
}

TEST(FixedStringFindTest, FindLiteral) {
  constexpr folly::FixedString<10> tmp{"hijdefghij"};
  static_assert(tmp.find("hij") == 0u, "");
  static_assert(tmp.find("hij", 1u) == 7u, "");
  static_assert(tmp.find("hijdefghij") == 0u, "");
}

TEST(FixedStringReverseFindTest, FindChar) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  static_assert(tmp.find('s') == 3u, "");
  static_assert(tmp.find('s', 9u) == 10u, "");
  static_assert(tmp.find('s', 10u) == 10u, "");
  static_assert(tmp.find('s', 11u) == tmp.npos, "");
}

TEST(FixedStringReverseFindTest, ReverseFindString) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  static_assert(tmp.rfind(FS("is")) == 5u, "");
  static_assert(tmp.rfind(FS("is"), 4u) == 2u, "");
  static_assert(tmp.rfind(FS("This is a string")) == 0u, "");
  static_assert(tmp.rfind(FS("This is a string!")) == tmp.npos, "");
  static_assert(tmp.rfind(FS("")) == 16u, "");
}

TEST(FixedStringReverseFindTest, ReverseFindLiteral) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  static_assert(tmp.rfind("is") == 5u, "");
  static_assert(tmp.rfind("is", 4u) == 2u, "");
  static_assert(tmp.rfind("This is a string") == 0u, "");
  static_assert(tmp.rfind("This is a string!") == tmp.npos, "");
  static_assert(tmp.rfind("") == 16u, "");
}

TEST(FixedStringReverseFindTest, ReverseFindChar) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  static_assert(tmp.rfind('s') == 10u, "");
  static_assert(tmp.rfind('s', 5u) == 3u, "");
  static_assert(tmp.rfind('s', 3u) == 3u, "");
  static_assert(tmp.rfind('s', 2u) == tmp.npos, "");
}

TEST(FixedStringFindFirstOfTest, FindFirstOfString) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  static_assert(tmp.find_first_of(FS("hi")) == 1u, "");
  static_assert(tmp.find_first_of(FS("xi")) == 2u, "");
  static_assert(tmp.find_first_of(FS("xi"), 6u) == 13u, "");
  static_assert(tmp.find_first_of(FS("xz")) == tmp.npos, "");
  static_assert(FS("a").find_first_of(FS("cba")) == 0u, "");
  static_assert(FS("").find_first_of(FS("cba")) == tmp.npos, "");
  static_assert(FS("a").find_first_of(FS("")) == tmp.npos, "");
  static_assert(FS("").find_first_of(FS("")) == tmp.npos, "");
}

TEST(FixedStringFindFirstOfTest, FindFirstOfLiteral) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  static_assert(tmp.find_first_of("hi") == 1u, "");
  static_assert(tmp.find_first_of("xi") == 2u, "");
  static_assert(tmp.find_first_of("xi", 6u) == 13u, "");
  static_assert(tmp.find_first_of("xis", 6u, 2u) == 13u, "");
  static_assert(tmp.find_first_of("xz") == tmp.npos, "");
  static_assert(FS("a").find_first_of("cba") == 0u, "");
  static_assert(FS("").find_first_of("cba") == tmp.npos, "");
  static_assert(FS("a").find_first_of("") == tmp.npos, "");
  static_assert(FS("").find_first_of("") == tmp.npos, "");
}

TEST(FixedStringFindFirstOfTest, FindFirstOfChar) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  static_assert(tmp.find_first_of('h') == 1u, "");
  static_assert(tmp.find_first_of('i') == 2u, "");
  static_assert(tmp.find_first_of('i', 6u) == 13u, "");
  static_assert(tmp.find_first_of('x') == tmp.npos, "");
  static_assert(FS("a").find_first_of('a') == 0u, "");
  static_assert(FS("").find_first_of('a') == tmp.npos, "");
}

TEST(FixedStringFindFirstNotOfTest, FindFirstNotOfString) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  static_assert(tmp.find_first_not_of(FS("Ti")) == 1u, "");
  static_assert(tmp.find_first_not_of(FS("hT")) == 2u, "");
  static_assert(tmp.find_first_not_of(FS("s atr"), 6u) == 13u, "");
  static_assert(tmp.find_first_not_of(FS("This atrng")) == tmp.npos, "");
  static_assert(FS("a").find_first_not_of(FS("X")) == 0u, "");
  static_assert(FS("").find_first_not_of(FS("cba")) == tmp.npos, "");
  static_assert(FS("a").find_first_not_of(FS("")) == 0u, "");
  static_assert(FS("").find_first_not_of(FS("")) == tmp.npos, "");
}

TEST(FixedStringFindFirstNotOfTest, FindFirstNotOfLiteral) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  static_assert(tmp.find_first_not_of("Ti") == 1u, "");
  static_assert(tmp.find_first_not_of("hT") == 2u, "");
  static_assert(tmp.find_first_not_of("s atr", 6u) == 13u, "");
  static_assert(tmp.find_first_not_of("This atrng") == tmp.npos, "");
  static_assert(FS("a").find_first_not_of("X") == 0u, "");
  static_assert(FS("").find_first_not_of("cba") == tmp.npos, "");
  static_assert(FS("a").find_first_not_of("") == 0u, "");
  static_assert(FS("").find_first_not_of("") == tmp.npos, "");
}

TEST(FixedStringFindFirstNotOfTest, FindFirstNotOfChar) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  static_assert(tmp.find_first_not_of('T') == 1u, "");
  static_assert(tmp.find_first_not_of('i') == 0u, "");
  static_assert(tmp.find_first_not_of('x', 6u) == 6u, "");
  static_assert(tmp.find_first_not_of('s', 6u) == 7u, "");
  static_assert(FS("a").find_first_not_of('a') == tmp.npos, "");
  static_assert(FS("").find_first_not_of('a') == tmp.npos, "");
}

TEST(FixedStringFindLastOfTest, FindLastOfString) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  static_assert(tmp.find_last_of(FS("hi")) == 13u, "");
  static_assert(tmp.find_last_of(FS("xh")) == 1u, "");
  static_assert(tmp.find_last_of(FS("xi"), 6u) == 5u, "");
  static_assert(tmp.find_last_of(FS("xz")) == tmp.npos, "");
  static_assert(FS("a").find_last_of(FS("cba")) == 0u, "");
  static_assert(FS("").find_last_of(FS("cba")) == tmp.npos, "");
  static_assert(FS("a").find_last_of(FS("")) == tmp.npos, "");
  static_assert(FS("").find_last_of(FS("")) == tmp.npos, "");
}

TEST(FixedStringFindLastOfTest, FindLastOfLiteral) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  static_assert(tmp.find_last_of("hi") == 13u, "");
  static_assert(tmp.find_last_of("xh") == 1u, "");
  static_assert(tmp.find_last_of("xi", 6u) == 5u, "");
  static_assert(tmp.find_last_of("xis", 6u, 2u) == 5u, "");
  static_assert(tmp.find_last_of("xz") == tmp.npos, "");
  static_assert(FS("a").find_last_of("cba") == 0u, "");
  static_assert(FS("").find_last_of("cba") == tmp.npos, "");
  static_assert(FS("a").find_last_of("") == tmp.npos, "");
  static_assert(FS("").find_last_of("") == tmp.npos, "");
}

TEST(FixedStringFindLastOfTest, FindLastOfChar) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  static_assert(tmp.find_last_of('h') == 1u, "");
  static_assert(tmp.find_last_of('i') == 13u, "");
  static_assert(tmp.find_last_of('i', 6u) == 5u, "");
  static_assert(tmp.find_last_of('x') == tmp.npos, "");
  static_assert(FS("a").find_last_of('a') == 0u, "");
  static_assert(FS("").find_last_of('a') == tmp.npos, "");
}

TEST(FixedStringFindLastNotOfTest, FindLastNotOfString) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  static_assert(tmp.find_last_not_of(FS("gstrin")) == 9u, "");
  static_assert(tmp.find_last_not_of(FS("hT")) == 15u, "");
  static_assert(tmp.find_last_not_of(FS("s atr"), 6u) == 5u, "");
  static_assert(tmp.find_last_not_of(FS("This atrng")) == tmp.npos, "");
  static_assert(FS("a").find_last_not_of(FS("X")) == 0u, "");
  static_assert(FS("").find_last_not_of(FS("cba")) == tmp.npos, "");
  static_assert(FS("a").find_last_not_of(FS("")) == 0u, "");
  static_assert(FS("").find_last_not_of(FS("")) == tmp.npos, "");
}

TEST(FixedStringFindLastNotOfTest, FindLastNotOfLiteral) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  static_assert(tmp.find_last_not_of("gstrin") == 9u, "");
  static_assert(tmp.find_last_not_of("hT") == 15u, "");
  static_assert(tmp.find_last_not_of("s atr", 6u) == 5u, "");
  static_assert(tmp.find_last_not_of(" atrs", 6u, 4u) == 6u, "");
  static_assert(tmp.find_last_not_of("This atrng") == tmp.npos, "");
  static_assert(FS("a").find_last_not_of("X") == 0u, "");
  static_assert(FS("").find_last_not_of("cba") == tmp.npos, "");
  static_assert(FS("a").find_last_not_of("") == 0u, "");
  static_assert(FS("").find_last_not_of("") == tmp.npos, "");
}

TEST(FixedStringFindLastNotOfTest, FindLastNotOfChar) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  static_assert(tmp.find_last_not_of('g') == 14u, "");
  static_assert(tmp.find_last_not_of('i') == 15u, "");
  static_assert(tmp.find_last_not_of('x', 6u) == 6u, "");
  static_assert(tmp.find_last_not_of('s', 6u) == 5u, "");
  static_assert(FS("a").find_last_not_of('a') == tmp.npos, "");
  static_assert(FS("").find_last_not_of('a') == tmp.npos, "");
}

TEST(FixedStringConversionTest, ConversionToStdString) {
  constexpr folly::FixedString<16> tmp{"This is a string"};
  std::string str = tmp;
  EXPECT_STREQ("This is a string", str.c_str());
  str = "another string"_fs16;
  EXPECT_STREQ("another string", str.c_str());
}

constexpr std::size_t countSpacesReverse(folly::FixedString<50> s) {
  std::size_t count = 0u;
  auto i = s.rbegin();
  for (; i != s.rend(); ++i, --i, i++, i--, i += 1, i -= 1, i += 1) {
    if (' ' == *i) {
      ++count;
    }
  }
  return count;
}

TEST(FixedStringReverseIteratorTest, Cpp14ConstexprReverseIteration) {
  static_assert(3 == countSpacesReverse("This is a string"), "");
}

TEST(FixedStringReverseIteratorTest, ConstexprReverseIteration) {
  static constexpr auto alpha = FS("abcdefghijklmnopqrstuvwxyz");
  static_assert('a' == alpha.rbegin()[25], "");
  static_assert('a' == *(alpha.rbegin() + 25), "");
  static_assert('c' == *(alpha.rbegin() + 25 - 2), "");
  static_assert((alpha.rend() - 2) == (alpha.rbegin() + 24), "");
}

/* #include <folly/Range.h> */

TEST(FixedStringConversionTest, ConversionToFollyRange) {
  // The following declaraction is static for compilers that haven't implemented
  // the resolution of:
  // http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#1454
  static constexpr folly::FixedString<16> tmp{"This is a string"};
  constexpr folly::StringPiece piece = tmp;
  static_assert(tmp.begin() == piece.begin(), "");
  static_assert(tmp.end() == piece.end(), "");
}
/* ^^^^ folly/test/FixedStringTest.cpp ^^^^ */

