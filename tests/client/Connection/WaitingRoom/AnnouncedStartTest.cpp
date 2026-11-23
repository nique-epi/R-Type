#include <gtest/gtest.h>
#include <optional>
#include "AnnouncedStartFixture.hpp"
#include "AnnouncedStartTestConstants.hpp"
#include "GameStarted.hpp"

using rtype::client::testing::ANNOUNCED_BIOME;
using rtype::client::testing::AnnouncedStartFixture;
using rtype::client::testing::startInTheCaves;
using rtype::network::GameStarted;

/**
 * Given a session
 * When the server announces a game whose level opens in the caves
 * Then the announced start is that GameStarted
 */
TEST_F(AnnouncedStartFixture, HoldsTheGameStartedOfTheSession) {
  open();

  deliver(startInTheCaves());

  EXPECT_EQ(start().started().value_or(GameStarted{}).biome, ANNOUNCED_BIOME);
}

/**
 * Given a session whose game started in the caves
 * When the game ends
 * Then no start is announced any more
 */
TEST_F(AnnouncedStartFixture, ForgetsTheStartOnceTheGameEnded) {
  open();
  deliver(startInTheCaves());

  room().endGame();

  EXPECT_EQ(start().started(), std::nullopt);
}

/**
 * Given a session whose game started in the caves
 * When the session closes
 * Then no start is announced any more
 */
TEST_F(AnnouncedStartFixture, ForgetsTheStartWhenTheSessionCloses) {
  open();
  deliver(startInTheCaves());

  room().sessionClosed();

  EXPECT_EQ(start().started(), std::nullopt);
}

/**
 * Given no session
 * When a GameStarted reaches the dispatcher
 * Then no start is announced, as the room starts no game
 */
TEST_F(AnnouncedStartFixture, HoldsNothingOutsideASession) {
  deliver(startInTheCaves());

  EXPECT_EQ(start().started(), std::nullopt);
}
