// the first time the header is included, it must be done in the global
// namespace
#include "fancy_assert/assert.hpp"
#include <fmt/core.h>
#include <iostream>

namespace {
namespace A {
void print(char const* s, int n) {
  fmt::print("{} {}\n", s, n);
}

template <typename T, std::size_t N>
void my_fn(std::array<T, N> arr) {
  // prints an error message if the condition fails then continues execution
  FANCY_EXPECT(arr[0] == 0);

  // prints an error message if the condition fails then continues execution
  FANCY_DEBUG_EXPECT(arr[0] == 3);

#define NDEBUG
  // subsequent inclusions are valid in any context
#include "fancy_assert/assert.hpp"
  FANCY_DEBUG_EXPECT(arr[0] == 3);

  // prints a warning (first argument) to stderr alongside the error message if
  // the condition fails
  // the string is never constructed if the condition holds
  FANCY_EXPECT_ELSE(fmt::format("sizeof(T) is not {}", N), sizeof(T) == N);

  // executes a callback (first argument) alongside the error message if the
  // condition fails
  FANCY_EXPECT_ELSE([&] { print("hello world", arr[0]); }, arr[0] == 0);

#undef NDEBUG
  // subsequent inclusions are valid in any context
#include "fancy_assert/multi_assert.hpp"

  FANCY_DEBUG_EXPECT_ALL_OF((arr[0] == 0), (arr[2] == 1), (arr[1] == 2));

  FANCY_DEBUG_EXPECT_ALL_OF_ELSE(
      ("assertion 0", arr[0] == 0), // print message "assertion 0"
      ([&] { std::cout << arr[2] << '\n'; }, arr[2] == 1), // call lambda
      ("", arr[1] == 2));                                  // do nothing

  // FANCY{,_DEBUG}_ASSERT{,_ELSE,_ALL_OF_ELSE} work the same, but terminate
  // after printing the error
}
} // namespace A
} // namespace

template <typename...>
struct invalid;

auto main() -> int {
  A::my_fn(std::array<int, 3>{{1, 2, 3}});
}
