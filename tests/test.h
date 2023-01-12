#ifndef TEST_H
#define TEST_H

#include <jackal.h>

#define TEST(name, block) \
    static int test_##name() \
    { \
      jkl_log("jkl_test", "running " #name); \
      block \
    };

#define RUN_TEST(name) \
    if (test_##name() == 1) \
      jkl_error("jkl_test", #name " failed") \
    else \
      jkl_log("jkl_test", #name " passed");

#define ASSERT(expr) \
    if (!(expr)) \
      return 1;

#define ASSERT_EQ(a, b) \
    if ((a) != (b)) \
    { \
      jkl_error("jkl_test", "assertion failed: %d != %d", (a), (b)); \
      return 1; \
    };
#endif
