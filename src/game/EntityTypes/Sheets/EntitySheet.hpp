#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "ActionKey.hpp"
#include "BossSheet.hpp"
#include "BurstSheet.hpp"
#include "CollisionBox.hpp"
#include "CollisionSheet.hpp"
#include "EntityType.hpp"
#include "GunSheet.hpp"
#include "HurtZone.hpp"
#include "MaskSheet.hpp"
#include "MotionSheet.hpp"
#include "PilotSheet.hpp"
#include "SheetVisualNames.hpp"
#include "VolleySheet.hpp"

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
  /** @brief What it is in the match: what a contact with it means. */
  EntityType side{};
  /** @brief The collision layer it lies in and the layers it hits: which
   * contacts count. */
  CollisionSheet collision;
  /** @brief The box it collides with, in logical units; for a sheet that
   * lists several boxes, the rectangle that encloses them. */
  CollisionBox hitbox{};
  /** @brief The boxes it collides with when its sheet lists them, which
   * become its HitboxParts; empty for a body of one box. */
  std::vector<CollisionBox> hitboxParts;
  /** @brief The mask an entity of the type is touched on, laid like its
   * sprite; nothing for a sheet without `[gameplay.mask]`, which collides with
   * its boxes alone. */
  std::optional<MaskSheet> mask;
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
  /** @brief The volleys it fires as it enters some of its actions, in the
   * order of the file; empty for a body that fires none. */
  std::vector<VolleySheet> volleys;
  /** @brief What makes it a boss; nothing for any other body. */
  std::optional<BossSheet> boss{std::nullopt};
  /** @brief The item of the locker it gives, as its number in the catalog of
   * ship looks (the position of its name plus one); nothing for a body of
   * another side than look_item. */
  std::optional<std::uint8_t> lookItem{std::nullopt};
  /** @brief The effects and the looks its `[appearance]` names, which a
   * script of a body of this type may ask the client to draw. */
  SheetVisualNames visualNames;
};

}  // namespace rtype::game
