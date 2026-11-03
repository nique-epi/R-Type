#include <gtest/gtest.h>
#include "ConnectionPhase.hpp"
#include "SessionLossWatch.hpp"

using rtype::client::ConnectionPhase;
using rtype::client::SessionLossWatch;

/**
 * Given an accepted session the watch saw
 * When the status turns Lost, then stays Lost a frame more
 * Then the watch tells the end once
 */
TEST(SessionLossWatch, TellsALostSessionOnce) {
  SessionLossWatch watch;
  ASSERT_FALSE(watch.endsTheSession({.phase = ConnectionPhase::Accepted}));

  const bool first = watch.endsTheSession({.phase = ConnectionPhase::Lost});
  const bool second = watch.endsTheSession({.phase = ConnectionPhase::Lost});

  EXPECT_TRUE(first);
  EXPECT_FALSE(second);
}

/**
 * Given an accepted session the watch saw
 * When the status turns Closed
 * Then the watch tells the end
 */
TEST(SessionLossWatch, TellsAClosedSession) {
  SessionLossWatch watch;
  ASSERT_FALSE(watch.endsTheSession({.phase = ConnectionPhase::Accepted}));

  EXPECT_TRUE(watch.endsTheSession({.phase = ConnectionPhase::Closed}));
}

/**
 * Given an attempt in progress
 * When the attempt gives up as Unreachable
 * Then no session ended: the connection screen says so itself
 */
TEST(SessionLossWatch, AnUnreachableAttemptEndsNoSession) {
  SessionLossWatch watch;
  ASSERT_FALSE(watch.endsTheSession({.phase = ConnectionPhase::Connecting}));

  EXPECT_FALSE(watch.endsTheSession({.phase = ConnectionPhase::Unreachable}));
}

/**
 * Given a session lost, then a new attempt accepted
 * When the new session is lost too
 * Then the watch tells this end as well
 */
TEST(SessionLossWatch, TellsTheEndOfTheNextSessionToo) {
  SessionLossWatch watch;
  for (const ConnectionPhase phase :
       {ConnectionPhase::Accepted, ConnectionPhase::Lost, ConnectionPhase::Idle,
        ConnectionPhase::Connecting, ConnectionPhase::Accepted}) {
    static_cast<void>(watch.endsTheSession({.phase = phase}));
  }

  EXPECT_TRUE(watch.endsTheSession({.phase = ConnectionPhase::Lost}));
}
