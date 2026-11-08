#include "ShipGunReading.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <optional>
#include <span>
#include "BehaviourConstants.hpp"
#include "BehaviourView.hpp"
#include "EngineException.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntityType.hpp"
#include "EntityTypeConstants.hpp"
#include "GameException.hpp"
#include "GunSheet.hpp"
#include "Intent.hpp"
#include "MotionKind.hpp"
#include "Position.hpp"
#include "SheetText.hpp"
#include "ShipGun.hpp"
#include "SteadyMotion.hpp"
#include "SteadyTravel.hpp"
#include "StraightBehaviourConstants.hpp"

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

}  // namespace

ShipGun shipGunOf(const EntitySheets& types) {
  const EntitySheet* const ship = armedShipOf(types);
  if (ship == nullptr || !ship->gun.has_value()) {
    return ShipGun{};
  }
  const GunSheet& shipGun = *ship->gun;
  ShipGun gun{
      .shipType = ship->number,
      .projectileType = identifiers::NO_ENTITY_TYPE,
      .intervalTicks = static_cast<std::uint16_t>(shipGun.intervalTicks),
      .firstShotTicks = static_cast<std::uint16_t>(shipGun.firstShotTicks),
      .velocityX = 0.0F,
      .velocityY = 0.0F};
  const EntitySheet* const missile = types.numbered(shipGun.projectileNumber);
  if (missile == nullptr) {
    return gun;
  }
  if (const std::optional<Position> velocity = straightVelocityOf(*missile)) {
    gun.projectileType = missile->number;
    gun.velocityX = velocity->x;
    gun.velocityY = velocity->y;
  }
  return gun;
}

ShipGun shipGunOrNoneOf(std::span<const SheetText> texts) {
  try {
    return shipGunOf(EntitySheets::fromTexts(texts));
  } catch (const engine::EngineException&) {
    return ShipGun{};
  } catch (const GameException&) {
    return ShipGun{};
  }
}

}  // namespace rtype::game
