#pragma once

#include <gtest/gtest.h>
#include "LoopStages.hpp"
#include "PlayerConnection.hpp"
#include "ServiceRegistry.hpp"

/** @brief A connection of a player, never opened, and the registry its parts
 * are provided to. */
class PlayerConnectionFixture : public ::testing::Test {
 protected:
  PlayerConnectionFixture();

  /** @brief Provides the parts of the connection, as the connection module
   * does when it is installed. */
  void provideTheParts();

  [[nodiscard]] const rtype::engine::ServiceRegistry& services() const;

 private:
  rtype::engine::ServiceRegistry services_;
  rtype::engine::LoopStages stages_;
  rtype::client::PlayerConnection connection_;
};
