#ifndef FANCY_ASSERT_IMPL_HPP_OWZJM607S
#define FANCY_ASSERT_IMPL_HPP_OWZJM607S

#include "fancy_assert/detail/hedley.h"

#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <initializer_list>
#include <sstream>
#include <string>
#include <type_traits>

#if __cplusplus >= 201402L
#define Z_FANCY_ASSERT_CPP14_CONSTEXPR constexpr
#else
#define Z_FANCY_ASSERT_CPP14_CONSTEXPR
#endif

#if defined(Z_FANCY_ASSERT_DIAGNOSTIC_PUSH)
#undef Z_FANCY_ASSERT_DIAGNOSTIC_PUSH
#endif
#if defined(Z_FANCY_ASSERT_DIAGNOSTIC_POP)
#undef Z_FANCY_ASSERT_DIAGNOSTIC_POP
#endif
#if defined(__clang__)
#define Z_FANCY_ASSERT_DIAGNOSTIC_PUSH _Pragma("clang diagnostic push")
#define Z_FANCY_ASSERT_DIAGNOSTIC_POP _Pragma("clang diagnostic pop")
#elif HEDLEY_INTEL_VERSION_CHECK(13, 0, 0)
#define Z_FANCY_ASSERT_DIAGNOSTIC_PUSH _Pragma("warning(push)")
#define Z_FANCY_ASSERT_DIAGNOSTIC_POP _Pragma("warning(pop)")
#elif HEDLEY_GCC_VERSION_CHECK(4, 6, 0)
#define Z_FANCY_ASSERT_DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push")
#define Z_FANCY_ASSERT_DIAGNOSTIC_POP _Pragma("GCC diagnostic pop")
#elif HEDLEY_MSVC_VERSION_CHECK(15, 0, 0) ||                                   \
    HEDLEY_INTEL_CL_VERSION_CHECK(2021, 1, 0)
#define Z_FANCY_ASSERT_DIAGNOSTIC_PUSH __pragma(warning(push))
#define Z_FANCY_ASSERT_DIAGNOSTIC_POP __pragma(warning(pop))
#elif HEDLEY_ARM_VERSION_CHECK(5, 6, 0)
#define Z_FANCY_ASSERT_DIAGNOSTIC_PUSH _Pragma("push")
#define Z_FANCY_ASSERT_DIAGNOSTIC_POP _Pragma("pop")
#elif HEDLEY_TI_VERSION_CHECK(15, 12, 0) ||                                    \
    HEDLEY_TI_ARMCL_VERSION_CHECK(5, 2, 0) ||                                  \
    HEDLEY_TI_CL430_VERSION_CHECK(4, 4, 0) ||                                  \
    HEDLEY_TI_CL6X_VERSION_CHECK(8, 1, 0) ||                                   \
    HEDLEY_TI_CL7X_VERSION_CHECK(1, 2, 0) ||                                   \
    HEDLEY_TI_CLPRU_VERSION_CHECK(2, 1, 0)
#define Z_FANCY_ASSERT_DIAGNOSTIC_PUSH _Pragma("diag_push")
#define Z_FANCY_ASSERT_DIAGNOSTIC_POP _Pragma("diag_pop")
#elif HEDLEY_PELLES_VERSION_CHECK(2, 90, 0)
#define Z_FANCY_ASSERT_DIAGNOSTIC_PUSH _Pragma("warning(push)")
#define Z_FANCY_ASSERT_DIAGNOSTIC_POP _Pragma("warning(pop)")
#else
#define Z_FANCY_ASSERT_DIAGNOSTIC_PUSH
#define Z_FANCY_ASSERT_DIAGNOSTIC_POP
#endif

#if HEDLEY_HAS_WARNING("-Woverloaded-shift-op-parentheses")
#define Z_FANCY_ASSERT_IMPL_IGNORE_SHIFT_PAREN_WARNING(Macro, ...)             \
  Z_FANCY_ASSERT_DIAGNOSTIC_PUSH _Pragma(                                      \
      "clang diagnostic ignored \"-Woverloaded-shift-op-parentheses\" ")       \
      Macro(__VA_ARGS__) Z_FANCY_ASSERT_DIAGNOSTIC_POP
#else
#define Z_FANCY_ASSERT_IMPL_IGNORE_SHIFT_PAREN_WARNING(Macro, ...)             \
  Macro(__VA_ARGS__)
#endif

#if defined(HEDLEY_GNUC_VERSION)
#define Z_FANCY_ASSERT_IMPL_THIS_FUNCTION __PRETTY_FUNCTION__
#elif defined(HEDLEY_MSVC_VERSION)
#define Z_FANCY_ASSERT_IMPL_THIS_FUNCTION __FUNCSIG__
#else
#define Z_FANCY_ASSERT_IMPL_THIS_FUNCTION __func__
#endif

namespace ns_assertions {
namespace _assert {

#define FANCY_ASSERT_DECLVAL(...)                                              \
  (static_cast < __VA_ARGS__ && (*)() > (nullptr)())

template <typename... Ts>
struct void_t {
  using type = void;
};

template <bool B, typename T>
struct enable_if {
  using type = T;
};
template <typename T>
struct enable_if<false, T> {};

template <typename T, typename Enable = void>
struct has_member_fn_begin_end : std::false_type {};

template <typename T>
struct has_member_fn_begin_end<
    T,
    typename void_t<
        decltype(FANCY_ASSERT_DECLVAL(T).begin()),                         //
        decltype(FANCY_ASSERT_DECLVAL(T).end()),                           //
        decltype(static_cast<char const*>(FANCY_ASSERT_DECLVAL(T).data())) //
        >::type> : std::true_type {};

enum struct def_e { mem, adl, none };

template <
    typename T,
    def_e = has_member_fn_begin_end<T>::value ? def_e::mem : def_e::none>
struct begin_end {
  static constexpr bool value = false;
};

template <typename T>
struct begin_end<T, def_e::mem> {
  static constexpr bool value = true;
  static constexpr auto b(T const& rng) -> char const* { return rng.data(); }
  static constexpr auto e(T const& rng) -> char const* {
    return rng.data() + static_cast<std::size_t>(rng.end() - rng.begin());
  }
};

template <std::size_t N>
struct begin_end<char[N], def_e::none> { // NOLINT
  static constexpr bool value = true;
  static constexpr auto b(char const (&arr)[N]) -> char const* { // NOLINT
    return arr;
  }                                                              // NOLINT
  static constexpr auto e(char const (&arr)[N]) -> char const* { // NOLINT
    return arr + N - 1;
  }
};

template <std::size_t N>
struct begin_end<char const[N], def_e::none> : begin_end<char[N]> { // NOLINT
};
template <>
struct begin_end<char const*> {
  static constexpr bool value = true;
  static auto b(char const* p) -> char const* { return p; }
  static auto e(char const* p) -> char const* { return p + std::strlen(p); }
};
template <>
struct begin_end<char*> : begin_end<char const*> {};

struct string_view {
  HEDLEY_WARN_UNUSED_RESULT auto size() const -> std::size_t {
    return static_cast<std::size_t>(m_end - m_begin);
  }

  template <
      typename T,
      typename enable_if<
          begin_end<T>::value and not std::is_same<T, string_view>::value,
          void>::type* = nullptr>
  constexpr string_view /* NOLINT */ (T const& str_container)
      : m_begin{begin_end<T>::b(str_container)},
        m_end{begin_end<T>::e(str_container)} {}

  string_view() = default;

  HEDLEY_WARN_UNUSED_RESULT auto begin() const -> char const* {
    return m_begin;
  }
  HEDLEY_WARN_UNUSED_RESULT auto end() const -> char const* { return m_end; }

  HEDLEY_WARN_UNUSED_RESULT friend auto
  operator==(string_view s1, string_view s2) -> bool {
    if (s1.empty()) {
      return s2.empty();
    }
    return s1.size() == s2.size() and
           (std::memcmp(s1.m_begin, s2.m_begin, s1.size()) == 0);
  }

  HEDLEY_WARN_UNUSED_RESULT auto
  substr(std::size_t pos, std::size_t count = SIZE_MAX) const -> string_view {
    if (count == SIZE_MAX) {
      count = size() - pos;
    }
    if (pos > size() or pos + count > size()) {
      std::abort();
    }
    return {m_begin + pos, m_begin + pos + count};
  }

  HEDLEY_WARN_UNUSED_RESULT auto empty() const -> bool {
    return m_begin == m_end;
  }

  HEDLEY_WARN_UNUSED_RESULT auto starts_with(string_view s2) const -> bool {
    return this->size() >= s2.size() and this->substr(0, s2.size()) == s2;
  }

  HEDLEY_WARN_UNUSED_RESULT auto empty_or_matches(string_view s2) const
      -> bool {
    return this->empty() or *this == s2;
  }

  string_view(char const* b, char const* e) : m_begin{b}, m_end{e} {}

private:
  char const* m_begin = nullptr;
  char const* m_end = nullptr;
};

struct callback_t {
  void* m_ptr = nullptr;
  void (*m_call)(void* ptr) = nullptr;
  auto (*m_get_str)(void* ptr) -> std::string = nullptr;

  template <typename F>
  static void call_impl(void* ptr) {
    static_cast<F&&> (
        *static_cast<typename std::remove_reference<F>::type*>(ptr))()();
  }
  template <typename F>
  static auto str_impl(void* ptr) -> std::string {
    auto r = static_cast<F&&>(
        *static_cast<typename std::remove_reference<F>::type*>(ptr))();
    return {begin_end<decltype(r)>::b(r), begin_end<decltype(r)>::e(r)};
  }

  template <
      typename F,
      typename enable_if<
          begin_end<typename std::remove_reference<
              decltype(FANCY_ASSERT_DECLVAL(F)())>::type>::value,
          void>::type* = nullptr>
  constexpr callback_t(F&& f) noexcept // NOLINT
      : m_ptr{&f}, m_get_str{&str_impl<F>} {}

  template <
      typename F,
      typename enable_if<
          not begin_end<typename std::remove_reference<
              decltype(FANCY_ASSERT_DECLVAL(F)())>::type>::value,
          typename void_t<decltype(FANCY_ASSERT_DECLVAL(F)()())>::type>::type* =
          nullptr>
  constexpr callback_t(F&& f) noexcept // NOLINT
      : m_ptr{&f}, m_call{&call_impl<F>} {}

  void call() const {
    if (m_call == nullptr) {
      std::abort();
    }
    m_call(m_ptr);
  }
  HEDLEY_WARN_UNUSED_RESULT auto get_str() const -> std::string {
    if (m_get_str == nullptr) {
      return "";
    }
    return m_get_str(m_ptr);
  }
};

template <typename T>
auto to_string(T const& val) -> std::string {
  std::stringstream ss;
  ss << val;
  return static_cast<std::stringstream&&>(ss).str();
}

Z_FANCY_ASSERT_CPP14_CONSTEXPR
auto inline all_of(std::initializer_list<bool> arr) -> bool {
  for/* NOLINT */ (bool b : arr) {
    if (!b) {
      return false;
    }
  }
  return true;
}

[[noreturn]]

void on_assert_fail(long line, char const* file, char const* func);
void on_expect_fail(long line, char const* file, char const* func);
void set_assert_params(     //
    string_view expression, //
    callback_t callback,    //
    string_view op,         //
    std::string lhs,        //
    std::string rhs         //
);
void parse_func_signature(std::string& output, string_view func);

template <typename T>
struct lhs_all_of_t {
  T const& lhs;
  string_view expr;
  callback_t callback;

  template <typename U>
  void on_assertion_fail(U const& rhs, string_view op_str) {
    ::ns_assertions::_assert::set_assert_params(
        expr,
        callback,
        op_str,
        ::ns_assertions::_assert::to_string(lhs),
        ::ns_assertions::_assert::to_string(rhs));
  }

#define FANCY_ASSERT_COMPARISON_OP(Op)                                         \
  template <typename U>                                                        \
  HEDLEY_ALWAYS_INLINE Z_FANCY_ASSERT_CPP14_CONSTEXPR auto operator Op(        \
      U const& rhs)                                                            \
      ->bool {                                                                 \
    bool res = static_cast<bool>(lhs Op rhs);                                  \
    if (not res) {                                                             \
      on_assertion_fail(rhs, " " #Op " ");                                     \
    }                                                                          \
    return res;                                                                \
  }                                                                            \
  static_assert(true, "")

#define FANCY_ASSERT_DISABLE(Op)                                               \
  template <typename U>                                                        \
  void operator Op(U const& rhs) = delete

  FANCY_ASSERT_DISABLE(<<);
  FANCY_ASSERT_DISABLE(>>);
  FANCY_ASSERT_DISABLE(&);
  FANCY_ASSERT_DISABLE(|);
  FANCY_ASSERT_DISABLE(+);
  FANCY_ASSERT_DISABLE(-);
  FANCY_ASSERT_DISABLE(*);
  FANCY_ASSERT_DISABLE(/);
  FANCY_ASSERT_DISABLE(%);
  FANCY_ASSERT_DISABLE(^);

  FANCY_ASSERT_COMPARISON_OP(==);
  FANCY_ASSERT_COMPARISON_OP(!=);
  FANCY_ASSERT_COMPARISON_OP(<);
  FANCY_ASSERT_COMPARISON_OP(>);
  FANCY_ASSERT_COMPARISON_OP(<=);
  FANCY_ASSERT_COMPARISON_OP(>=);

#undef FANCY_ASSERT_COMPARISON_OP

  HEDLEY_ALWAYS_INLINE explicit Z_FANCY_ASSERT_CPP14_CONSTEXPR
  operator/* NOLINT(hicpp-explicit-conversions) */ bool() const {
    bool res = static_cast<bool>(lhs);
    if (not res) {
      _assert::set_assert_params(
          expr, callback, "", _assert::to_string(lhs), "");
    }
    return res;
  }
};

struct expression_decomposer_all_of_t {
  string_view expr;
  callback_t callback;

  template <typename T>
  constexpr auto operator<<(T const& lhs) const -> lhs_all_of_t<T> {
    return {lhs, expr, callback};
  }
};

#if __cplusplus >= 201402L
#define Z_FANCY_ASSERT_IMPL_WRAP_LAMBDA(Elem)                                  \
  [&]() -> decltype(auto) { return Elem; }
#else

template <typename T>
struct remove_rvalue_ref;
template <typename T>
struct remove_rvalue_ref<T&> {
  using type = T&;
};
template <typename T>
struct remove_rvalue_ref<T&&> {
  using type = T;
};

// allows moving into lambda
template <typename T>
struct move_on_copy { // NOLINT
  T m_inner;
  move_on_copy(T&& other) : m_inner{static_cast<T&&>(other)} {} // NOLINT
  move_on_copy(move_on_copy& other)
      : m_inner{static_cast<T&&>(other.m_inner)} {}
  move_on_copy(move_on_copy&& other) = default; // NOLINT
};

template <typename T>
struct call_twice {
  T&& callable;
  auto operator()() const -> decltype(static_cast<T&&>(callable)()()) {
    return static_cast<T&&>(callable)()();
  }
};

template <typename T>
auto make_call_twice(T&& callable) -> call_twice<T&&> {
  return {static_cast<T&&>(callable)};
}

#define Z_FANCY_ASSERT_IMPL_WRAP_LAMBDA(Elem)                                  \
  ::ns_assertions::__assert::make_call_twice(                                  \
      [&]() { /* NOLINT */                                                     \
              /* returns a callable, that when called, returns reference to */ \
              /* Elem if lvalue, or moves it if rvalue*/                       \
              auto&& _value = Elem; /* create object */                        \
              using _type =                                                    \
                  typename remove_rvalue_ref<decltype(_value)>::type;          \
                                                                               \
              /* move into lambda */                                           \
              move_on_copy<_type> _wrapper{static_cast<_type&&>(_value)};      \
              return [&, _wrapper]() mutable -> _type {                        \
                return static_cast<_type&&>(_wrapper.m_inner);                 \
              };                                                               \
      })
#endif

struct do_nothing_callable {
  auto operator()() -> char const* { return ""; }
};

#define Z_FANCY_ASSERT_IMPL_EMPTY_CALLABLE(Elem)                               \
  ::ns_assertions::_assert::do_nothing_callable {}

#define Z_FANCY_ASSERT_IMPL_ASSERT_IMPL(                                       \
    Callback, Callable_Wrapper, If_Fail, ...)                                  \
  (static_cast<bool>(                                                          \
       ::ns_assertions::_assert::expression_decomposer_all_of_t{               \
           #__VA_ARGS__,                                                       \
           Callable_Wrapper(If_Fail),                                          \
       }                                                                       \
       << __VA_ARGS__)                                                         \
       ? (void)(0)                                                             \
       : ::ns_assertions::_assert::Callback(                                   \
             __LINE__,                                                         \
             static_cast<char const*>(__FILE__),                               \
             static_cast<char const*>(Z_FANCY_ASSERT_IMPL_THIS_FUNCTION)))

#define FANCY_ASSERT_ELSE(...)                                                 \
  Z_FANCY_ASSERT_IMPL_IGNORE_SHIFT_PAREN_WARNING(                              \
      Z_FANCY_ASSERT_IMPL_ASSERT_IMPL,                                         \
      on_assert_fail,                                                          \
      Z_FANCY_ASSERT_IMPL_WRAP_LAMBDA,                                         \
      __VA_ARGS__)
#define FANCY_EXPECT_ELSE(...)                                                 \
  Z_FANCY_ASSERT_IMPL_IGNORE_SHIFT_PAREN_WARNING(                              \
      Z_FANCY_ASSERT_IMPL_ASSERT_IMPL,                                         \
      on_expect_fail,                                                          \
      Z_FANCY_ASSERT_IMPL_WRAP_LAMBDA,                                         \
      __VA_ARGS__)

#define FANCY_ASSERT(...)                                                      \
  Z_FANCY_ASSERT_IMPL_IGNORE_SHIFT_PAREN_WARNING(                              \
      Z_FANCY_ASSERT_IMPL_ASSERT_IMPL,                                         \
      on_assert_fail,                                                          \
      Z_FANCY_ASSERT_IMPL_EMPTY_CALLABLE,                                      \
      "",                                                                      \
      __VA_ARGS__)

#define FANCY_EXPECT(...)                                                      \
  Z_FANCY_ASSERT_IMPL_IGNORE_SHIFT_PAREN_WARNING(                              \
      Z_FANCY_ASSERT_IMPL_ASSERT_IMPL,                                         \
      on_expect_fail,                                                          \
      Z_FANCY_ASSERT_IMPL_EMPTY_CALLABLE,                                      \
      "",                                                                      \
      __VA_ARGS__)

} // namespace _assert
} // namespace ns_assertions

#include "fancy_assert/detail/unhedley.h"
#endif /* end of include guard FANCY_ASSERT_IMPL_HPP_OWZJM607S */
