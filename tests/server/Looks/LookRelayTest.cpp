#include <gtest/gtest.h>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "LookRelayFixture.hpp"
#include "LookRelayTestConstants.hpp"
#include "PacketHeader.hpp"
#include "PlayerLooks.hpp"
#include "SentDatagrams.hpp"
#include "ShipLook.hpp"

namespace rtype::server::testing {

using identifiers::ShipLook;
using network::PlayerLook;
using network::PlayerLooks;

/**
 * Given two seated players
 * When the first one chooses the party hat
 * Then each of them gets one guaranteed PlayerLooks listing the first player
 * with the party hat and the second one with nothing
 */
TEST_F(LookRelayFixture, RelaysAChangedLookToEveryPlayer) {
  seat(FIRST_SEAT);
  seat(SECOND_SEAT);

  choose(FIRST_SEAT, ShipLook{.head = PARTY_HAT_NUMBER});

  const PlayerLooks expected{
      .players = {PlayerLook{.playerNumber = FIRST_PLAYER,
                             .look = ShipLook{.head = PARTY_HAT_NUMBER}},
                  PlayerLook{.playerNumber = SECOND_PLAYER, .look = {}}}};
  EXPECT_EQ(looksTo(FIRST_SEAT), std::vector<PlayerLooks>{expected});
  EXPECT_EQ(looksTo(SECOND_SEAT), std::vector<PlayerLooks>{expected});
  EXPECT_NE(headerOf(sent().at(0)).flags & network::GUARANTEED_FLAG, 0U);
}

/**
 * Given two seated players, neither wearing anything
 * When the first one chooses nothing
 * Then only the first one gets a PlayerLooks: it is a copy, answered to its
 * sender alone
 */
TEST_F(LookRelayFixture, AnswersAnUnchangedLookToItsSenderAlone) {
  seat(FIRST_SEAT);
  seat(SECOND_SEAT);

  choose(FIRST_SEAT, ShipLook{});

  EXPECT_EQ(looksTo(FIRST_SEAT).size(), 1U);
  EXPECT_TRUE(looksTo(SECOND_SEAT).empty());
}

/**
 * Given a player whose first choice was taken
 * When it chooses again in the same tick, then once the whole wait is over
 * Then the second choice sends nothing, and the third is taken
 */
TEST_F(LookRelayFixture, IgnoresAChoiceBeforeTheWaitIsOver) {
  seat(FIRST_SEAT);
  choose(FIRST_SEAT, ShipLook{.head = PARTY_HAT_NUMBER});

  choose(FIRST_SEAT, ShipLook{});
  runTicks(TICKS_ONE_SHORT_OF_THE_CHOICE_WAIT);
  choose(FIRST_SEAT, ShipLook{});

  EXPECT_EQ(looksTo(FIRST_SEAT).size(), 1U);

  runTicks(1);
  choose(FIRST_SEAT, ShipLook{});

  EXPECT_EQ(looksTo(FIRST_SEAT).size(), 2U);
}

/**
 * Given two seated players
 * When a player arrives
 * Then every player gets the looks of all of them
 */
TEST_F(LookRelayFixture, SendsTheLooksToEveryPlayerWhenOneArrives) {
  seat(FIRST_SEAT);
  seat(SECOND_SEAT);

  arrive();

  EXPECT_EQ(looksTo(FIRST_SEAT).size(), 1U);
  EXPECT_EQ(looksTo(SECOND_SEAT).size(), 1U);
}

/**
 * Given a second player who wears the party hat and leaves
 * When a new player takes its seat and the looks are sent
 * Then that player is listed with nothing
 */
TEST_F(LookRelayFixture, ANewPlayerOnAFreedSeatWearsNothing) {
  seat(FIRST_SEAT);
  seat(SECOND_SEAT);
  choose(SECOND_SEAT, ShipLook{.head = PARTY_HAT_NUMBER});
  leave(SECOND_SEAT);
  seat(SECOND_SEAT);

  arrive();

  const std::vector<PlayerLooks> received = looksTo(FIRST_SEAT);
  ASSERT_FALSE(received.empty());
  EXPECT_EQ(received.back().players.back(),
            (PlayerLook{.playerNumber = SECOND_PLAYER, .look = {}}));
}

}  // namespace rtype::server::testing
