#include <gtest/gtest.h>
#include <cstdint>
#include <memory>
#include <vector>
#include "GameRegistry.hpp"
#include "GameStatus.hpp"
#include "PlaceKind.hpp"
#include "ProcessExit.hpp"
#include "RecordingChildState.hpp"
#include "SupervisedGame.hpp"
#include "SupervisedGamesFixtures.hpp"
#include "SupervisedGamesTestConstants.hpp"

using rtype::network::GameStatus;
using rtype::network::GameStatusState;
using rtype::server::GameLifecycle;
using rtype::server::GameRegistry;
using rtype::server::PlaceKind;
using rtype::server::ProcessExit;
using rtype::server::SupervisedGame;

/**
 * Given a registry with no game
 * When a place is asked for a new player
 * Then a game must be opened
 */
TEST(GameRegistry, OpensAGameWhenThereIsNone) {
  const GameRegistry registry;

  EXPECT_EQ(registry.placeForNewPlayer().kind, PlaceKind::Open);
}

/**
 * Given a game that was just opened, nothing heard from it
 * When a place is asked for a new player
 * Then the player waits, and the place names the port of that game
 */
TEST(GameRegistry, MakesAPlayerWaitForAGameThatStarts) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));

  const auto place = registry.placeForNewPlayer();

  EXPECT_EQ(place.kind, PlaceKind::Wait);
  EXPECT_EQ(place.port, portOfGame(0));
}

/**
 * Given a game that reported it waits for players, with two of them
 * When a place is asked for a new player
 * Then the player is sent to that game's port
 */
TEST(GameRegistry, SendsAPlayerToAGameThatWaitsForPlayers) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  GameStatus{.state = GameStatusState::WaitingForPlayers,
                             .playerCount = 2});

  const auto place = registry.placeForNewPlayer();

  EXPECT_EQ(place.kind, PlaceKind::Redirect);
  EXPECT_EQ(place.port, portOfGame(0));
}

/**
 * Given a first game in progress and a second that waits for players
 * When a place is asked for a new player
 * Then the player goes to the second game
 */
TEST(GameRegistry, SkipsAGameInProgress) {
  GameRegistry registry;
  auto first = std::make_shared<RecordingChildState>();
  auto second = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, first));
  registry.open(openingOf(1, second));
  registry.record(
      *registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
      GameStatus{.state = GameStatusState::InProgress, .playerCount = 4});
  registry.record(*registry.findByToken(tokenOfGame(1)), SEQUENCE_FIVE,
                  GameStatus{.state = GameStatusState::WaitingForPlayers,
                             .playerCount = 1});

  EXPECT_EQ(registry.placeForNewPlayer().port, portOfGame(1));
}

/**
 * Given a game that waits for players and already holds four
 * When a place is asked for a new player
 * Then another game must be opened
 */
TEST(GameRegistry, SkipsAFullGame) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  GameStatus{.state = GameStatusState::WaitingForPlayers,
                             .playerCount = 4});

  EXPECT_EQ(registry.placeForNewPlayer().kind, PlaceKind::Open);
}

/**
 * Given eight games, all in progress
 * When a place is asked for a new player
 * Then it is full
 */
TEST(GameRegistry, IsFullWhenEightGamesRunAndNoneHasAPlace) {
  GameRegistry registry;
  std::vector<std::shared_ptr<RecordingChildState>> states;
  for (std::uint32_t number = 0; number < EIGHT_GAMES; ++number) {
    states.push_back(std::make_shared<RecordingChildState>());
    registry.open(openingOf(number, states.back()));
    registry.record(
        *registry.findByToken(tokenOfGame(number)), SEQUENCE_FIVE,
        GameStatus{.state = GameStatusState::InProgress, .playerCount = 4});
  }

  EXPECT_EQ(registry.placeForNewPlayer().kind, PlaceKind::Full);
}

/**
 * Given seven games, all in progress
 * When a place is asked for a new player
 * Then an eighth game may be opened
 */
TEST(GameRegistry, StillOpensAGameWhenSevenRun) {
  GameRegistry registry;
  std::vector<std::shared_ptr<RecordingChildState>> states;
  for (std::uint32_t number = 0; number < EIGHT_GAMES - 1; ++number) {
    states.push_back(std::make_shared<RecordingChildState>());
    registry.open(openingOf(number, states.back()));
    registry.record(
        *registry.findByToken(tokenOfGame(number)), SEQUENCE_FIVE,
        GameStatus{.state = GameStatusState::InProgress, .playerCount = 4});
  }

  EXPECT_EQ(registry.placeForNewPlayer().kind, PlaceKind::Open);
}

/**
 * Given a game whose status of sequence 5 was taken
 * When a status of sequence 4 arrives
 * Then it is ignored and the game keeps the state of sequence 5
 */
TEST(GameRegistry, IgnoresAStatusOlderThanTheNewestOne) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  SupervisedGame& game = *registry.findByToken(tokenOfGame(0));
  registry.record(
      game, SEQUENCE_FIVE,
      GameStatus{.state = GameStatusState::InProgress, .playerCount = 3});

  const bool taken =
      registry.record(game, SEQUENCE_FOUR,
                      GameStatus{.state = GameStatusState::WaitingForPlayers,
                                 .playerCount = 0});

  EXPECT_FALSE(taken);
  EXPECT_EQ(game.lifecycle, GameLifecycle::InProgress);
  EXPECT_EQ(game.playerCount, 3);
}

/**
 * Given a game whose newest status has sequence 65535
 * When a status of sequence 0 arrives
 * Then it is taken: 0 follows 65535
 */
TEST(GameRegistry, TakesSequenceZeroAfterTheLastSequence) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  SupervisedGame& game = *registry.findByToken(tokenOfGame(0));
  registry.record(
      game, LAST_SEQUENCE,
      GameStatus{.state = GameStatusState::InProgress, .playerCount = 3});

  EXPECT_TRUE(registry.record(
      game, FIRST_SEQUENCE,
      GameStatus{.state = GameStatusState::Finished, .playerCount = 3}));
}

/**
 * Given a registry with one game
 * When a game is looked for by its own token and by another token
 * Then the first lookup finds it and the second finds none
 */
TEST(GameRegistry, FindsAGameByItsTokenOnly) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));

  EXPECT_NE(registry.findByToken(tokenOfGame(0)), nullptr);
  EXPECT_EQ(registry.findByToken(UNKNOWN_LINK_TOKEN), nullptr);
}

/**
 * Given a game that reported, then ended with the code 0
 * When the exits are collected
 * Then the game left the registry and did not crash
 */
TEST(GameRegistry, CollectsAGameThatEndedByItself) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  GameStatus{.state = GameStatusState::WaitingForPlayers,
                             .playerCount = 0});
  state->exit = ProcessExit{.code = 0};

  const auto exits = registry.collectExits();

  ASSERT_EQ(exits.size(), 1U);
  EXPECT_FALSE(exits.front().crashed);
  EXPECT_TRUE(registry.all().empty());
}

/**
 * Given a game that ended with the code 11
 * When the exits are collected
 * Then the game is said to have crashed and left the registry
 */
TEST(GameRegistry, CollectsAGameThatCrashed) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  state->exit = ProcessExit{.code = 11};

  const auto exits = registry.collectExits();

  ASSERT_EQ(exits.size(), 1U);
  EXPECT_TRUE(exits.front().crashed);
  EXPECT_EQ(exits.front().exitCode, 11);
}

/**
 * Given a game that ended with the code 0 before it ever reported
 * When the exits are collected
 * Then it is said to have crashed
 */
TEST(GameRegistry, ACleanExitBeforeAnyStatusIsACrash) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  state->exit = ProcessExit{.code = 0};

  EXPECT_TRUE(registry.collectExits().front().crashed);
}

/**
 * Given two games of which only the first ended
 * When the exits are collected
 * Then the second stays in the registry
 */
TEST(GameRegistry, KeepsTheGamesThatStillRun) {
  GameRegistry registry;
  auto first = std::make_shared<RecordingChildState>();
  auto second = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, first));
  registry.open(openingOf(1, second));
  first->exit = ProcessExit{.code = 11};

  static_cast<void>(registry.collectExits());

  ASSERT_EQ(registry.all().size(), 1U);
  EXPECT_EQ(registry.all().front().number, 1U);
}

/**
 * Given a game that starts
 * When time passes up to just under the start timeout
 * Then it is not terminated
 */
TEST(GameRegistry, DoesNotTerminateAGameJustUnderTheStartTimeout) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));

  registry.terminateStale(JUST_UNDER_TEN_SECONDS_TO_START);

  EXPECT_FALSE(state->terminated);
}

/**
 * Given a game that starts
 * When ten seconds pass without a status
 * Then it is terminated
 */
TEST(GameRegistry, TerminatesAGameThatDoesNotStartInTenSeconds) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));

  registry.terminateStale(TEN_SECONDS_TO_START);

  EXPECT_TRUE(state->terminated);
}

/**
 * Given a game that reported it waits for players
 * When ten seconds pass
 * Then it is not terminated
 */
TEST(GameRegistry, NeverTerminatesAGameThatReported) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  GameStatus{.state = GameStatusState::WaitingForPlayers,
                             .playerCount = 0});

  registry.terminateStale(TEN_SECONDS_TO_START);

  EXPECT_FALSE(state->terminated);
}

/**
 * Given two games
 * When every game is terminated
 * Then both processes were terminated
 */
TEST(GameRegistry, TerminatesEveryGame) {
  GameRegistry registry;
  auto first = std::make_shared<RecordingChildState>();
  auto second = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, first));
  registry.open(openingOf(1, second));

  registry.terminateAll();

  EXPECT_TRUE(first->terminated);
  EXPECT_TRUE(second->terminated);
}
