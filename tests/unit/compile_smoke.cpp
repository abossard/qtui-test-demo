#include <gtest/gtest.h>

// Trivial smoke test to validate GoogleTest integration & C++23 build pipeline.
TEST(CompileSmoke, BasicEnvironment) {
  EXPECT_EQ(1 + 1, 2);
}
