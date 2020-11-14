#include "fancy-assert/detail/impl.hpp"

#ifdef FANCY_DEBUG_ASSERT
#undef FANCY_DEBUG_ASSERT
#endif
#ifdef FANCY_DEBUG_EXPECT
#undef FANCY_DEBUG_EXPECT
#endif
#ifdef FANCY_DEBUG_ASSERT_ELSE
#undef FANCY_DEBUG_ASSERT_ELSE
#endif
#ifdef FANCY_DEBUG_EXPECT_ELSE
#undef FANCY_DEBUG_EXPECT_ELSE
#endif

#ifdef NDEBUG
#define FANCY_DEBUG_ASSERT(...) ((void)0)
#define FANCY_DEBUG_EXPECT(...) ((void)0)
#define FANCY_DEBUG_ASSERT_ELSE(Message, ...) ((void)0)
#define FANCY_DEBUG_EXPECT_ELSE(Message, ...) ((void)0)
#else
#define FANCY_DEBUG_ASSERT(...) FANCY_ASSERT(__VA_ARGS__)
#define FANCY_DEBUG_EXPECT(...) FANCY_EXPECT(__VA_ARGS__)
#define FANCY_DEBUG_ASSERT_ELSE(Message, ...)                                  \
  FANCY_ASSERT_ELSE(Message, __VA_ARGS__)
#define FANCY_DEBUG_EXPECT_ELSE(Message, ...)                                  \
  FANCY_EXPECT_ELSE(Message, __VA_ARGS__)
#endif
