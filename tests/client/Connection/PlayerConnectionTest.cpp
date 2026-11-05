#include <gtest/gtest.h>
#include "IServerTicks.hpp"
#include "PlayerConnectionFixture.hpp"

using rtype::client::IServerTicks;

/**
 * Given a connection of a player
 * When it provides its parts to the modules installed after it
 * Then the clock of the server is among them, for the screens of a game
 */
TEST_F(PlayerConnectionFixture, ProvidesTheClockOfTheServer) {
  provideTheParts();

  EXPECT_NE(services().find<IServerTicks>(), nullptr);
}
