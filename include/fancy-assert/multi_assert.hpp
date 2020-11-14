#ifndef REPO_MULTI_ASSERT_HPP_JL3FUYOTS
#define REPO_MULTI_ASSERT_HPP_JL3FUYOTS

#include "fancy-assert/detail/boostpp.h"
#include "fancy-assert/assert.hpp"

#define Z_FANCY_ASSERT_IMPL_ELSE_AGGREGATE_IMPL1(                              \
    Ftor, Decomposer, Aggregator, Callback, ...)                               \
  Z_FANCY_ASSERT_IMPL_ELSE_AGGREGATE_IMPL2(                                    \
      Ftor,                                                                    \
      Decomposer,                                                              \
      Aggregator,                                                              \
      Callback,                                                                \
      Z_FANCY_ASSERT_IMPL_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define Z_FANCY_ASSERT_IMPL_ASSERT_AGGREGATE_IMPL_FTOR(_, Decomposer, Elem)    \
  (::ns_assertions::_assert::Decomposer{                                       \
       Z_FANCY_ASSERT_IMPL_PP_STRINGIZE(Z_FANCY_ASSERT_IMPL_PP_REMOVE_PARENS(  \
           Z_FANCY_ASSERT_IMPL_PP_TUPLE_POP_FRONT(Elem))),                     \
       Z_FANCY_ASSERT_IMPL_WRAP_LAMBDA(""),                                    \
   }                                                                           \
   << Z_FANCY_ASSERT_IMPL_PP_REMOVE_PARENS(                                    \
          Z_FANCY_ASSERT_IMPL_PP_TUPLE_POP_FRONT(Elem))),

#define Z_FANCY_ASSERT_IMPL_ASSERT_ELSE_AGGREGATE_IMPL_FTOR(                   \
    _, Decomposer, Elem)                                                       \
  (::ns_assertions::_assert::Decomposer{                                       \
       Z_FANCY_ASSERT_IMPL_PP_STRINGIZE(Z_FANCY_ASSERT_IMPL_PP_REMOVE_PARENS(  \
           Z_FANCY_ASSERT_IMPL_PP_TUPLE_POP_FRONT(Elem))),                     \
       Z_FANCY_ASSERT_IMPL_WRAP_LAMBDA(                                        \
           Z_FANCY_ASSERT_IMPL_PP_TUPLE_ELEM(0, Elem)),                        \
   }                                                                           \
   << Z_FANCY_ASSERT_IMPL_PP_REMOVE_PARENS(                                    \
          Z_FANCY_ASSERT_IMPL_PP_TUPLE_POP_FRONT(Elem))),

#define Z_FANCY_ASSERT_IMPL_ELSE_AGGREGATE_IMPL2(                              \
    Ftor, Decomposer, Aggregator, Callback, Seq)                               \
  (::ns_assertions::_assert::Aggregator(                                       \
       {Z_FANCY_ASSERT_IMPL_PP_SEQ_FOR_EACH(Ftor, Decomposer, Seq)})           \
       ? (void)(0)                                                             \
       : ::ns_assertions::_assert::Callback(                                   \
             __LINE__,                                                         \
             static_cast<char const*>(__FILE__),                               \
             static_cast<char const*>(Z_FANCY_ASSERT_IMPL_THIS_FUNCTION)))

#define FANCY_ASSERT_ALL_OF(...)                                               \
  Z_FANCY_ASSERT_IMPL_IGNORE_SHIFT_PAREN_WARNING(                              \
      Z_FANCY_ASSERT_IMPL_ELSE_AGGREGATE_IMPL1,                                \
      Z_FANCY_ASSERT_IMPL_ASSERT_AGGREGATE_IMPL_FTOR,                          \
      expression_decomposer_all_of_t,                                          \
      all_of,                                                                  \
      on_assert_fail,                                                          \
      __VA_ARGS__)

#define FANCY_EXPECT_ALL_OF(...)                                               \
  Z_FANCY_ASSERT_IMPL_IGNORE_SHIFT_PAREN_WARNING(                              \
      Z_FANCY_ASSERT_IMPL_ELSE_AGGREGATE_IMPL1,                                \
      Z_FANCY_ASSERT_IMPL_ASSERT_AGGREGATE_IMPL_FTOR,                          \
      expression_decomposer_all_of_t,                                          \
      all_of,                                                                  \
      on_expect_fail,                                                          \
      __VA_ARGS__)

#define FANCY_ASSERT_ALL_OF_ELSE(...)                                          \
  Z_FANCY_ASSERT_IMPL_IGNORE_SHIFT_PAREN_WARNING(                              \
      Z_FANCY_ASSERT_IMPL_ELSE_AGGREGATE_IMPL1,                                \
      Z_FANCY_ASSERT_IMPL_ASSERT_ELSE_AGGREGATE_IMPL_FTOR,                     \
      expression_decomposer_all_of_t,                                          \
      all_of,                                                                  \
      on_assert_fail,                                                          \
      __VA_ARGS__)

#define FANCY_EXPECT_ALL_OF_ELSE(...)                                          \
  Z_FANCY_ASSERT_IMPL_IGNORE_SHIFT_PAREN_WARNING(                              \
      Z_FANCY_ASSERT_IMPL_ELSE_AGGREGATE_IMPL1,                                \
      Z_FANCY_ASSERT_IMPL_ASSERT_ELSE_AGGREGATE_IMPL_FTOR,                     \
      expression_decomposer_all_of_t,                                          \
      all_of,                                                                  \
      on_expect_fail,                                                          \
      __VA_ARGS__)

#endif /* end of include guard REPO_MULTI_ASSERT_HPP_JL3FUYOTS */

#ifdef NDEBUG
#define FANCY_DEBUG_ASSERT_ALL_OF(...) ((void)0)
#define FANCY_DEBUG_EXPECT_ALL_OF(...) ((void)0)
#define FANCY_DEBUG_ASSERT_ALL_OF_ELSE(...) ((void)0)
#define FANCY_DEBUG_EXPECT_ALL_OF_ELSE(...) ((void)0)
#else
#define FANCY_DEBUG_ASSERT_ALL_OF(...) FANCY_ASSERT_ALL_OF(__VA_ARGS__)
#define FANCY_DEBUG_EXPECT_ALL_OF(...) FANCY_EXPECT_ALL_OF(__VA_ARGS__)
#define FANCY_DEBUG_ASSERT_ALL_OF_ELSE(...)                                    \
  FANCY_ASSERT_ALL_OF_ELSE(__VA_ARGS__)
#define FANCY_DEBUG_EXPECT_ALL_OF_ELSE(...)                                    \
  FANCY_EXPECT_ALL_OF_ELSE(__VA_ARGS__)
#endif
