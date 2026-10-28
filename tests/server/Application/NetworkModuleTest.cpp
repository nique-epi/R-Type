#include <gtest/gtest.h>
#include <atomic>
#include <csignal>
#include <cstddef>
#include <future>
#include <thread>
#include <utility>
#include "Application.hpp"
#include "JournalModule.hpp"
#include "LoopStages.hpp"
#include "NetworkModule.hpp"
#include "NetworkModuleFixtures.hpp"
#include "ServerConstants.hpp"
#include "ServerTestConstants.hpp"

using rtype::engine::Application;
using rtype::engine::LoopStages;
using rtype::server::NetworkModule;
using rtype::server::SERVER_LOOP_STAGES;

/**
 * Given the network module run on its own thread, after a module that counts
 * the passes of the loop was started with it
 * When a few ticks are due
 * Then each tick runs the stages of the loop
 */
TEST(NetworkModule, RunsTheStagesAtEachTick) {
#ifdef _WIN32
  GTEST_SKIP() << "the test stops the server with SIGTERM, which Windows "
                  "never delivers";
#else
  JournalModule journal;
  std::atomic<std::size_t> passes{0};
  auto network = networkOnLoopback();
  NetworkModule& pacer = *network;
  Application application(
      LoopStages(SERVER_LOOP_STAGES),
      journalNetworkThenCounter(journal, std::move(network), passes));
  application.start();
  const std::jthread runner{[&pacer] { pacer.run(); }};

  const bool ticked = waitForPasses(passes);
  static_cast<void>(std::raise(SIGTERM));

  EXPECT_TRUE(ticked);
#endif
}

/**
 * Given the network module run on its own thread
 * When the process raises SIGINT, as Ctrl-C does
 * Then run() returns, so the modules can stop
 */
TEST(NetworkModule, ReturnsFromRunOnSigint) {
#ifdef _WIN32
  GTEST_SKIP() << "Windows sends Ctrl-C to the console handler, not to raise()";
#else
  JournalModule journal;
  std::atomic<std::size_t> passes{0};
  auto network = networkOnLoopback();
  NetworkModule& pacer = *network;
  Application application(
      LoopStages(SERVER_LOOP_STAGES),
      journalNetworkThenCounter(journal, std::move(network), passes));
  application.start();
  std::promise<void> returned;
  const std::future<void> runReturned = returned.get_future();
  const std::jthread runner{[&pacer, &returned] {
    pacer.run();
    returned.set_value();
  }};
  static_cast<void>(waitForPasses(passes));

  static_cast<void>(std::raise(SIGINT));

  EXPECT_EQ(runReturned.wait_for(THREAD_TIMEOUT), std::future_status::ready);
#endif
}
