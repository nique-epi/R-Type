#include <gtest/gtest.h>
#include "ConnectionPhase.hpp"
#include "ConnectionStatus.hpp"
#include "RecordingServerConnection.hpp"
#include "ServerMapScreenTestConstants.hpp"
#include "SessionEndWatch.hpp"
#include "SessionEndWatchTestConstants.hpp"

using rtype::client::ConnectionPhase;
using rtype::client::ConnectionStatus;
using rtype::client::SessionEndWatch;
using rtype::client::testing::LAST_MILLISECOND;
using rtype::client::testing::LONG_PAST_THE_RECHECK;
using rtype::client::testing::ONE_FRAME;
using rtype::client::testing::SHORT_OF_THE_RECHECK;

/**
 * Given a player accepted in a waiting room, then gone back to the map
 * When 1999 ms pass after the session ended
 * Then the games are not asked for yet: the master may still count the player
 */
TEST(SessionEndWatch, WaitsTwoSecondsAfterTheEnd) {
  RecordingServerConnection connection;
  SessionEndWatch watch(connection);
  connection.setStatus(ConnectionStatus{.phase = ConnectionPhase::Accepted});
  static_cast<void>(watch.recheckDue(ONE_FRAME));
  connection.cancel();

  EXPECT_FALSE(watch.recheckDue(SHORT_OF_THE_RECHECK));
}

/**
 * Given a player accepted in a waiting room, then gone back to the map
 * When 2 s pass after the session ended
 * Then the games are asked for again
 */
TEST(SessionEndWatch, AsksAgainTwoSecondsAfterTheEnd) {
  RecordingServerConnection connection;
  SessionEndWatch watch(connection);
  connection.setStatus(ConnectionStatus{.phase = ConnectionPhase::Accepted});
  static_cast<void>(watch.recheckDue(ONE_FRAME));
  connection.cancel();
  static_cast<void>(watch.recheckDue(SHORT_OF_THE_RECHECK));

  EXPECT_TRUE(watch.recheckDue(LAST_MILLISECOND));
}

/**
 * Given a session that ended, and the games asked for again once
 * When a minute more passes with no new attempt
 * Then the games are not asked for again on that end
 */
TEST(SessionEndWatch, AsksOncePerEnd) {
  RecordingServerConnection connection;
  SessionEndWatch watch(connection);
  connection.setStatus(ConnectionStatus{.phase = ConnectionPhase::Accepted});
  static_cast<void>(watch.recheckDue(ONE_FRAME));
  connection.cancel();
  static_cast<void>(watch.recheckDue(LONG_PAST_THE_RECHECK));

  EXPECT_FALSE(watch.recheckDue(LONG_PAST_THE_RECHECK));
}

/**
 * Given a connection that never started an attempt
 * When a minute passes
 * Then the games are never asked for: nothing ended
 */
TEST(SessionEndWatch, AsksNothingWithoutAnAttempt) {
  const RecordingServerConnection connection;
  SessionEndWatch watch(connection);

  EXPECT_FALSE(watch.recheckDue(LONG_PAST_THE_RECHECK));
}

/**
 * Given an attempt the server refused, the connection screen still open
 * When BACK drops the attempt and 2 s pass
 * Then the games are asked for again: the list may have changed meanwhile
 */
TEST(SessionEndWatch, AsksAgainAfterARefusedAttempt) {
  RecordingServerConnection connection;
  SessionEndWatch watch(connection);
  connection.setStatus(ConnectionStatus{.phase = ConnectionPhase::Refused});
  static_cast<void>(watch.recheckDue(ONE_FRAME));
  connection.cancel();
  static_cast<void>(watch.recheckDue(SHORT_OF_THE_RECHECK));

  EXPECT_TRUE(watch.recheckDue(LAST_MILLISECOND));
}
