#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include "ActionKey.hpp"
#include "BurstSheet.hpp"
#include "CollisionBox.hpp"
#include "EntityType.hpp"
#include "GunSheet.hpp"
#include "HurtZone.hpp"
#include "MotionSheet.hpp"
#include "PilotSheet.hpp"

namespace rtype::game {

/**
 * @brief The gameplay of one entity type, as the table `[gameplay]` of its
 * sheet describes it: what an entity of that type starts with.
 *
 * gameplayOf() reads it and EntitySheets numbers it. Every value was checked
 * when the sheets were read, so the code that creates an entity from it checks
 * nothing again.
 */
struct EntitySheet {
  /** @brief The name of the type: the name of its file, without `.toml`. */
  std::string name;
  /** @brief Its place in the name order of the sheets, from 1; 0 until
   * EntitySheets numbers it. */
  std::uint8_t number{};
  /** @brief Who it hits and who hits it. */
  EntityType side{};
  /** @brief The box it collides with, in logical units. */
  CollisionBox hitbox{};
  /** @brief How a direction of its Intent moves it. */
  MotionSheet motion{};
  /** @brief The behaviour that steers it; nothing for a body a player steers.
   */
  std::optional<PilotSheet> pilot;
  /** @brief Its health points; nothing for a body without Health. */
  std::optional<int> health;
  /** @brief The points the player who destroys it earns; nothing for a body
   * without ScoreValue. */
  std::optional<std::uint32_t> points;
  /** @brief Its gun; nothing for a body that fires nothing. */
  std::optional<GunSheet> gun;
  /** @brief The boxes that hurt a ship on contact; nothing for a body without
   * HurtZone. */
  std::optional<HurtZone> hurtZone;
  /** @brief Whether a ship that touches it leaves it standing. */
  bool survivesContact{false};
  /** @brief The action in which any hit destroys it; nothing for a body that
   * loses one point a hit all the time. */
  std::optional<ActionKey> fragileIn{std::nullopt};
  /** @brief How it bursts into shards; nothing for a body that does not. */
  std::optional<BurstSheet> burst{std::nullopt};
};

}  // namespace rtype::game
