#pragma once

#include <optional>
#include <vector>
#include "CrystalDropped.hpp"
#include "DropSystemParts.hpp"
#include "EntitySpawned.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::server {

/**
 * @brief Makes the crystal an enemy drops appear where it died, and tells
 * every connected player.
 *
 * It keeps each game::CrystalDropped the event bus delivers, then at its next
 * onTick() creates each crystal at the position of its enemy (see
 * game::World::spawn) and sends every connected player its EntitySpawned, as
 * a wave does for a Bydo. The crystal is created out of the delivery, never
 * inside the query that found the enemy. When every network id was given, no
 * crystal appears. Subscribes on construction, unsubscribes on destruction.
 * Not thread-safe: use it from the simulation thread only.
 */
class DropSystem {
 public:
  explicit DropSystem(DropSystemParts parts);
  ~DropSystem();

  DropSystem(const DropSystem&) = delete;
  DropSystem& operator=(const DropSystem&) = delete;
  DropSystem(DropSystem&&) = delete;
  DropSystem& operator=(DropSystem&&) = delete;

  /** @brief Makes every crystal dropped since the last call appear. */
  void onTick();

 private:
  /** @brief Makes the crystal of @p dropped appear and tells every connected
   * player; nothing when spawnCrystal() made none. */
  void drop(const game::CrystalDropped& dropped) const;

  /** @returns The EntitySpawned of the crystal of @p dropped, created where
   * its enemy died; nothing when its sheet is unknown or every network id was
   * given. */
  [[nodiscard]] std::optional<network::EntitySpawned> spawnCrystal(
      const game::CrystalDropped& dropped) const;

  DropSystemParts parts_;
  std::vector<game::CrystalDropped> due_;
  engine::SubscriptionHandle subscription_;
};

}  // namespace rtype::server
