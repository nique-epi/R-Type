#include <gtest/gtest.h>
#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <memory>
#include <optional>
#include <system_error>
#include <thread>
#include "NetworkContext.hpp"
#include "NetworkTestConstants.hpp"

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

/**
 * Given a context kept running on the test thread
 * When another thread posts a task to it
 * Then the task runs on the test thread, the one running the context
 */
TEST(NetworkContext, PostedTaskRunsOnTheThreadThatRunsTheContext) {
  rtype::network::NetworkContext context;
  const auto keepRunning = asio::make_work_guard(context.ioContext());
  asio::steady_timer safetyStop{context.ioContext(), DATAGRAM_TIMEOUT};
  safetyStop.async_wait([&context](const std::error_code&) { context.stop(); });
  std::optional<std::thread::id> taskThread;
  const std::jthread poster{[&context, &taskThread] {
    context.post([&context, &taskThread] {
      taskThread = std::this_thread::get_id();
      context.stop();
    });
  }};

  context.run();

  EXPECT_EQ(taskThread, std::this_thread::get_id());
}

/**
 * Given a task posted to a context that is never run
 * When the context is destroyed
 * Then the task was destroyed without running
 */
TEST(NetworkContext, TaskStillQueuedAtDestructionIsDestroyedWithoutRunning) {
  const auto runCount = std::make_shared<int>(0);
  {
    rtype::network::NetworkContext context;
    context.post([runCount] { ++*runCount; });
  }

  EXPECT_EQ(*runCount, 0);
  EXPECT_EQ(runCount.use_count(), 1);
}
