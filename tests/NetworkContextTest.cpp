#include <gtest/gtest.h>
#include "NetworkContext.hpp"

/**
 * Given a context with no pending work
 * When it is run
 * Then run returns instead of blocking
 */
TEST(NetworkContext, RunReturnsWhenThereIsNoWork) {
  rtype::network::NetworkContext context;

  context.run();

  EXPECT_TRUE(context.ioContext().stopped());
}
