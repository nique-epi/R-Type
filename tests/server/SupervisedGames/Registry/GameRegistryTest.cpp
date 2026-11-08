#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "ConnectionRefused.hpp"
#include "ConnectionRequest.hpp"
#include "GameLifecycle.hpp"
#include "GameOpening.hpp"
#include "GamePlace.hpp"
#include "GameRegistry.hpp"
#include "GameStatus.hpp"
#include "PlaceKind.hpp"
#include "ProcessExit.hpp"
#include "RecordingChildState.hpp"
#include "SeatRequest.hpp"
#include "ServerException.hpp"
#include "SupervisedGame.hpp"
#include "SupervisedGamesFixtures.hpp"
#include "SupervisedGamesReportTestConstants.hpp"
#include "SupervisedGamesTestConstants.hpp"

using rtype::network::ANY_GAME_CHOICE;
using rtype::network::GameStatus;
using rtype::network::GameStatusState;
using rtype::network::NEW_GAME_CHOICE;
using rtype::network::RefusalReason;
using rtype::server::GameLifecycle;
using rtype::server::GameOpening;
using rtype::server::GamePlace;
using rtype::server::GameRegistry;
using rtype::server::NoFreeGameSlotException;
using rtype::server::PlaceKind;
using rtype::server::ProcessExit;
using rtype::server::SeatRequest;
using rtype::server::SupervisedGame;

/**
 * Given a registry with no game
 * When a place is asked for a new player
 * Then a game must be opened
 */
TEST(GameRegistry, OpensAGameWhenThereIsNone) {
  GameRegistry registry;

  EXPECT_EQ(registry.placeFor(seatRequestOf(0), ANY_GAME_CHOICE).kind,
            PlaceKind::Open);
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

  const auto place = registry.placeFor(seatRequestOf(0), ANY_GAME_CHOICE);

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
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 2));

  const auto place = registry.placeFor(seatRequestOf(0), ANY_GAME_CHOICE);

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
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::InProgress, 4));
  registry.record(*registry.findByToken(tokenOfGame(1)), SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 1));

  EXPECT_EQ(registry.placeFor(seatRequestOf(0), ANY_GAME_CHOICE).port,
            portOfGame(1));
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
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 4));

  EXPECT_EQ(registry.placeFor(seatRequestOf(0), ANY_GAME_CHOICE).kind,
            PlaceKind::Open);
}

/**
 * Given eight games, all in progress
 * When a place is asked for a new player who takes any game
 * Then it is refused: the server is full
 */
TEST(GameRegistry, IsFullWhenEightGamesRunAndNoneHasAPlace) {
  GameRegistry registry;
  std::vector<std::shared_ptr<RecordingChildState>> states;
  for (std::uint32_t number = 0; number < EIGHT_GAMES; ++number) {
    states.push_back(std::make_shared<RecordingChildState>());
    registry.open(openingOf(number, states.back()));
    registry.record(*registry.findByToken(tokenOfGame(number)), SEQUENCE_FIVE,
                    statusWithPlayers(GameStatusState::InProgress, 4));
  }

  const GamePlace place = registry.placeFor(seatRequestOf(0), ANY_GAME_CHOICE);

  EXPECT_EQ(place.kind, PlaceKind::Refuse);
  EXPECT_EQ(place.refusal, RefusalReason::ServerFull);
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
    registry.record(*registry.findByToken(tokenOfGame(number)), SEQUENCE_FIVE,
                    statusWithPlayers(GameStatusState::InProgress, 4));
  }

  EXPECT_EQ(registry.placeFor(seatRequestOf(0), ANY_GAME_CHOICE).kind,
            PlaceKind::Open);
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
  registry.record(game, SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::InProgress, 3));

  const bool taken =
      registry.record(game, SEQUENCE_FOUR,
                      statusWithPlayers(GameStatusState::WaitingForPlayers, 0));

  EXPECT_FALSE(taken);
  EXPECT_EQ(game.lifecycle, GameLifecycle::InProgress);
  EXPECT_EQ(game.nicknames.size(), 3U);
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
  registry.record(game, LAST_SEQUENCE,
                  statusWithPlayers(GameStatusState::InProgress, 3));

  EXPECT_TRUE(registry.record(game, FIRST_SEQUENCE,
                              statusWithPlayers(GameStatusState::Finished, 3)));
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
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 0));
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
  state->exit = ProcessExit{.code = CRASH_EXIT_CODE};

  const auto exits = registry.collectExits();

  ASSERT_EQ(exits.size(), 1U);
  EXPECT_TRUE(exits.front().crashed);
  EXPECT_EQ(exits.front().exitCode, CRASH_EXIT_CODE);
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
  first->exit = ProcessExit{.code = CRASH_EXIT_CODE};

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
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 0));

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

/**
 * Given a registry with no game
 * When two games are opened
 * Then the first takes place 1 and the second place 2
 */
TEST(GameRegistry, GivesEachNewGameTheLowestFreePlace) {
  GameRegistry registry;
  auto first = std::make_shared<RecordingChildState>();
  auto second = std::make_shared<RecordingChildState>();

  registry.open(openingOf(0, first));
  registry.open(openingOf(1, second));

  ASSERT_EQ(registry.all().size(), 2U);
  EXPECT_EQ(registry.all()[0].slot, FIRST_PLACE);
  EXPECT_EQ(registry.all()[1].slot, SECOND_PLACE);
}

/**
 * Given three games, of which the one in place 1 ended and was collected
 * When a fourth game is opened
 * Then it takes place 1 again, while its number is a new one
 */
TEST(GameRegistry, GivesAgainThePlaceOfAGameThatLeft) {
  GameRegistry registry;
  std::vector<std::shared_ptr<RecordingChildState>> states;
  for (std::uint32_t number = 0; number < THREE_GAMES; ++number) {
    states.push_back(std::make_shared<RecordingChildState>());
    registry.open(openingOf(number, states.back()));
  }
  states.front()->exit = ProcessExit{.code = 0};
  static_cast<void>(registry.collectExits());
  states.push_back(std::make_shared<RecordingChildState>());

  registry.open(openingOf(THREE_GAMES, states.back()));

  EXPECT_EQ(registry.all().back().slot, FIRST_PLACE);
  EXPECT_EQ(registry.all().back().number, THREE_GAMES);
}

/**
 * Given eight games, one in each place
 * When a ninth game is opened
 * Then it is refused and the registry keeps eight games
 */
TEST(GameRegistry, RefusesAGameWhenEveryPlaceHoldsOne) {
  GameRegistry registry;
  std::vector<std::shared_ptr<RecordingChildState>> states;
  for (std::uint32_t number = 0; number <= EIGHT_GAMES; ++number) {
    states.push_back(std::make_shared<RecordingChildState>());
  }
  for (std::uint32_t number = 0; number < EIGHT_GAMES; ++number) {
    registry.open(openingOf(number, states.at(number)));
  }

  EXPECT_THROW(registry.open(openingOf(static_cast<std::uint32_t>(EIGHT_GAMES),
                                       states.back())),
               NoFreeGameSlotException);
  EXPECT_EQ(registry.all().size(), EIGHT_GAMES);
}

/**
 * Given a game that reported it waits for players, with none yet
 * When four players ask for a place before its next status, then a fifth
 * Then the four are sent to that game and the fifth must open another: the
 * seats given count before the game reports their players
 */
TEST(GameRegistry, CountsTheSeatsItGaveBeforeTheGameReportsThem) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 0));
  for (std::size_t player = 0; player < FOUR_PLAYERS; ++player) {
    ASSERT_EQ(registry.placeFor(seatRequestOf(player), ANY_GAME_CHOICE).kind,
              PlaceKind::Redirect);
  }

  EXPECT_EQ(
      registry.placeFor(seatRequestOf(FIFTH_PLAYER), ANY_GAME_CHOICE).kind,
      PlaceKind::Open);
}

/**
 * Given a game that was just opened, nothing heard from it, and four players
 * told to wait for it
 * When a fifth player asks for a place
 * Then another game must be opened
 */
TEST(GameRegistry, CountsThePlayersToldToWaitForAGameThatStarts) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  for (std::size_t player = 0; player < FOUR_PLAYERS; ++player) {
    ASSERT_EQ(registry.placeFor(seatRequestOf(player), ANY_GAME_CHOICE).kind,
              PlaceKind::Wait);
  }

  EXPECT_EQ(
      registry.placeFor(seatRequestOf(FIFTH_PLAYER), ANY_GAME_CHOICE).kind,
      PlaceKind::Open);
}

/**
 * Given a game that waits for players, with none yet, and a player that asked
 * twice
 * When three other players ask for a place
 * Then the last of them is still sent to that game: a sender holds one seat
 */
TEST(GameRegistry, GivesASenderThatAsksAgainNoSecondSeat) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 0));
  static_cast<void>(registry.placeFor(seatRequestOf(0), ANY_GAME_CHOICE));
  static_cast<void>(registry.placeFor(seatRequestOf(0), ANY_GAME_CHOICE));
  static_cast<void>(registry.placeFor(seatRequestOf(1), ANY_GAME_CHOICE));
  static_cast<void>(registry.placeFor(seatRequestOf(2), ANY_GAME_CHOICE));

  const auto place = registry.placeFor(seatRequestOf(3), ANY_GAME_CHOICE);

  EXPECT_EQ(place.kind, PlaceKind::Redirect);
  EXPECT_EQ(place.port, portOfGame(0));
}

/**
 * Given a game of four seats given and no player yet, then a game opened
 * after it
 * When the first player asks again
 * Then it is sent to the game that holds its seat, not to the other one
 */
TEST(GameRegistry, SendsASenderThatAsksAgainToTheGameOfItsSeat) {
  GameRegistry registry;
  auto first = std::make_shared<RecordingChildState>();
  auto second = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, first));
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 0));
  for (std::size_t player = 0; player < FOUR_PLAYERS; ++player) {
    static_cast<void>(
        registry.placeFor(seatRequestOf(player), ANY_GAME_CHOICE));
  }
  registry.open(openingOf(1, second));

  EXPECT_EQ(registry.placeFor(seatRequestOf(0), ANY_GAME_CHOICE).port,
            portOfGame(0));
}

/**
 * Given a game that waits for players and gave two seats
 * When its next status names those two players, then two more players ask
 * Then both are sent to it: a player seated holds no seat of its own anymore
 */
TEST(GameRegistry, EndsTheSeatsOfThePlayersTheGameReports) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  SupervisedGame& game = *registry.findByToken(tokenOfGame(0));
  registry.record(game, SEQUENCE_FOUR,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 0));
  static_cast<void>(registry.placeFor(seatRequestOf(0), ANY_GAME_CHOICE));
  static_cast<void>(registry.placeFor(seatRequestOf(1), ANY_GAME_CHOICE));
  registry.record(game, SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 2));
  static_cast<void>(registry.placeFor(seatRequestOf(2), ANY_GAME_CHOICE));

  EXPECT_EQ(registry.placeFor(seatRequestOf(3), ANY_GAME_CHOICE).kind,
            PlaceKind::Redirect);
}

/**
 * Given a game that gave its four seats, to players not seated yet
 * When a newer status that names none of them arrives, then a fifth player
 * asks
 * Then another game must be opened: that status left before they sat down
 */
TEST(GameRegistry, KeepsTheSeatsAStatusDoesNotNameThePlayersOf) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  SupervisedGame& game = *registry.findByToken(tokenOfGame(0));
  registry.record(game, SEQUENCE_FOUR,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 0));
  for (std::size_t player = 0; player < FOUR_PLAYERS; ++player) {
    static_cast<void>(
        registry.placeFor(seatRequestOf(player), ANY_GAME_CHOICE));
  }

  registry.record(game, SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 0));

  EXPECT_EQ(
      registry.placeFor(seatRequestOf(FIFTH_PLAYER), ANY_GAME_CHOICE).kind,
      PlaceKind::Open);
}

/**
 * Given a game whose status of sequence 5 was taken, then four seats given
 * When a late status of sequence 4 naming the four players arrives, then a
 * fifth player asks
 * Then another game must be opened: a status that is not taken ends no seat
 */
TEST(GameRegistry, ALateStatusEndsNoSeat) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  SupervisedGame& game = *registry.findByToken(tokenOfGame(0));
  registry.record(game, SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 0));
  for (std::size_t player = 0; player < FOUR_PLAYERS; ++player) {
    static_cast<void>(
        registry.placeFor(seatRequestOf(player), ANY_GAME_CHOICE));
  }

  static_cast<void>(registry.record(
      game, SEQUENCE_FOUR,
      statusWithPlayers(GameStatusState::WaitingForPlayers, FOUR_PLAYERS)));

  EXPECT_EQ(
      registry.placeFor(seatRequestOf(FIFTH_PLAYER), ANY_GAME_CHOICE).kind,
      PlaceKind::Open);
}

/**
 * Given a game that gave its four seats to players that never came
 * When three seconds pass, then a fifth player asks
 * Then it is sent to that game: the seats are free again
 */
TEST(GameRegistry, FreesTheSeatsOfPlayersThatNeverCameAfterThreeSeconds) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 0));
  for (std::size_t player = 0; player < FOUR_PLAYERS; ++player) {
    static_cast<void>(
        registry.placeFor(seatRequestOf(player), ANY_GAME_CHOICE));
  }

  registry.terminateStale(THREE_SECONDS_TO_SIT);

  EXPECT_EQ(
      registry.placeFor(seatRequestOf(FIFTH_PLAYER), ANY_GAME_CHOICE).kind,
      PlaceKind::Redirect);
}

/**
 * Given a game that gave its four seats to players not seated yet
 * When just under three seconds pass, then a fifth player asks
 * Then another game must be opened: the seats are still held
 */
TEST(GameRegistry, KeepsTheSeatsJustUnderThreeSeconds) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 0));
  for (std::size_t player = 0; player < FOUR_PLAYERS; ++player) {
    static_cast<void>(
        registry.placeFor(seatRequestOf(player), ANY_GAME_CHOICE));
  }

  registry.terminateStale(JUST_UNDER_THREE_SECONDS_TO_SIT);

  EXPECT_EQ(
      registry.placeFor(seatRequestOf(FIFTH_PLAYER), ANY_GAME_CHOICE).kind,
      PlaceKind::Open);
}

/**
 * Given a game with PILOT1 seated, then a seat given to another sender that
 * asks as PILOT1 and seats given to PILOT2 and PILOT3
 * When a newer status that still names PILOT1 alone arrives, then PILOT4 asks
 * Then another game must be opened: the namesake has not sat down yet
 */
TEST(GameRegistry, KeepsTheSeatOfANamesakeOfAPlayerSeated) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  SupervisedGame& game = *registry.findByToken(tokenOfGame(0));
  registry.record(game, SEQUENCE_FOUR,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 1));
  static_cast<void>(registry.placeFor(
      SeatRequest{.sender = supervisedPlayer(FIFTH_PLAYER),
                  .nickname = std::string{SUPERVISED_NICKNAMES.front()}},
      ANY_GAME_CHOICE));
  static_cast<void>(registry.placeFor(seatRequestOf(1), ANY_GAME_CHOICE));
  static_cast<void>(registry.placeFor(seatRequestOf(2), ANY_GAME_CHOICE));

  registry.record(game, SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 1));

  EXPECT_EQ(registry.placeFor(seatRequestOf(3), ANY_GAME_CHOICE).kind,
            PlaceKind::Open);
}

/**
 * Given two games that wait for players, each with a free seat
 * When a player asks for the game in the second place
 * Then it is sent to that game, though the first one has a seat
 */
TEST(GameRegistry, SendsAPlayerToTheGameItNames) {
  GameRegistry registry;
  openReporting(registry, 0,
                statusWithPlayers(GameStatusState::WaitingForPlayers, 1));
  openReporting(registry, 1,
                statusWithPlayers(GameStatusState::WaitingForPlayers, 1));

  const GamePlace place =
      registry.placeFor(seatRequestOf(0), choiceOfGame(SECOND_PLACE));

  EXPECT_EQ(place.kind, PlaceKind::Redirect);
  EXPECT_EQ(place.port, portOfGame(1));
}

/**
 * Given a game that was just opened, nothing heard from it
 * When a player asks for the game in its place
 * Then the player waits for it
 */
TEST(GameRegistry, MakesAPlayerWaitForTheGameItNamesWhileItStarts) {
  GameRegistry registry;
  registry.open(openingOf(0, std::make_shared<RecordingChildState>()));

  const GamePlace place =
      registry.placeFor(seatRequestOf(0), choiceOfGame(FIRST_PLACE));

  EXPECT_EQ(place.kind, PlaceKind::Wait);
  EXPECT_EQ(place.port, portOfGame(0));
}

/**
 * Given one game, in the first place
 * When a player asks for the game in the second place
 * Then it is refused: no such game
 */
TEST(GameRegistry, RefusesAGameNoGameHolds) {
  GameRegistry registry;
  openReporting(registry, 0,
                statusWithPlayers(GameStatusState::WaitingForPlayers, 1));

  const GamePlace place =
      registry.placeFor(seatRequestOf(0), choiceOfGame(SECOND_PLACE));

  EXPECT_EQ(place.kind, PlaceKind::Refuse);
  EXPECT_EQ(place.refusal, RefusalReason::GameNotFound);
}

/**
 * Given a game in progress with four players
 * When a player asks for it
 * Then it is refused because the game is in progress, not because it is
 * full: its state is read before its seats
 */
TEST(GameRegistry, RefusesAGameInProgressBeforeCountingItsSeats) {
  GameRegistry registry;
  openReporting(registry, 0,
                statusWithPlayers(GameStatusState::InProgress, FOUR_PLAYERS));

  const GamePlace place =
      registry.placeFor(seatRequestOf(0), choiceOfGame(FIRST_PLACE));

  EXPECT_EQ(place.kind, PlaceKind::Refuse);
  EXPECT_EQ(place.refusal, RefusalReason::GameInProgress);
}

/**
 * Given a game that finished
 * When a player asks for it
 * Then it is refused because the game no longer waits for players
 */
TEST(GameRegistry, RefusesAGameThatFinished) {
  GameRegistry registry;
  openReporting(registry, 0, statusWithPlayers(GameStatusState::Finished, 1));

  EXPECT_EQ(
      registry.placeFor(seatRequestOf(0), choiceOfGame(FIRST_PLACE)).refusal,
      RefusalReason::GameInProgress);
}

/**
 * Given a game that waits with three players and a seat given to a fourth
 * When a fifth player asks for that game
 * Then it is refused as full: the seat held counts
 */
TEST(GameRegistry, RefusesAFullGameCountingTheSeatsItHolds) {
  GameRegistry registry;
  openReporting(registry, 0,
                statusWithPlayers(GameStatusState::WaitingForPlayers, 3));
  ASSERT_EQ(registry.placeFor(seatRequestOf(3), ANY_GAME_CHOICE).kind,
            PlaceKind::Redirect);

  const GamePlace place =
      registry.placeFor(seatRequestOf(FIFTH_PLAYER), choiceOfGame(FIRST_PLACE));

  EXPECT_EQ(place.kind, PlaceKind::Refuse);
  EXPECT_EQ(place.refusal, RefusalReason::GameFull);
}

/**
 * Given a game that waits with three players
 * When a fourth player asks for it, then asks again
 * Then both requests send it to that game: it holds the last seat
 */
TEST(GameRegistry, GivesTheLastSeatOfTheGameItNames) {
  GameRegistry registry;
  openReporting(registry, 0,
                statusWithPlayers(GameStatusState::WaitingForPlayers, 3));

  const GamePlace first =
      registry.placeFor(seatRequestOf(3), choiceOfGame(FIRST_PLACE));
  const GamePlace again =
      registry.placeFor(seatRequestOf(3), choiceOfGame(FIRST_PLACE));

  EXPECT_EQ(first.kind, PlaceKind::Redirect);
  EXPECT_EQ(again.kind, PlaceKind::Redirect);
}

/**
 * Given a game that waits for players and has free seats
 * When a player asks for a new game
 * Then a game must be opened for it
 */
TEST(GameRegistry, OpensANewGameEvenWhenAGameHasASeat) {
  GameRegistry registry;
  openReporting(registry, 0,
                statusWithPlayers(GameStatusState::WaitingForPlayers, 1));

  EXPECT_EQ(registry.placeFor(seatRequestOf(0), NEW_GAME_CHOICE).kind,
            PlaceKind::Open);
}

/**
 * Given a game opened for the new game of a player, nothing heard from it
 * When the player asks for a new game again, twice
 * Then it waits for that game each time: its requests open one game
 */
TEST(GameRegistry, SendsARepeatedNewGameToTheGameItOpened) {
  GameRegistry registry;
  registry.open(openingOf(0, std::make_shared<RecordingChildState>(),
                          supervisedPlayer(0)));

  const GamePlace first = registry.placeFor(seatRequestOf(0), NEW_GAME_CHOICE);
  const GamePlace again = registry.placeFor(seatRequestOf(0), NEW_GAME_CHOICE);

  EXPECT_EQ(first.kind, PlaceKind::Wait);
  EXPECT_EQ(again.kind, PlaceKind::Wait);
  EXPECT_EQ(again.port, portOfGame(0));
}

/**
 * Given a game opened for the new game of a player, who holds a seat in it,
 * and another player sent there for any game
 * When the first player asks for a new game again
 * Then it waits for the same game: the game it opened still holds its seat
 */
TEST(GameRegistry, KeepsTheCreatorInItsGameWhileItHoldsASeat) {
  GameRegistry registry;
  registry.open(openingOf(0, std::make_shared<RecordingChildState>(),
                          supervisedPlayer(0)));
  ASSERT_EQ(registry.placeFor(seatRequestOf(0), NEW_GAME_CHOICE).kind,
            PlaceKind::Wait);
  ASSERT_EQ(registry.placeFor(seatRequestOf(1), ANY_GAME_CHOICE).kind,
            PlaceKind::Wait);

  const GamePlace again = registry.placeFor(seatRequestOf(0), NEW_GAME_CHOICE);

  EXPECT_EQ(again.kind, PlaceKind::Wait);
  EXPECT_EQ(again.port, portOfGame(0));
}

/**
 * Given a game opened for the new game of a player, in which another player
 * sat down, the first one holding no seat
 * When the first player asks for a new game again
 * Then another game must be opened: somebody joined the one it opened
 */
TEST(GameRegistry, OpensAnotherGameOnceSomebodyJoined) {
  GameRegistry registry;
  registry.open(openingOf(0, std::make_shared<RecordingChildState>(),
                          supervisedPlayer(0)));
  registry.record(
      *registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
      GameStatus{.state = GameStatusState::WaitingForPlayers,
                 .nicknames = {std::string{SUPERVISED_NICKNAMES[1]}},
                 .measures = {}});

  EXPECT_EQ(registry.placeFor(seatRequestOf(0), NEW_GAME_CHOICE).kind,
            PlaceKind::Open);
}

/**
 * Given a game opened for the new game of a player, who then sat down in it
 * When the player asks for a new game again
 * Then another game must be opened: the first one has its creator seated
 */
TEST(GameRegistry, OpensAnotherGameOnceTheCreatorSatDown) {
  GameRegistry registry;
  registry.open(openingOf(0, std::make_shared<RecordingChildState>(),
                          supervisedPlayer(0)));
  ASSERT_EQ(registry.placeFor(seatRequestOf(0), NEW_GAME_CHOICE).kind,
            PlaceKind::Wait);
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 1));

  EXPECT_EQ(registry.placeFor(seatRequestOf(0), NEW_GAME_CHOICE).kind,
            PlaceKind::Open);
}

/**
 * Given a game opened for the new game of a player, that waits for players
 * When another player asks for any game
 * Then it is sent to that game: a game opened for a new game is open to all
 */
TEST(GameRegistry, SendsAnyGameToAGameOpenedForANewGame) {
  GameRegistry registry;
  registry.open(openingOf(0, std::make_shared<RecordingChildState>(),
                          supervisedPlayer(0)));
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 1));

  const GamePlace place = registry.placeFor(seatRequestOf(1), ANY_GAME_CHOICE);

  EXPECT_EQ(place.kind, PlaceKind::Redirect);
  EXPECT_EQ(place.port, portOfGame(0));
}

/**
 * Given eight games that wait for players, each with a free seat
 * When a player asks for a new game
 * Then it is refused: the server runs as many games as it may
 */
TEST(GameRegistry, RefusesANewGameWhenEightGamesRun) {
  GameRegistry registry;
  for (std::uint32_t number = 0; number < EIGHT_GAMES; ++number) {
    openReporting(registry, number,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 1));
  }

  const GamePlace place = registry.placeFor(seatRequestOf(0), NEW_GAME_CHOICE);

  EXPECT_EQ(place.kind, PlaceKind::Refuse);
  EXPECT_EQ(place.refusal, RefusalReason::ServerFull);
}

/**
 * Given seven games that wait for players, each with a free seat
 * When a player asks for a new game
 * Then an eighth game may be opened for it
 */
TEST(GameRegistry, OpensANewGameWhenSevenGamesRun) {
  GameRegistry registry;
  for (std::uint32_t number = 0; number + 1 < EIGHT_GAMES; ++number) {
    openReporting(registry, number,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 1));
  }

  EXPECT_EQ(registry.placeFor(seatRequestOf(0), NEW_GAME_CHOICE).kind,
            PlaceKind::Open);
}

/**
 * Given a game opened for the new game of a player with rules it chose, waiting
 * for players with one
 * When another player asks for any game
 * Then a new game must be opened: a player who takes any game is never sent to
 * rules it did not choose
 */
TEST(GameRegistry, AnyGameSkipsAGameWithOtherRules) {
  GameRegistry registry;
  GameOpening opening = openingOf(0, std::make_shared<RecordingChildState>(),
                                  supervisedPlayer(0));
  opening.rules = chosenRules();
  registry.open(std::move(opening));
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 1));

  EXPECT_EQ(registry.placeFor(seatRequestOf(1), ANY_GAME_CHOICE).kind,
            PlaceKind::Open);
}

/**
 * Given a game opened for the new game of a player with the default rules,
 * waiting for players with one
 * When another player asks for any game
 * Then it is sent to that game
 */
TEST(GameRegistry, AnyGameJoinsANewGameWithTheDefaultRules) {
  GameRegistry registry;
  registry.open(openingOf(0, std::make_shared<RecordingChildState>(),
                          supervisedPlayer(0)));
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 1));

  const GamePlace place = registry.placeFor(seatRequestOf(1), ANY_GAME_CHOICE);

  EXPECT_EQ(place.kind, PlaceKind::Redirect);
  EXPECT_EQ(place.port, portOfGame(0));
}

/**
 * Given a game with rules its creator chose, waiting for players with one
 * When another player asks for that game by its place
 * Then it is sent to that game: a player who names a game takes its rules
 */
TEST(GameRegistry, ANamedGameIsJoinedWhateverItsRules) {
  GameRegistry registry;
  GameOpening opening = openingOf(0, std::make_shared<RecordingChildState>(),
                                  supervisedPlayer(0));
  opening.rules = chosenRules();
  registry.open(std::move(opening));
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 1));

  EXPECT_EQ(registry.placeFor(seatRequestOf(1), choiceOfGame(FIRST_PLACE)).kind,
            PlaceKind::Redirect);
}
