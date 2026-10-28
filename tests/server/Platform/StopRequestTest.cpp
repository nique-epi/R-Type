#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include "StopReason.hpp"
#include "StopRequest.hpp"
#include "StopRequestTestConstants.hpp"

using rtype::server::StopReason;
using rtype::server::StopRequest;

/**
 * Given a stop asked for by the operator before anyone waits
 * When the server waits for the stop
 * Then it returns at once, and the stop is not a failure
 */
TEST(StopRequest, ReturnsAtOnceWhenTheStopWasAskedForBeforeTheWait) {
  StopRequest request;
  request.request();

  const StopReason reason = request.wait();

  EXPECT_FALSE(reason.failed);
}

/**
 * Given a part of the server that failed with a text
 * When the server waits for the stop
 * Then the reason is a failure carrying that text
 */
TEST(StopRequest, CarriesTheTextOfAFailure) {
  StopRequest request;
  request.fail(FAILURE_TEXT);

  const StopReason reason = request.wait();

  EXPECT_TRUE(reason.failed);
  EXPECT_EQ(reason.text, FAILURE_TEXT);
}

/**
 * Given a stop asked for by a signal, then a failure
 * When the server waits for the stop
 * Then the reason is the signal: the failure does not replace it
 */
TEST(StopRequest, KeepsASignalWhenAFailureFollows) {
  StopRequest request;
  request.request();
  request.fail(FAILURE_TEXT);

  EXPECT_FALSE(request.wait().failed);
}

/**
 * Given a failure, then a second failure
 * When the server waits for the stop
 * Then the reason is the first failure
 */
TEST(StopRequest, KeepsTheFirstFailure) {
  StopRequest request;
  request.fail(FAILURE_TEXT);
  request.fail(LATER_FAILURE_TEXT);

  EXPECT_EQ(request.wait().text, FAILURE_TEXT);
}

/**
 * Given the server waiting for the stop on this thread
 * When another thread asks for it a moment later
 * Then the wait returns with that failure
 */
TEST(StopRequest, WakesTheWaitWhenAnotherThreadAsksForTheStop) {
  StopRequest request;
  std::jthread asker{[&request] {
    std::this_thread::sleep_for(ASKING_DELAY);
    request.fail(FAILURE_TEXT);
  }};

  EXPECT_EQ(request.wait().text, FAILURE_TEXT);
}

/**
 * Given a stop nobody announced finished
 * When a handler waits for it with a short timeout
 * Then it gets false after the timeout
 */
TEST(StopRequest, ReportsThatTheStopWasNotAnnouncedWhenTheWaitTimesOut) {
  StopRequest request;
  request.request();

  EXPECT_FALSE(request.waitUntilStopped(UNANSWERED_WAIT));
}

/**
 * Given a stop announced finished
 * When a handler waits for it
 * Then it gets true
 */
TEST(StopRequest, ReportsThatTheStopWasAnnounced) {
  StopRequest request;
  request.request();
  request.announceStopped();

  EXPECT_TRUE(request.waitUntilStopped(ANSWERED_WAIT));
}
