#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include "ConnectedPlayer.hpp"
#include "PlayerInput.hpp"
#include "PlayerRoster.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A world and a roster the controller tests fill themselves.
 */
class PlayerControllerFixture : public ::testing::Test {
 protected:
  /** @brief Connects the player at @p index, under @p sessionToken, without a
   * ship. */
  ConnectedPlayer& addPlayer(std::size_t index, std::uint64_t sessionToken);

  /** @brief Connects the player at @p index, under @p sessionToken, and gives
   * it a ship. */
  ConnectedPlayer& addPlayerWithShip(std::size_t index,
                                     std::uint64_t sessionToken);

  /** @returns An input numbered @p number that holds @p actions. */
  [[nodiscard]] static network::PlayerInput inputHolding(std::uint8_t actions,
                                                         std::uint32_t number);

  [[nodiscard]] game::World& world();
  [[nodiscard]] PlayerRoster& roster();

 private:
  game::World world_;
  PlayerRoster roster_;
};

}  // namespace rtype::server::testing
