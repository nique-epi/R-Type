#include <gtest/gtest.h>
#include "ConnectedPlayer.hpp"
#include "GameLoaded.hpp"
#include "MapVote.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "ReadyKeeper.hpp"
#include "ReadyState.hpp"
#include "SessionsFixtures.hpp"
#include "SessionsTestConstants.hpp"

namespace rtype::server {

using testing::addPlayer;
using testing::MAP_PAST_THE_LAST;
using testing::NEWER_SEQUENCE;
using testing::NO_MAPS;
using testing::originOf;
using testing::SECOND_MAP;
using testing::SOME_SEQUENCE;
using testing::TWO_MAPS;
using testing::TWO_PLAYERS;

/**
 * Given a keeper that accepts a ReadyState and a player who is not ready
 * When the player says ready
 * Then the player is ready
 */
TEST(ReadyKeeper, MarksThePlayerReady) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  ReadyKeeper keeper(roster, [] { return true; }, NO_MAPS);

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
  ReadyKeeper keeper(roster, [] { return false; }, NO_MAPS);

  keeper.apply(originOf(0, SOME_SEQUENCE), network::ReadyState{.ready = true});

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
  ReadyKeeper keeper(roster, [] { return true; }, NO_MAPS);

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
  ReadyKeeper keeper(roster, [] { return true; }, NO_MAPS);

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
  ReadyKeeper keeper(roster, [] { return true; }, NO_MAPS);
  keeper.apply(originOf(0, SOME_SEQUENCE), network::ReadyState{.ready = true});
  ASSERT_EQ(roster.connected().size(), TWO_PLAYERS);

  EXPECT_FALSE(keeper.everyPlayerIsReady());

  keeper.apply(originOf(1, SOME_SEQUENCE), network::ReadyState{.ready = true});

  EXPECT_TRUE(keeper.everyPlayerIsReady());
}

/**
 * Given two players, the first ready and the second, not ready, away
 * When the keeper is asked whether every player is ready
 * Then the answer is yes: a player who is away does not hold the game back
 */
TEST(ReadyKeeper, DoesNotWaitForAPlayerWhoIsAway) {
  PlayerRoster roster;
  static_cast<void>(addPlayer(roster, 0));
  ConnectedPlayer& second = addPlayer(roster, 1);
  ReadyKeeper keeper(roster, [] { return true; }, NO_MAPS);
  keeper.apply(originOf(0, SOME_SEQUENCE), network::ReadyState{.ready = true});
  second.away = true;

  EXPECT_TRUE(keeper.everyPlayerIsReady());
}

/**
 * Given one player, away
 * When the keeper is asked whether every player is ready
 * Then the answer is no: nobody is there to start the game
 */
TEST(ReadyKeeper, IsNotReadyWhenEveryPlayerIsAway) {
  PlayerRoster roster;
  ConnectedPlayer& player = addPlayer(roster, 0);
  ReadyKeeper keeper(roster, [] { return true; }, NO_MAPS);
  keeper.apply(originOf(0, SOME_SEQUENCE), network::ReadyState{.ready = true});
  player.away = true;

  EXPECT_FALSE(keeper.everyPlayerIsReady());
}

/**
 * Given a keeper that accepts no ReadyState and a player not loaded
 * When the player sends GameLoaded
 * Then the player is loaded, since GameLoaded counts in every state
 */
TEST(ReadyKeeper, MarksThePlayerLoadedWhateverTheStateOfTheGame) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  ReadyKeeper keeper(roster, [] { return false; }, NO_MAPS);

  keeper.apply(originOf(0, SOME_SEQUENCE), network::GameLoaded{});

  EXPECT_TRUE(player.loaded);
}

/**
 * Given a keeper and a roster without the sender
 * When a GameLoaded arrives from that sender
 * Then nobody is loaded
 */
TEST(ReadyKeeper, IgnoresAGameLoadedFromANonPlayer) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  ReadyKeeper keeper(roster, [] { return true; }, NO_MAPS);

  keeper.apply(originOf(1, SOME_SEQUENCE), network::GameLoaded{});

  EXPECT_FALSE(player.loaded);
}

/**
 * Given a keeper that accepts the choices of the room, with two maps
 * When a player votes for the second map
 * Then the player's vote is the second map
 */
TEST(ReadyKeeper, KeepsTheVoteOfAPlayer) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  ReadyKeeper keeper(roster, [] { return true; }, TWO_MAPS);

  keeper.apply(originOf(0, SOME_SEQUENCE), network::MapVote{.map = SECOND_MAP});

  EXPECT_EQ(player.mapVote, SECOND_MAP);
}

/**
 * Given a keeper that does not accept the choices of the room right now
 * When a player votes for the second map
 * Then the player has no vote
 */
TEST(ReadyKeeper, IgnoresAVoteItDoesNotAccept) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  ReadyKeeper keeper(roster, [] { return false; }, TWO_MAPS);

  keeper.apply(originOf(0, SOME_SEQUENCE), network::MapVote{.map = SECOND_MAP});

  EXPECT_EQ(player.mapVote, network::NO_MAP_VOTE);
}

/**
 * Given a keeper with two maps, and a player who voted for the second
 * When the player votes for a third map, past the last
 * Then the player keeps its vote for the second
 */
TEST(ReadyKeeper, IgnoresAVotePastTheLastMap) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  ReadyKeeper keeper(roster, [] { return true; }, TWO_MAPS);
  keeper.apply(originOf(0, SOME_SEQUENCE), network::MapVote{.map = SECOND_MAP});

  keeper.apply(originOf(0, NEWER_SEQUENCE),
               network::MapVote{.map = MAP_PAST_THE_LAST});

  EXPECT_EQ(player.mapVote, SECOND_MAP);
}

/**
 * Given a keeper with two maps, and a player who voted for the second
 * When the player votes for none
 * Then the player has no vote
 */
TEST(ReadyKeeper, TakesBackAVote) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  ReadyKeeper keeper(roster, [] { return true; }, TWO_MAPS);
  keeper.apply(originOf(0, SOME_SEQUENCE), network::MapVote{.map = SECOND_MAP});

  keeper.apply(originOf(0, NEWER_SEQUENCE),
               network::MapVote{.map = network::NO_MAP_VOTE});

  EXPECT_EQ(player.mapVote, network::NO_MAP_VOTE);
}

/**
 * Given a keeper with two maps and a roster without the sender
 * When a vote arrives from that sender
 * Then nobody has a vote
 */
TEST(ReadyKeeper, IgnoresAVoteFromANonPlayer) {
  PlayerRoster roster;
  const ConnectedPlayer& player = addPlayer(roster, 0);
  ReadyKeeper keeper(roster, [] { return true; }, TWO_MAPS);

  keeper.apply(originOf(1, SOME_SEQUENCE), network::MapVote{.map = SECOND_MAP});

  EXPECT_EQ(player.mapVote, network::NO_MAP_VOTE);
}

}  // namespace rtype::server
