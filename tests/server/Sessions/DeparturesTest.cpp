#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DeparturesFixture.hpp"
#include "Endpoint.hpp"
#include "PlayerDeparture.hpp"
#include "SentDatagrams.hpp"
#include "SessionConstants.hpp"
#include "SessionsTestConstants.hpp"

namespace rtype::server {

using testing::ClosingStep;
using testing::DeparturesFixture;
using testing::playerEndpoint;
using testing::SOME_INPUT_NUMBER;

/**
 * Given a game with one player
 * When its session closes because it left
 * Then its seat is free, the closing function saw it already free, a
 * departure (Left) is announced and the player is kept for whoever forgets it
 */
TEST_F(DeparturesFixture, ClosesTheSessionOfAPlayerWhoLeft) {
  const network::Endpoint endpoint = player().endpoint;
  const std::uint8_t number = player().playerNumber;

  departures().close(endpoint, game::DepartureCause::Left);

  EXPECT_EQ(roster().find(endpoint), nullptr);
  EXPECT_TRUE(seatFreedWhenClosing());
  ASSERT_EQ(closingPlayers().size(), 1U);
  EXPECT_EQ(closingPlayers().front().playerNumber, number);
  ASSERT_EQ(announced().size(), 1U);
  EXPECT_EQ(announced().front().playerNumber, number);
  EXPECT_EQ(announced().front().cause, game::DepartureCause::Left);
  EXPECT_EQ(departures().takeDeparted().size(), 1U);
}

/**
 * Given a game with one player
 * When a session closes for an address no player sends from
 * Then nothing is called, announced or kept
 */
TEST_F(DeparturesFixture, ClosesNothingForAnUnknownAddress) {
  departures().close(playerEndpoint(1), game::DepartureCause::Left);

  EXPECT_TRUE(closingPlayers().empty());
  EXPECT_TRUE(announced().empty());
  EXPECT_TRUE(departures().takeDeparted().empty());
  EXPECT_NE(roster().find(player().endpoint), nullptr);
}

/**
 * Given a player closed once
 * When the departed players are taken twice
 * Then the second call returns none
 */
TEST_F(DeparturesFixture, HandsEachDepartedPlayerOverOnce) {
  departures().close(player().endpoint, game::DepartureCause::Left);
  static_cast<void>(departures().takeDeparted());

  EXPECT_TRUE(departures().takeDeparted().empty());
}

/**
 * Given a player not heard from for 1200 ticks, the timeout and the grace of
 * the protocol page, 5 s and 15 s
 * When one more tick of silence is counted
 * Then its session closes with a departure (TimedOut), and not a tick before
 */
TEST_F(DeparturesFixture, ClosesASilentSessionOnceTheGraceIsOver) {
  for (std::size_t tick = 0; tick < SESSION_CLOSING_TICKS; ++tick) {
    departures().closeSilentSessions();
  }
  ASSERT_NE(roster().find(playerEndpoint(0)), nullptr);

  departures().closeSilentSessions();

  EXPECT_EQ(roster().find(playerEndpoint(0)), nullptr);
  ASSERT_EQ(announced().size(), 1U);
  EXPECT_EQ(announced().front().cause, game::DepartureCause::TimedOut);
}

/**
 * Given a player with an input number, silent for 299 ticks, 5 s less one
 * When one more tick of silence is counted
 * Then the player is away, its input number cleared, its seat kept and no
 * departure announced; and it was not away a tick before
 */
TEST_F(DeparturesFixture, MarksAPlayerAwayOnceTheTimeoutIsReached) {
  player().newestInputNumber = SOME_INPUT_NUMBER;
  for (std::size_t tick = 0; tick < SESSION_TIMEOUT_TICKS - 1; ++tick) {
    departures().closeSilentSessions();
  }
  ASSERT_FALSE(player().away);

  departures().closeSilentSessions();

  EXPECT_TRUE(player().away);
  EXPECT_FALSE(player().newestInputNumber.has_value());
  EXPECT_NE(roster().find(playerEndpoint(0)), nullptr);
  EXPECT_TRUE(announced().empty());
}

/**
 * Given a player with an input applied, silent until its timeout is reached
 * When one more tick of silence is counted
 * Then the number of its newest applied input is cleared
 */
TEST_F(DeparturesFixture, ClearsTheAppliedInputNumberOfAPlayerWhoIsAway) {
  player().newestAppliedInputNumber = SOME_INPUT_NUMBER;
  for (std::size_t tick = 0; tick < SESSION_TIMEOUT_TICKS; ++tick) {
    departures().closeSilentSessions();
  }

  EXPECT_FALSE(player().newestAppliedInputNumber.has_value());
}

/**
 * Given a player heard from at the last tick before its timeout
 * When silence is counted again
 * Then its session stays open
 */
TEST_F(DeparturesFixture, KeepsTheSessionOfAPlayerHeardFrom) {
  for (std::size_t tick = 0; tick < SESSION_TIMEOUT_TICKS; ++tick) {
    departures().closeSilentSessions();
  }
  player().ticksSinceReceived = 0;

  departures().closeSilentSessions();

  EXPECT_NE(roster().find(playerEndpoint(0)), nullptr);
  EXPECT_TRUE(announced().empty());
}

/**
 * Given a game with one player
 * When its session closes
 * Then the bus delivers the removal of its ship before its departure
 */
TEST_F(DeparturesFixture, RemovesTheShipBeforeAnnouncingTheDeparture) {
  departures().close(player().endpoint, game::DepartureCause::Left);

  EXPECT_EQ(steps(),
            (std::vector<ClosingStep>{ClosingStep::ShipRemoved,
                                      ClosingStep::DepartureAnnounced}));
}

}  // namespace rtype::server
