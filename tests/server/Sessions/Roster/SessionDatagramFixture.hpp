#pragma once

#include <gtest/gtest.h>
#include "ConnectedPlayer.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server::testing {

/** @brief A roster whose first player is connected under SESSION_TOKEN. */
class SessionDatagramFixture : public ::testing::Test {
 protected:
  SessionDatagramFixture();

  [[nodiscard]] PlayerRoster& roster();
  [[nodiscard]] ConnectedPlayer& firstPlayer();

 private:
  PlayerRoster roster_;
  ConnectedPlayer* firstPlayer_{nullptr};
};

}  // namespace rtype::server::testing
