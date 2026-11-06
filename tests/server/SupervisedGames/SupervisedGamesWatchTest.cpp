#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include "GameRegistry.hpp"
#include "GameStatus.hpp"
#include "HostedGame.hpp"
#include "RecordingChildState.hpp"
#include "SupervisedGamesFixtures.hpp"
#include "SupervisedGamesTestConstants.hpp"
#include "SupervisedGamesWatch.hpp"
#include "SupervisedGamesWatchTestConstants.hpp"

using rtype::network::GameStatus;
using rtype::network::GameStatusState;
using rtype::network::HostedGame;
using rtype::server::GameRegistry;
using rtype::server::SupervisedGamesWatch;

/**
 * Given a watch on a registry with no game
 * When it publishes for the first time
 * Then the list and the measures are each published once, the list empty
 */
TEST(SupervisedGamesWatch, PublishesTheListAndTheMeasuresTheFirstTime) {
  const GameRegistry registry;
  SupervisedGamesWatch watch{registry};

  watch.publishIfChanged();

  EXPECT_EQ(watch.hostedGames().revision(), PUBLISHED_ONCE);
  EXPECT_EQ(watch.measures().revision(), PUBLISHED_ONCE);
  const auto list = watch.hostedGames().value();
  ASSERT_TRUE(list.has_value());
  EXPECT_TRUE(list.value_or(std::vector<HostedGame>{}).empty());
}

/**
 * Given a watch that published once
 * When it publishes again and nothing changed
 * Then neither the list nor the measures are published again
 */
TEST(SupervisedGamesWatch, PublishesNothingMoreWithoutAChange) {
  const GameRegistry registry;
  SupervisedGamesWatch watch{registry};
  watch.publishIfChanged();

  watch.publishIfChanged();

  EXPECT_EQ(watch.hostedGames().revision(), PUBLISHED_ONCE);
  EXPECT_EQ(watch.measures().revision(), PUBLISHED_ONCE);
}

/**
 * Given a watch that published a game that starts
 * When the game reports a player and the watch publishes
 * Then the list is published again, and the measures, unchanged, are not
 */
TEST(SupervisedGamesWatch, PublishesTheListAgainWhenAGameChanges) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  SupervisedGamesWatch watch{registry};
  watch.publishIfChanged();
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  statusWithPlayers(GameStatusState::WaitingForPlayers, 1));

  watch.publishIfChanged();

  EXPECT_EQ(watch.hostedGames().revision(), PUBLISHED_TWICE);
  EXPECT_EQ(watch.measures().revision(), PUBLISHED_ONCE);
}

/**
 * Given a watch that published a game that starts
 * When the game reports a median tick and no player, and the watch publishes
 * Then the measures are published again, and the list, still one waiting
 * game of nobody, is not
 */
TEST(SupervisedGamesWatch, PublishesTheMeasuresAgainWhenTheyChange) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  SupervisedGamesWatch watch{registry};
  watch.publishIfChanged();
  GameStatus status = statusWithPlayers(GameStatusState::WaitingForPlayers, 0);
  status.measures.tickMedianMicroseconds = REPORTED_MEDIAN_MICROSECONDS;
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE, status);

  watch.publishIfChanged();

  EXPECT_EQ(watch.measures().revision(), PUBLISHED_TWICE);
  EXPECT_EQ(watch.hostedGames().revision(), PUBLISHED_ONCE);
}

/**
 * Given a watch on a registry of one game that starts with nobody
 * When the occupancy is asked
 * Then it counts that game's four empty seats and those of the seven free
 * places
 */
TEST(SupervisedGamesWatch, CountsTheOccupancyOfTheRegistry) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  const SupervisedGamesWatch watch{registry};

  EXPECT_EQ(watch.occupancy().freeSeatCount, SEATS_BESIDE_A_STARTING_GAME);
}
