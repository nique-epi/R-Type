#include <gtest/gtest.h>
#include <optional>
#include <vector>
#include "GamePlacement.hpp"
#include "GamePlacementTestConstants.hpp"
#include "GameVacancy.hpp"

using rtype::server::firstJoinableGame;
using rtype::server::GameVacancy;
using rtype::server::testing::FINISHED;
using rtype::server::testing::FIRST_GAME_INDEX;
using rtype::server::testing::IN_PROGRESS;
using rtype::server::testing::SECOND_GAME_INDEX;
using rtype::server::testing::THIRD_GAME_INDEX;
using rtype::server::testing::WAITING_AND_FULL;
using rtype::server::testing::WAITING_WITH_A_SEAT;

/**
 * Given two games waiting for players, each with a free seat
 * When a new player is placed
 * Then it goes to the first one
 */
TEST(GamePlacement, ChoosesTheFirstWaitingGameWithASeat) {
  const std::vector<GameVacancy> games{WAITING_WITH_A_SEAT,
                                       WAITING_WITH_A_SEAT};

  EXPECT_EQ(firstJoinableGame(games), std::optional{FIRST_GAME_INDEX});
}

/**
 * Given a full game, then a game in progress, then a waiting game with a seat
 * When a new player is placed
 * Then it goes to the third one
 */
TEST(GamePlacement, SkipsAFullGameAndAGameInProgress) {
  const std::vector<GameVacancy> games{WAITING_AND_FULL, IN_PROGRESS,
                                       WAITING_WITH_A_SEAT};

  EXPECT_EQ(firstJoinableGame(games), std::optional{THIRD_GAME_INDEX});
}

/**
 * Given a finished game, then a waiting game with a seat
 * When a new player is placed
 * Then it goes to the second one: a finished game waits again only later
 */
TEST(GamePlacement, SkipsAFinishedGame) {
  const std::vector<GameVacancy> games{FINISHED, WAITING_WITH_A_SEAT};

  EXPECT_EQ(firstJoinableGame(games), std::optional{SECOND_GAME_INDEX});
}

/**
 * Given games that are all full, in progress or finished
 * When a new player is placed
 * Then no game is found
 */
TEST(GamePlacement, FindsNoneWhenEveryGameIsTaken) {
  const std::vector<GameVacancy> games{WAITING_AND_FULL, IN_PROGRESS, FINISHED};

  EXPECT_EQ(firstJoinableGame(games), std::nullopt);
}

/**
 * Given no game
 * When a new player is placed
 * Then no game is found
 */
TEST(GamePlacement, FindsNoneInAnEmptyList) {
  EXPECT_EQ(firstJoinableGame({}), std::nullopt);
}
