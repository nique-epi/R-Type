#include "World.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include "BodyPlacement.hpp"
#include "Boss.hpp"
#include "CollisionBox.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "Entity.hpp"
#include "EntitySheet.hpp"
#include "EntityType.hpp"
#include "EntityTypeNumber.hpp"
#include "Fragile.hpp"
#include "Gun.hpp"
#include "Health.hpp"
#include "HurtZone.hpp"
#include "Intent.hpp"
#include "Invulnerable.hpp"
#include "Lives.hpp"
#include "LivesConstants.hpp"
#include "MissileOwner.hpp"
#include "MotionKind.hpp"
#include "MotionSheet.hpp"
#include "NetworkEntityTable.hpp"
#include "NetworkIdentity.hpp"
#include "Pilot.hpp"
#include "PilotSheet.hpp"
#include "Position.hpp"
#include "ScoreValue.hpp"
#include "ShardBurst.hpp"
#include "ShipMotion.hpp"
#include "SteadyMotion.hpp"
#include "SurvivesContact.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

namespace {

/** @brief A body being made: the entity, the sheet of its type, where it
 * appears and the point its Pilot steers from. */
struct BodyStart {
  engine::Entity body;
  std::reference_wrapper<const EntitySheet> sheet;
  Position position;
  Position origin;
};

/** @brief Gives @p start what every entity of a sheet has: where it is, its
 * side, its box, the number of its sheet, and an Intent that wants nothing. */
void addPlace(engine::ComponentRegistry& components, const BodyStart& start) {
  const EntitySheet& sheet = start.sheet.get();
  components.add(start.body, start.position);
  components.add(start.body, sheet.side);
  components.add(start.body, sheet.hitbox);
  components.add(start.body, EntityTypeNumber{.number = sheet.number});
  components.add(start.body, Intent{});
}

/** @brief Gives @p start the motion its sheet names. */
void addMotion(engine::ComponentRegistry& components, const BodyStart& start) {
  const MotionSheet& motion = start.sheet.get().motion;
  if (motion.kind == MotionKind::Ship) {
    components.add(start.body, ShipMotion{});
    return;
  }
  components.add(start.body, SteadyMotion{.topSpeed = motion.topSpeed});
}

/** @brief Gives @p start the Pilot of its behaviour, when one pilots it. */
void addPilot(engine::ComponentRegistry& components, const BodyStart& start) {
  const std::optional<PilotSheet>& pilot = start.sheet.get().pilot;
  if (!pilot.has_value()) {
    return;
  }
  components.add(start.body, Pilot{.behaviour = pilot->behaviour,
                                   .origin = start.origin,
                                   .age = engine::Duration::zero(),
                                   .settings = pilot->settings});
}

/** @brief Gives @p start the Health, the ScoreValue, the Gun and the
 * ShardBurst its sheet gives. */
void addCombat(engine::ComponentRegistry& components, const BodyStart& start) {
  const EntitySheet& sheet = start.sheet.get();
  if (sheet.health.has_value()) {
    components.add(start.body,
                   Health{.points = *sheet.health, .full = *sheet.health});
  }
  if (sheet.points.has_value()) {
    components.add(start.body, ScoreValue{.points = *sheet.points});
  }
  if (sheet.gun.has_value()) {
    components.add(start.body,
                   Gun{.intervalTicks = sheet.gun->intervalTicks,
                       .ticksUntilReady = sheet.gun->firstShotTicks,
                       .projectileNumber = sheet.gun->projectileNumber});
  }
  if (sheet.burst.has_value()) {
    components.add(start.body,
                   ShardBurst{.ticksLeft = sheet.burst->afterTicks,
                              .shardNumber = sheet.burst->shardNumber,
                              .count = sheet.burst->count});
  }
}

/** @brief Gives @p start the parts that decide what its contact with a ship
 * does: the CurrentAction of an enemy, then the HurtZone and the
 * SurvivesContact and the Fragile its sheet declares. */
void addContactParts(engine::ComponentRegistry& components,
                     const BodyStart& start) {
  const EntitySheet& sheet = start.sheet.get();
  if (sheet.side == EntityType::Enemy) {
    components.add(start.body, CurrentAction{});
  }
  if (sheet.hurtZone.has_value()) {
    components.add(start.body, *sheet.hurtZone);
  }
  if (sheet.survivesContact) {
    components.add(start.body, SurvivesContact{});
  }
  if (sheet.fragileIn.has_value()) {
    components.add(start.body, Fragile{.in = *sheet.fragileIn});
  }
}

/** @brief Gives @p start its Boss when its sheet makes it one: its rage
 * threshold is its full health times the percentage of the sheet. */
void addBoss(engine::ComponentRegistry& components, const BodyStart& start) {
  const EntitySheet& sheet = start.sheet.get();
  if (!sheet.boss.has_value() || !sheet.health.has_value()) {
    return;
  }
  components.add(start.body,
                 Boss{.rageBelowPoints = rageBelowPointsOf(
                          *sheet.health, sheet.boss->rageBelowPercent),
                      .rageAttack = sheet.boss->rageAttack,
                      .arrivalAttack = sheet.boss->arrivalAttack});
}

/** @brief Gives the ship of a player its lives and no protection yet. */
void addShipParts(engine::ComponentRegistry& components, engine::Entity ship) {
  components.add(ship, Lives{.count = STARTING_LIVES});
  components.add(ship, Invulnerable{});
}

}  // namespace

std::uint32_t World::spawn(const EntitySheet& sheet, Position position,
                           std::optional<std::uint8_t> owner) {
  return spawnFrom(
      sheet, BodyPlacement{.position = position, .origin = position}, owner);
}

std::uint32_t World::spawnFrom(const EntitySheet& sheet,
                               BodyPlacement placement,
                               std::optional<std::uint8_t> owner) {
  const std::uint32_t networkId = networkIds_.allocate();
  const BodyStart start{.body = registry_.create(),
                        .sheet = sheet,
                        .position = placement.position,
                        .origin = placement.origin};
  addPlace(components_, start);
  addMotion(components_, start);
  addPilot(components_, start);
  addCombat(components_, start);
  addContactParts(components_, start);
  addBoss(components_, start);
  components_.add(start.body, NetworkIdentity{.networkId = networkId});
  if (owner.has_value()) {
    components_.add(start.body, MissileOwner{.playerNumber = *owner});
  }
  if (!sheet.pilot.has_value()) {
    addShipParts(components_, start.body);
    shipByNetworkId_.bind(networkId, start.body);
  }
  return networkId;
}

bool World::removePlayer(std::uint32_t shipNetworkId) {
  const std::optional<engine::Entity> ship =
      shipByNetworkId_.find(shipNetworkId);
  if (!ship.has_value()) {
    return false;
  }
  shipByNetworkId_.unbind(shipNetworkId);
  components_.destroy(*ship);
  return true;
}

std::size_t World::entityCount() const { return registry_.size(); }

engine::ComponentRegistry& World::components() { return components_; }

Position* World::shipPositionOf(std::uint32_t shipId) {
  return shipComponentOf<Position>(shipId);
}

ShipMotion* World::shipMotionOf(std::uint32_t shipId) {
  return shipComponentOf<ShipMotion>(shipId);
}

}  // namespace rtype::game
