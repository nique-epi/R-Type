#ifndef _WIN32

// misc-include-cleaner wants <signal.h> for the POSIX signal API, which
// modernize-deprecated-headers rejects in C++: the checks cannot both be met.
// NOLINTBEGIN(misc-include-cleaner)

#include <gtest/gtest.h>
#include <pthread.h>
#include <unistd.h>
#include <csignal>
#include "StopRequest.hpp"
#include "StopSignals.hpp"

using rtype::server::StopRequest;
using rtype::server::StopSignals;

namespace {

/** @returns Whether the calling thread blocks @p signalNumber. */
bool blocksSignal(int signalNumber) {
  sigset_t mask{};
  pthread_sigmask(SIG_BLOCK, nullptr, &mask);
  return sigismember(&mask, signalNumber) == 1;
}

}  // namespace

/**
 * Given the stop signals taken by the server
 * When the system sends the process SIGTERM
 * Then the wait for the stop returns, and the stop is not a failure
 */
TEST(StopSignals, TurnsSigtermIntoAStopRequest) {
  StopRequest request;
  const StopSignals signals{request};

  kill(getpid(), SIGTERM);

  EXPECT_FALSE(request.wait().failed);
}

/**
 * Given the stop signals taken by the server
 * When the operator presses Ctrl-C, which sends the process SIGINT
 * Then the wait for the stop returns, and the stop is not a failure
 */
TEST(StopSignals, TurnsSigintIntoAStopRequest) {
  StopRequest request;
  const StopSignals signals{request};

  kill(getpid(), SIGINT);

  EXPECT_FALSE(request.wait().failed);
}

/**
 * Given the stop signals taken and given back without any signal
 * When the object is destroyed
 * Then the signals are not blocked any more, as before it was built
 */
TEST(StopSignals, GivesTheSignalMaskBackWhenDestroyed) {
  StopRequest request;
  const bool blockedBefore = blocksSignal(SIGTERM) || blocksSignal(SIGINT);
  {
    const StopSignals signals{request};
  }

  EXPECT_FALSE(blockedBefore);
  EXPECT_FALSE(blocksSignal(SIGTERM));
  EXPECT_FALSE(blocksSignal(SIGINT));
}

// NOLINTEND(misc-include-cleaner)

#endif
