#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Position.hpp"
#include "WeaponSystemFixture.hpp"

namespace rtype::server::testing {

/**
 * @brief The weapon system fixture with the ships of players, that a test
 * makes want to shoot or not.
 */
class ShipWeaponFixture : public WeaponSystemFixture {
 protected:
  /** @brief Connects the player at @p index and gives it a ship where the ship
   * of its number starts. */
  ConnectedPlayer& connect(std::size_t index);

  /** @brief Runs @p count ticks in which only @p shooter holds shoot. */
  void runTicksShooting(ConnectedPlayer& shooter, std::size_t count);

  /** @brief Runs @p count ticks in which no player holds shoot. */
  void runTicksWithoutShooting(std::size_t count);

  /** @returns How many missiles of players the world holds. */
  [[nodiscard]] std::size_t missilesInTheWorld();

  /** @returns The player numbers the missiles of the world belong to. */
  [[nodiscard]] std::vector<std::uint8_t> ownersOfTheMissiles();

  /** @returns Where the ship of @p player is; the origin, after a failure, when
   * it has none. */
  [[nodiscard]] game::Position shipOf(const ConnectedPlayer& player);

 private:
  /** @brief Makes the ship of @p player want to shoot, or not. */
  void setShooting(const ConnectedPlayer& player, bool shooting);

  /** @brief Makes the ship of every connected player stop wanting to shoot. */
  void releaseEveryone();
};

}  // namespace rtype::server::testing
