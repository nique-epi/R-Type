#include <gtest/gtest.h>
#include <csignal>
#include "StopSignals.hpp"

using rtype::server::StopSignals;

/**
 * Given the stop signals caught
 * When no signal arrives
 * Then no stop was received
 */
TEST(StopSignals, ReceivesNoStopBeforeASignal) {
  const StopSignals signals;

  EXPECT_FALSE(signals.received());
}

/**
 * Given the stop signals caught, on Linux or macOS
 * When the process raises SIGINT, as Ctrl-C does
 * Then a stop was received, and the process goes on
 */
TEST(StopSignals, ReceivesAStopOnSigint) {
#ifdef _WIN32
  GTEST_SKIP() << "Windows sends Ctrl-C to the console handler, not to raise()";
#else
  const StopSignals signals;

  static_cast<void>(std::raise(SIGINT));

  EXPECT_TRUE(signals.received());
#endif
}

/**
 * Given the stop signals caught, on Linux or macOS
 * When the process raises SIGTERM, as a service manager does to stop it
 * Then a stop was received, and the process goes on
 */
TEST(StopSignals, ReceivesAStopOnSigterm) {
#ifdef _WIN32
  GTEST_SKIP() << "Windows never generates SIGTERM";
#else
  const StopSignals signals;

  static_cast<void>(std::raise(SIGTERM));

  EXPECT_TRUE(signals.received());
#endif
}

/**
 * Given stop signals that received SIGINT, then were destroyed
 * When the stop signals are caught again
 * Then no stop was received
 */
TEST(StopSignals, StartsWithoutTheStopOfAnEarlierOne) {
#ifdef _WIN32
  GTEST_SKIP() << "Windows sends Ctrl-C to the console handler, not to raise()";
#else
  {
    const StopSignals earlier;
    static_cast<void>(std::raise(SIGINT));
  }

  const StopSignals signals;

  EXPECT_FALSE(signals.received());
#endif
}
