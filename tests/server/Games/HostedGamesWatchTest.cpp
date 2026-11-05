#include <gtest/gtest.h>
#include "GameState.hpp"
#include "GameStateChanged.hpp"
#include "HostedGameState.hpp"
#include "HostedGames.hpp"
#include "HostedGamesWatchFixture.hpp"
#include "HostedGamesWatchTestConstants.hpp"
#include "PlayerArrival.hpp"
#include "PlayerDeparture.hpp"

using rtype::game::DepartureCause;
using rtype::game::PlayerArrival;
using rtype::game::PlayerDeparture;
using rtype::network::HostedGameState;
using rtype::server::GameState;
using rtype::server::GameStateChanged;
using rtype::server::hostedStateOf;
using rtype::server::testing::ARRIVING_PLAYER;
using rtype::server::testing::FIRST_LIST;
using rtype::server::testing::HostedGamesWatchFixture;
using rtype::server::testing::NO_LIST;
using rtype::server::testing::SECOND_LIST;

/**
 * Given the states of a game
 * When the master names them
 * Then waiting, starting, playing and finished match them in that order
 */
TEST(HostedGames, NamesEachStateOfAGameAsTheMasterDoes) {
  EXPECT_EQ(hostedStateOf(GameState::WaitingForPlayers),
            HostedGameState::Waiting);
  EXPECT_EQ(hostedStateOf(GameState::Starting), HostedGameState::Starting);
  EXPECT_EQ(hostedStateOf(GameState::InProgress), HostedGameState::Playing);
  EXPECT_EQ(hostedStateOf(GameState::Finished), HostedGameState::Finished);
}

/**
 * Given a watch that published nothing yet
 * When it is asked to publish
 * Then it publishes the list, the first one
 */
TEST_F(HostedGamesWatchFixture, PublishesTheListTheFirstTime) {
  ASSERT_EQ(listsPublished(), NO_LIST);

  watch().publishIfChanged(games());

  EXPECT_EQ(listsPublished(), FIRST_LIST);
}

/**
 * Given a watch that published its list
 * When it is asked again with no event in between
 * Then it publishes nothing more
 */
TEST_F(HostedGamesWatchFixture, PublishesNothingMoreWithoutAChange) {
  watch().publishIfChanged(games());

  watch().publishIfChanged(games());

  EXPECT_EQ(listsPublished(), FIRST_LIST);
}

/**
 * Given a watch that published its list
 * When a player arrives
 * Then it publishes the list again
 */
TEST_F(HostedGamesWatchFixture, PublishesAgainWhenAPlayerArrives) {
  watch().publishIfChanged(games());

  deliver(PlayerArrival{.playerNumber = ARRIVING_PLAYER});

  EXPECT_EQ(listsPublished(), SECOND_LIST);
}

/**
 * Given a watch that published its list
 * When a player leaves
 * Then it publishes the list again
 */
TEST_F(HostedGamesWatchFixture, PublishesAgainWhenAPlayerLeaves) {
  watch().publishIfChanged(games());

  deliver(PlayerDeparture{.playerNumber = ARRIVING_PLAYER,
                          .cause = DepartureCause::Left});

  EXPECT_EQ(listsPublished(), SECOND_LIST);
}

/**
 * Given a watch that published its list
 * When a game changes state
 * Then it publishes the list again
 */
TEST_F(HostedGamesWatchFixture, PublishesAgainWhenAGameChangesState) {
  watch().publishIfChanged(games());

  deliver(GameStateChanged{.previous = GameState::WaitingForPlayers,
                           .current = GameState::Starting});

  EXPECT_EQ(listsPublished(), SECOND_LIST);
}

/**
 * Given a watch that forgot the bus
 * When a player arrives
 * Then the list stays as it was
 */
TEST_F(HostedGamesWatchFixture, ForgetsTheBusItWatched) {
  watch().publishIfChanged(games());
  watch().forget();

  deliver(PlayerArrival{.playerNumber = ARRIVING_PLAYER});

  EXPECT_EQ(listsPublished(), FIRST_LIST);
}
