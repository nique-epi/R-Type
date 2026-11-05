#include "EntitySheetReader.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
#include <toml++/toml.hpp>
#include <vector>
#include "BehaviourRegistry.hpp"
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "CollisionBox.hpp"
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetConstants.hpp"
#include "EntityType.hpp"
#include "GameException.hpp"
#include "GunSheet.hpp"
#include "IBehaviour.hpp"
#include "MotionKind.hpp"
#include "MotionSheet.hpp"
#include "PilotSheet.hpp"
#include "Position.hpp"
#include "SheetText.hpp"
#include "SheetTextReading.hpp"
#include "ShipControlConstants.hpp"
#include "TimeConstants.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::game {

namespace {

/** @returns The whole simulation ticks nearest to @p duration: 333 ms are 20
 * ticks at 60 per second, 1500 ms are 90, 0 ms are 0. */
std::size_t nearestTicksIn(engine::Duration duration) {
  return static_cast<std::size_t>((duration + HALF_A_TICK) /
                                  engine::SIMULATION_TICK_DURATION);
}

/** @returns The table `[gameplay]` of @p root, as sheetTableOf() reads it,
 * once it holds no unknown key. */
engine::DataFileTable gameplayTableOf(const engine::DataFileTable& root) {
  engine::DataFileTable gameplay = sheetTableOf(root, GAMEPLAY_KEY);
  engine::refuseUnknownKeys(gameplay, GAMEPLAY_KEYS);
  return gameplay;
}

/** @returns The side `[gameplay]` names. */
EntityType sideFrom(const engine::DataFileTable& gameplay) {
  return SIDES.at(engine::choiceAt(gameplay, SIDE_KEY, SIDE_NAMES));
}

/** @returns The box `[gameplay.hitbox]` describes. */
CollisionBox hitboxFrom(const engine::DataFileTable& gameplay) {
  const engine::DataFileTable hitbox = engine::tableAt(gameplay, HITBOX_KEY);
  engine::refuseUnknownKeys(hitbox, HITBOX_KEYS);
  return CollisionBox{.width = static_cast<float>(engine::wholeNumberAt(
                          hitbox, WIDTH_KEY, HITBOX_SIDE_RANGE)),
                      .height = static_cast<float>(engine::wholeNumberAt(
                          hitbox, HEIGHT_KEY, HITBOX_SIDE_RANGE))};
}

/** @returns The motion `[gameplay.motion]` describes: a ship with its kind
 * only, a steady body with its top speed too. */
MotionSheet motionFrom(const engine::DataFileTable& gameplay) {
  const engine::DataFileTable motion = engine::tableAt(gameplay, MOTION_KEY);
  const MotionKind kind =
      MOTION_KINDS.at(engine::choiceAt(motion, KIND_KEY, MOTION_KIND_NAMES));
  if (kind == MotionKind::Ship) {
    engine::refuseUnknownKeys(motion, SHIP_MOTION_KEYS);
    return MotionSheet{.kind = kind, .topSpeed = 0.0F};
  }
  engine::refuseUnknownKeys(motion, STEADY_MOTION_KEYS);
  return MotionSheet{.kind = kind,
                     .topSpeed = static_cast<float>(engine::wholeNumberAt(
                         motion, TOP_SPEED_KEY, TOP_SPEED_RANGE))};
}

/** @returns What a pilot may be: a player, then every behaviour of
 * @p behaviours, for the error that names an unknown one. */
std::vector<std::string_view> pilotNames(const BehaviourRegistry& behaviours) {
  std::vector<std::string_view> names{PLAYER_PILOT_NAME};
  for (const std::string_view name : behaviours.names()) {
    names.push_back(name);
  }
  return names;
}

/** @returns The behaviour the pilot of @p gameplay names, nullptr for a
 * player. @throws engine::UnknownDataFileChoiceException when it names none of
 * pilotNames(). */
const IBehaviour* behaviourOf(const engine::DataFileTable& gameplay,
                              const BehaviourRegistry& behaviours) {
  const std::vector<std::string_view> names = pilotNames(behaviours);
  const std::string_view pilot =
      names.at(engine::choiceAt(gameplay, PILOT_KEY, names));
  return pilot == PLAYER_PILOT_NAME ? nullptr : behaviours.find(pilot);
}

/** @throws InconsistentEntitySheetException when @p gameplay holds a table
 * `[gameplay.behaviour]`. */
void refuseBehaviourTable(const engine::DataFileTable& gameplay) {
  if (engine::optionalTableAt(gameplay, BEHAVIOUR_KEY).has_value()) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, BEHAVIOUR_KEY), BEHAVIOUR_TABLE_RULE);
  }
}

/** @returns The names of @p settings, in their order. */
std::vector<std::string_view> settingNames(
    std::span<const BehaviourSetting> settings) {
  std::vector<std::string_view> names;
  names.reserve(settings.size());
  for (const BehaviourSetting& setting : settings) {
    names.push_back(setting.name);
  }
  return names;
}

/** @returns The values `[gameplay.behaviour]` gives the settings @p behaviour
 * declares, in their order; none, and no table, for a behaviour without
 * settings. */
std::vector<std::int64_t> settingsFrom(const engine::DataFileTable& gameplay,
                                       const IBehaviour& behaviour) {
  const std::span<const BehaviourSetting> declared = behaviour.settings();
  if (declared.empty()) {
    refuseBehaviourTable(gameplay);
    return {};
  }
  const engine::DataFileTable table = engine::tableAt(gameplay, BEHAVIOUR_KEY);
  engine::refuseUnknownKeys(table, settingNames(declared));
  std::vector<std::int64_t> values;
  values.reserve(declared.size());
  for (const BehaviourSetting& setting : declared) {
    values.push_back(engine::wholeNumberAt(table, setting.name, setting.range));
  }
  return values;
}

/** @returns The behaviour that pilots the body of @p gameplay with its
 * settings; nothing, and no behaviour table, for a body a player pilots. */
std::optional<PilotSheet> pilotFrom(const engine::DataFileTable& gameplay,
                                    const BehaviourRegistry& behaviours) {
  const IBehaviour* const behaviour = behaviourOf(gameplay, behaviours);
  if (behaviour == nullptr) {
    refuseBehaviourTable(gameplay);
    return std::nullopt;
  }
  return PilotSheet{.behaviour = *behaviour,
                    .settings = settingsFrom(gameplay, *behaviour)};
}

/** @returns The points of the optional table @p key of @p gameplay, in
 * @p range, as a @p Points, which @p range fits in: the health points of
 * `[gameplay.health]` as an int, those of `[gameplay.score]` as a
 * std::uint32_t; nothing when the sheet has no such table. */
template <typename Points>
std::optional<Points> pointsIn(const engine::DataFileTable& gameplay,
                               std::string_view key,
                               engine::WholeNumberRange range) {
  const std::optional<engine::DataFileTable> table =
      engine::optionalTableAt(gameplay, key);
  if (!table.has_value()) {
    return std::nullopt;
  }
  engine::refuseUnknownKeys(*table, POINTS_KEYS);
  return static_cast<Points>(engine::wholeNumberAt(*table, POINTS_KEY, range));
}

/** @returns The gun of `[gameplay.gun]`, its durations in the nearest whole
 * ticks, at least MINIMUM_GUN_INTERVAL_TICKS between two shots; nothing
 * without it. */
std::optional<GunSheet> gunFrom(const engine::DataFileTable& gameplay) {
  const std::optional<engine::DataFileTable> gun =
      engine::optionalTableAt(gameplay, GUN_KEY);
  if (!gun.has_value()) {
    return std::nullopt;
  }
  engine::refuseUnknownKeys(*gun, GUN_KEYS);
  return GunSheet{
      .intervalTicks = std::max(MINIMUM_GUN_INTERVAL_TICKS,
                                nearestTicksIn(engine::millisecondsAt(
                                    *gun, INTERVAL_KEY, GUN_INTERVAL_RANGE))),
      .firstShotTicks = nearestTicksIn(
          engine::millisecondsAt(*gun, FIRST_SHOT_KEY, FIRST_SHOT_RANGE)),
      .projectile = engine::textAt(*gun, PROJECTILE_KEY),
      .projectilePlace = engine::placeOfKey(*gun, PROJECTILE_KEY)};
}

/** @throws InconsistentEntitySheetException when a player pilots @p sheet and
 * its side is not player, or its side is player and a behaviour pilots it. */
void checkPilotMatchesSide(const EntitySheet& sheet,
                           const engine::DataFileTable& gameplay) {
  const bool pilotedByAPlayer = !sheet.pilot.has_value();
  if (pilotedByAPlayer != (sheet.side == EntityType::Player)) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, PILOT_KEY), PLAYER_PILOT_RULE);
  }
}

/** @throws InconsistentEntitySheetException when a body of side player does
 * not move as a ship or another body does, or a ship has another box than the
 * one shipAfter() keeps on screen. */
void checkShipMotion(const EntitySheet& sheet,
                     const engine::DataFileTable& gameplay) {
  const bool movesAsAShip = sheet.motion.kind == MotionKind::Ship;
  if (movesAsAShip != (sheet.side == EntityType::Player)) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, MOTION_KEY), SHIP_MOTION_RULE);
  }
  if (movesAsAShip && (sheet.hitbox.width != SHIP_WIDTH ||
                       sheet.hitbox.height != SHIP_HEIGHT)) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, HITBOX_KEY), SHIP_HITBOX_RULE);
  }
}

/** @returns Whether @p side is the side of a missile, of a player or of an
 * enemy. */
bool isAMissile(EntityType side) {
  return side == EntityType::PlayerMissile || side == EntityType::EnemyMissile;
}

/** @throws InconsistentEntitySheetException when a missile has a gun. */
void checkMissileHasNoGun(const EntitySheet& sheet,
                          const engine::DataFileTable& gameplay) {
  if (isAMissile(sheet.side) && sheet.gun.has_value()) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, GUN_KEY), MISSILE_GUN_RULE);
  }
}

/** @returns The horizontal share of the Direction @p pilot asks of a body of
 * @p motion on its first tick: below 0 to the left, above 0 to the right, 0
 * for a body that stays. */
float firstHorizontalSteeringOf(const PilotSheet& pilot,
                                const MotionSheet& motion) {
  const BehaviourView firstTick{.position = Position{},
                                .origin = Position{},
                                .age = engine::SIMULATION_TICK_DURATION,
                                .elapsed = engine::SIMULATION_TICK_DURATION,
                                .topSpeed = motion.topSpeed,
                                .settings = pilot.settings};
  return pilot.behaviour.get().steer(firstTick).direction.x;
}

/** @returns Whether a missile of @p side steered by @p horizontalSteering
 * flies towards the edge PlayfieldExitSystem removes it at: the right one for
 * a missile of a player, the left one for a missile of an enemy. */
bool fliesTowardsItsEdge(EntityType side, float horizontalSteering) {
  return side == EntityType::PlayerMissile ? horizontalSteering > 0.0F
                                           : horizontalSteering < 0.0F;
}

/** @throws InconsistentEntitySheetException when the pilot of a missile does
 * not steer it, on its first tick, towards the edge that removes it: such a
 * missile that hits nothing would live for the rest of the game. */
void checkMissileFliesTowardsItsEdge(const EntitySheet& sheet,
                                     const engine::DataFileTable& gameplay) {
  if (!isAMissile(sheet.side) || !sheet.pilot.has_value()) {
    return;
  }
  if (!fliesTowardsItsEdge(
          sheet.side, firstHorizontalSteeringOf(*sheet.pilot, sheet.motion))) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, PILOT_KEY), MISSILE_HEADING_RULE);
  }
}

}  // namespace

EntitySheet gameplayOf(const SheetText& sheet,
                       const BehaviourRegistry& behaviours) {
  const toml::table root = engine::parseDataFile(sheet.text, sheet.source);
  const engine::DataFileTable gameplay =
      gameplayTableOf(engine::rootOf(root, sheet.source));
  EntitySheet read{
      .name = sheet.name,
      .number = 0,
      .side = sideFrom(gameplay),
      .hitbox = hitboxFrom(gameplay),
      .motion = motionFrom(gameplay),
      .pilot = pilotFrom(gameplay, behaviours),
      .health = pointsIn<int>(gameplay, HEALTH_KEY, HEALTH_RANGE),
      .points = pointsIn<std::uint32_t>(gameplay, SCORE_KEY, SCORE_RANGE),
      .gun = gunFrom(gameplay)};
  checkPilotMatchesSide(read, gameplay);
  checkShipMotion(read, gameplay);
  checkMissileHasNoGun(read, gameplay);
  checkMissileFliesTowardsItsEdge(read, gameplay);
  return read;
}

}  // namespace rtype::game
