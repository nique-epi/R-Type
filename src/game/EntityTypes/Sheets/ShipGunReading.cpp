#include "ShipGunReading.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>
#include "BehaviourConstants.hpp"
#include "BehaviourView.hpp"
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntityType.hpp"
#include "EntityTypeConstants.hpp"
#include "GameException.hpp"
#include "GunSheet.hpp"
#include "HeldWeapon.hpp"
#include "Intent.hpp"
#include "MotionKind.hpp"
#include "Position.hpp"
#include "SheetText.hpp"
#include "ShipArsenal.hpp"
#include "ShipGun.hpp"
#include "SteadyMotion.hpp"
#include "SteadyTravel.hpp"
#include "StraightBehaviourConstants.hpp"
#include "WeaponSheet.hpp"

namespace rtype::game {

namespace {

/** @returns The first sheet of @p types of side Player that has a gun;
 * nullptr when none has. */
const EntitySheet* armedShipOf(const EntitySheets& types) {
  const auto armed =
      std::ranges::find_if(types.all(), [](const EntitySheet& sheet) {
        return sheet.side == EntityType::Player && sheet.gun.has_value();
      });
  return armed == types.all().end() ? nullptr : &*armed;
}

/** @returns The velocity of a missile of @p sheet, in logical units per
 * second, when it flies in a straight line at a steady speed; nothing for
 * any other path. */
std::optional<Position> straightVelocityOf(const EntitySheet& sheet) {
  if (sheet.side != EntityType::PlayerMissile ||
      sheet.motion.kind != MotionKind::Steady || !sheet.pilot.has_value() ||
      sheet.pilot->behaviour.get().name() != STRAIGHT_BEHAVIOUR_NAME ||
      sheet.pilot->settings.size() <= HEADING_SETTING) {
    return std::nullopt;
  }
  const Intent steered = sheet.pilot->behaviour.get().steer(BehaviourView{
      .topSpeed = sheet.motion.topSpeed, .settings = sheet.pilot->settings});
  return steadyTravel(SteadyMotion{.topSpeed = sheet.motion.topSpeed},
                      steered.direction, std::chrono::seconds{1});
}

/** @returns The speed of a missile of @p sheet, in logical units per second,
 * when it scatters away from the muzzle at a steady speed; nothing for any
 * other path. */
std::optional<float> scatterSpeedOf(const EntitySheet& sheet) {
  if (sheet.side != EntityType::PlayerMissile ||
      sheet.motion.kind != MotionKind::Steady || !sheet.pilot.has_value() ||
      sheet.pilot->behaviour.get().name() != SCATTER_BEHAVIOUR_NAME) {
    return std::nullopt;
  }
  return sheet.motion.topSpeed;
}

/** @returns The gun of @p ship that @p sheet describes, its missile read
 * from @p types: what it fires and how that missile flies, or no projectile
 * when it flies no path the client can predict. */
ShipGun gunOf(const EntitySheet& ship, const GunSheet& sheet,
              const EntitySheets& types) {
  ShipGun gun{
      .shipType = ship.number,
      .projectileType = identifiers::NO_ENTITY_TYPE,
      .intervalTicks = static_cast<std::uint16_t>(sheet.intervalTicks),
      .firstShotTicks = static_cast<std::uint16_t>(sheet.firstShotTicks),
      .velocityX = 0.0F,
      .velocityY = 0.0F,
      .pattern = sheet.pattern,
      .scatterSpeed = 0.0F};
  const EntitySheet* const missile = types.numbered(sheet.projectileNumber);
  if (missile == nullptr) {
    return gun;
  }
  if (const std::optional<Position> velocity = straightVelocityOf(*missile)) {
    gun.projectileType = missile->number;
    gun.velocityX = velocity->x;
    gun.velocityY = velocity->y;
  } else if (const std::optional<float> speed = scatterSpeedOf(*missile)) {
    gun.projectileType = missile->number;
    gun.scatterSpeed = *speed;
  }
  return gun;
}

}  // namespace

ShipArsenal shipArsenalOf(const EntitySheets& types) {
  const EntitySheet* const ship = armedShipOf(types);
  if (ship == nullptr || !ship->gun.has_value()) {
    return ShipArsenal{};
  }
  ShipArsenal arsenal{.gun = gunOf(*ship, *ship->gun, types), .weapons = {}};
  for (const WeaponSheet& weapon : ship->weapons) {
    std::vector<ShipGun>& levels = arsenal.weapons.emplace_back();
    for (const GunSheet& level : weapon.levels) {
      levels.push_back(gunOf(*ship, level, types));
    }
  }
  return arsenal;
}

ShipArsenal shipArsenalOrNoneOf(std::span<const SheetText> texts) {
  try {
    return shipArsenalOf(EntitySheets::fromTexts(texts));
  } catch (const engine::EngineException&) {
    return ShipArsenal{};
  } catch (const GameException&) {
    return ShipArsenal{};
  }
}

const ShipGun& heldGunOf(const ShipArsenal& arsenal, HeldWeapon held) {
  if (held.weapon == 0 || held.weapon > arsenal.weapons.size()) {
    return arsenal.gun;
  }
  const std::vector<ShipGun>& levels = arsenal.weapons.at(held.weapon - 1U);
  if (held.level == 0 || held.level > levels.size()) {
    return arsenal.gun;
  }
  return levels.at(held.level - 1U);
}

}  // namespace rtype::game
