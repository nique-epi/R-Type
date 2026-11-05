#include <gtest/gtest.h>
#include <memory>
#include "ChildrenStopper.hpp"
#include "GameRegistry.hpp"
#include "GameStatus.hpp"
#include "ModuleConstants.hpp"
#include "ProcessExit.hpp"
#include "RecordingChildState.hpp"
#include "RecordingDatagramSender.hpp"
#include "SupervisedGamesFixtures.hpp"
#include "SupervisedGamesTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::server::ChildrenStopper;
using rtype::server::GameRegistry;
using rtype::server::ProcessExit;

/**
 * Given two games that end as soon as the stopper first lets time pass
 * When it stops them
 * Then nothing was terminated, and both are reported as ended without a crash
 */
TEST(ChildrenStopper, DoesNotKillGamesThatEndWhenAsked) {
  GameRegistry registry;
  auto first = std::make_shared<RecordingChildState>();
  auto second = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, first));
  registry.open(openingOf(1, second));
  registry.record(*registry.findByToken(tokenOfGame(0)), SEQUENCE_FIVE,
                  rtype::network::GameStatus{});
  registry.record(*registry.findByToken(tokenOfGame(1)), SEQUENCE_FIVE,
                  rtype::network::GameStatus{});
  RecordingDatagramSender sender;
  ChildrenStopper stopper{sender, [&](rtype::engine::Duration) {
                            first->exit = ProcessExit{.code = 0};
                            second->exit = ProcessExit{.code = 0};
                          }};

  const auto exited = stopper.stopAll(registry);

  EXPECT_FALSE(first->terminated);
  EXPECT_FALSE(second->terminated);
  ASSERT_EQ(exited.size(), 2U);
  EXPECT_FALSE(exited.front().crashed);
  EXPECT_TRUE(registry.all().empty());
}

/**
 * Given a game that never ends
 * When it is stopped
 * Then it is told to stop FAREWELL_COPIES times, the stopper waited the grace,
 * and then terminated it
 */
TEST(ChildrenStopper, KillsAGameThatDoesNotEndWithinTheGrace) {
  GameRegistry registry;
  auto state = std::make_shared<RecordingChildState>();
  registry.open(openingOf(0, state));
  RecordingDatagramSender sender;
  rtype::engine::Duration paused = rtype::engine::Duration::zero();
  ChildrenStopper stopper{
      sender, [&paused](rtype::engine::Duration pause) { paused += pause; }};

  const auto exited = stopper.stopAll(registry);

  EXPECT_EQ(sender.sent().size(), rtype::server::FAREWELL_COPIES);
  EXPECT_GE(paused, THREE_SECONDS_TO_STOP);
  EXPECT_TRUE(state->terminated);
  ASSERT_EQ(exited.size(), 1U);
  EXPECT_TRUE(exited.front().crashed);
}

/**
 * Given a registry with no game
 * When it is stopped
 * Then nothing is sent and nothing is reported
 */
TEST(ChildrenStopper, HasNothingToDoWithoutAGame) {
  GameRegistry registry;
  RecordingDatagramSender sender;
  ChildrenStopper stopper{sender, [](rtype::engine::Duration) {}};

  const auto exited = stopper.stopAll(registry);

  EXPECT_TRUE(sender.sent().empty());
  EXPECT_TRUE(exited.empty());
}
