#pragma once

#include <optional>
#include <vector>
#include "CrystalTouched.hpp"
#include "Gun.hpp"
#include "ItemClaim.hpp"
#include "SubscriptionHandle.hpp"
#include "WeaponKeeperParts.hpp"

namespace rtype::server {

/**
 * @brief Gives each crystal a ship touched to one pilot, whose ship takes the
 * weapon of the crystal.
 *
 * Every CrystalTouched of a tick is recorded; onTick(), called at the start of
 * the next tick before the world moves, gives each crystal to the pilot
 * winningClaims() names: the Gun of its ship becomes the one gunAfterPickup()
 * gives, and the crystal is removed (EntityDestruction, Removed). A crystal of
 * a weapon the ship does not hold stays in the world. A ship that is destroyed
 * loses its weapon with it: the next one starts with the gun of its sheet.
 * Subscribes on construction, unsubscribes on destruction. Not thread-safe:
 * use it from the simulation thread only.
 */
class WeaponKeeper {
 public:
  explicit WeaponKeeper(WeaponKeeperParts parts);
  ~WeaponKeeper();

  WeaponKeeper(const WeaponKeeper&) = delete;
  WeaponKeeper& operator=(const WeaponKeeper&) = delete;
  WeaponKeeper(WeaponKeeper&&) = delete;
  WeaponKeeper& operator=(WeaponKeeper&&) = delete;

  /** @brief Gives every crystal touched since the previous call. */
  void onTick();

 private:
  void onTouched(const game::CrystalTouched& touched);

  /** @returns The gun the ship of @p winner fires once it takes the crystal
   * of @p winner; nothing when the crystal, the ship or the weapon is
   * unknown. */
  [[nodiscard]] std::optional<game::Gun> armedGunOf(
      const ItemClaim& winner) const;

  /** @brief Arms the ship of @p winner with @p gun and removes its crystal. */
  void give(const ItemClaim& winner, const game::Gun& gun) const;

  WeaponKeeperParts parts_;
  std::vector<ItemClaim> claims_;
  engine::SubscriptionHandle subscription_;
};

}  // namespace rtype::server
