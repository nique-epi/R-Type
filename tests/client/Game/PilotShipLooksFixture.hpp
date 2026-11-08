#pragma once

#include <gtest/gtest.h>
#include <optional>
#include <span>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityRegistry.hpp"
#include "PlayerLooks.hpp"
#include "ShipLook.hpp"

/** @brief The entities of a game, the ships of pilots among them. */
class PilotShipLooksFixture : public ::testing::Test {
 protected:
  /** @returns A ship of @p pilotColor, with the sprite and the pilot a ship
   * of the game has. */
  rtype::engine::Entity spawnShip(int pilotColor);
  /** @brief Dresses the ships with @p looks. */
  void dress(std::span<const rtype::network::PlayerLook> looks);
  /** @returns What @p ship wears, or nothing when it has no WornLook. */
  [[nodiscard]] std::optional<rtype::identifiers::ShipLook> wornBy(
      rtype::engine::Entity ship);

 private:
  rtype::engine::EntityRegistry entities_;
  rtype::engine::ComponentRegistry components_{entities_};
};
