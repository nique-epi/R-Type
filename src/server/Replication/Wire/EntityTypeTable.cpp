#include "EntityTypeTable.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <optional>
#include <vector>
#include "BehaviourConstants.hpp"
#include "BehaviourView.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntitySideMapping.hpp"
#include "EntityType.hpp"
#include "EntityTypeConstants.hpp"
#include "EntityTypeEntry.hpp"
#include "GunSheet.hpp"
#include "Intent.hpp"
#include "MotionKind.hpp"
#include "Position.hpp"
#include "ShipGun.hpp"
#include "SteadyMotion.hpp"
#include "SteadyTravel.hpp"
#include "StraightBehaviourConstants.hpp"

namespace rtype::server {

namespace {

/** @returns The first sheet of @p types of side Player that has a gun;
 * nullptr when none has. */
const game::EntitySheet* armedShipOf(const game::EntitySheets& types) {
  const auto armed =
      std::ranges::find_if(types.all(), [](const game::EntitySheet& sheet) {
        return sheet.side == game::EntityType::Player && sheet.gun.has_value();
      });
  return armed == types.all().end() ? nullptr : &*armed;
}

/** @returns The velocity of a missile of @p sheet, in logical units per
 * second, when it flies in a straight line at a steady speed; nothing for
 * any other path. */
std::optional<game::Position> straightVelocityOf(
    const game::EntitySheet& sheet) {
  if (sheet.side != game::EntityType::PlayerMissile ||
      sheet.motion.kind != game::MotionKind::Steady ||
      !sheet.pilot.has_value() ||
      sheet.pilot->behaviour.get().name() != game::STRAIGHT_BEHAVIOUR_NAME ||
      sheet.pilot->settings.size() <= game::HEADING_SETTING) {
    return std::nullopt;
  }
  const game::Intent steered = sheet.pilot->behaviour.get().steer(
      game::BehaviourView{.topSpeed = sheet.motion.topSpeed,
                          .settings = sheet.pilot->settings});
  return game::steadyTravel(
      game::SteadyMotion{.topSpeed = sheet.motion.topSpeed}, steered.direction,
      std::chrono::seconds{1});
}

}  // namespace

std::vector<network::EntityTypeEntry> entityTypeTableOf(
    const game::EntitySheets& types) {
  std::vector<network::EntityTypeEntry> table;
  for (const game::EntitySheet& sheet : types.all()) {
    table.push_back(network::EntityTypeEntry{
        .side = toEntitySide(sheet.side),
        .name = sheet.name,
        .hitboxWidth = static_cast<std::uint16_t>(sheet.hitbox.width),
        .hitboxHeight = static_cast<std::uint16_t>(sheet.hitbox.height)});
  }
  return table;
}

network::ShipGun shipGunOf(const game::EntitySheets& types) {
  const game::EntitySheet* const ship = armedShipOf(types);
  if (ship == nullptr || !ship->gun.has_value()) {
    return network::ShipGun{};
  }
  const game::GunSheet& shipGun = *ship->gun;
  network::ShipGun gun{
      .shipType = ship->number,
      .projectileType = identifiers::NO_ENTITY_TYPE,
      .intervalTicks = static_cast<std::uint16_t>(shipGun.intervalTicks),
      .firstShotTicks = static_cast<std::uint16_t>(shipGun.firstShotTicks),
      .velocityX = 0.0F,
      .velocityY = 0.0F};
  const game::EntitySheet* const missile =
      types.numbered(shipGun.projectileNumber);
  if (missile == nullptr) {
    return gun;
  }
  if (const std::optional<game::Position> velocity =
          straightVelocityOf(*missile)) {
    gun.projectileType = missile->number;
    gun.velocityX = velocity->x;
    gun.velocityY = velocity->y;
  }
  return gun;
}

}  // namespace rtype::server
