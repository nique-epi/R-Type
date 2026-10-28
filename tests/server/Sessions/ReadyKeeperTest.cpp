#include <gtest/gtest.h>
#include "ConnectedPlayer.hpp"
#include "PlayerRoster.hpp"
#include "ReadyKeeper.hpp"
#include "ReadyState.hpp"
#include "SessionsFixtures.hpp"
#include "SessionsTestConstants.hpp"

namespace rtype::server {

using testing::addPlayer;
using testing::FIRST_SEQUENCE;
using testing::LAST_SEQUENCE;
using testing::NEWER_SEQUENCE;
using testing::OLDER_SEQUENCE;
using testing::originOf;
using testing::SOME_SEQUENCE;
using testing::TWO_PLAYERS;

/**
 * Given a keeper that accepts a ReadyState and a player who is not ready
 * When the player says ready
 * Then the player is ready
 */
TEST(ReadyKeeper, MarksThePlayerReady) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  ReadyKeeper keeper(roster, [] { return true; });

  keeper.apply(originOf(0, SOME_SEQUENCE), network::ReadyState{.ready = true});

  EXPECT_TRUE(player.ready);
}

/**
 * Given a keeper that does not accept a ReadyState right now
 * When a player says ready
 * Then the player is not ready
 */
TEST(ReadyKeeper, IgnoresAReadyStateItDoesNotAccept) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  ReadyKeeper keeper(roster, [] { return false; });

  keeper.apply(originOf(0, SOME_SEQUENCE), network::ReadyState{.ready = true});

  EXPECT_FALSE(player.ready);
}

/**
 * Given a player who said ready in the datagram of sequence 40
 * When a ReadyState of sequence 39 says not ready
 * Then the player stays ready
 */
TEST(ReadyKeeper, IgnoresAReadyStateOlderThanTheNewestAccepted) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  ReadyKeeper keeper(roster, [] { return true; });
  keeper.apply(originOf(0, SOME_SEQUENCE), network::ReadyState{.ready = true});

  keeper.apply(originOf(0, OLDER_SEQUENCE),
               network::ReadyState{.ready = false});

  EXPECT_TRUE(player.ready);
}

/**
 * Given a player who said ready in the datagram of sequence 40
 * When a ReadyState of sequence 41 says not ready
 * Then the player is not ready
 */
TEST(ReadyKeeper, AppliesAReadyStateNewerThanTheNewestAccepted) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  ReadyKeeper keeper(roster, [] { return true; });
  keeper.apply(originOf(0, SOME_SEQUENCE), network::ReadyState{.ready = true});

  keeper.apply(originOf(0, NEWER_SEQUENCE),
               network::ReadyState{.ready = false});

  EXPECT_FALSE(player.ready);
}

/**
 * Given a keeper and a roster without the sender
 * When a ReadyState arrives from that sender
 * Then nobody is ready
 */
TEST(ReadyKeeper, IgnoresAReadyStateFromAnAddressNoPlayerSendsFrom) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  ReadyKeeper keeper(roster, [] { return true; });

  keeper.apply(originOf(1, SOME_SEQUENCE), network::ReadyState{.ready = true});

  EXPECT_FALSE(player.ready);
}

/**
 * Given an empty roster
 * When the keeper is asked whether every player is ready
 * Then the answer is no
 */
TEST(ReadyKeeper, IsNotReadyWithoutPlayers) {
  PlayerRoster roster;
  ReadyKeeper keeper(roster, [] { return true; });

  EXPECT_FALSE(keeper.everyPlayerIsReady());
}

/**
 * Given two players, one of them ready
 * When the keeper is asked whether every player is ready
 * Then the answer is no, until the other one says ready too
 */
TEST(ReadyKeeper, WaitsForEveryConnectedPlayer) {
  PlayerRoster roster;
  static_cast<void>(addPlayer(roster, 0));
  static_cast<void>(addPlayer(roster, 1));
  ReadyKeeper keeper(roster, [] { return true; });
  keeper.apply(originOf(0, SOME_SEQUENCE), network::ReadyState{.ready = true});
  ASSERT_EQ(roster.connected().size(), TWO_PLAYERS);

  EXPECT_FALSE(keeper.everyPlayerIsReady());

  keeper.apply(originOf(1, SOME_SEQUENCE), network::ReadyState{.ready = true});

  EXPECT_TRUE(keeper.everyPlayerIsReady());
}

/**
 * Given a player who said ready in the datagram of sequence 65535
 * When a ReadyState of sequence 0 says not ready
 * Then the player is not ready, since sequence 0 follows sequence 65535
 */
TEST(ReadyKeeper, AppliesAReadyStateOnceTheSequenceWrapsAround) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  ReadyKeeper keeper(roster, [] { return true; });
  keeper.apply(originOf(0, LAST_SEQUENCE), network::ReadyState{.ready = true});

  keeper.apply(originOf(0, FIRST_SEQUENCE),
               network::ReadyState{.ready = false});

  EXPECT_FALSE(player.ready);
}

}  // namespace rtype::server
