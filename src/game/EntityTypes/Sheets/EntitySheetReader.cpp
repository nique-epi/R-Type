#include "EntitySheetReader.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
#include <toml++/toml.hpp>
#include <vector>
#include "ActionKey.hpp"
#include "BehaviourRegistry.hpp"
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "BurstSheet.hpp"
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetConstants.hpp"
#include "EntityType.hpp"
#include "GameException.hpp"
#include "GunSheet.hpp"
#include "HurtBox.hpp"
#include "HurtWindow.hpp"
#include "HurtZone.hpp"
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

/** @returns How `[gameplay.burst]` bursts a missile; nothing without it. */
std::optional<BurstSheet> burstFrom(const engine::DataFileTable& gameplay) {
  const std::optional<engine::DataFileTable> burst =
      engine::optionalTableAt(gameplay, BURST_KEY);
  if (!burst.has_value()) {
    return std::nullopt;
  }
  engine::refuseUnknownKeys(*burst, BURST_KEYS);
  return BurstSheet{
      .afterTicks = std::max(MINIMUM_BURST_TICKS,
                             nearestTicksIn(engine::millisecondsAt(
                                 *burst, AFTER_KEY, BURST_AFTER_RANGE))),
      .count = static_cast<std::uint8_t>(
          engine::wholeNumberAt(*burst, COUNT_KEY, SHARD_COUNT_RANGE)),
      .shard = engine::textAt(*burst, SHARD_KEY),
      .shardPlace = engine::placeOfKey(*burst, SHARD_KEY)};
}

/** @returns The attack and the phase @p table names, both required.
 * @throws What wholeNumberAt() and choiceAt() throw. */
ActionKey actionKeyAt(const engine::DataFileTable& table) {
  return ActionKey{
      .attack = static_cast<std::uint8_t>(
          engine::wholeNumberAt(table, ATTACK_KEY, ATTACK_NUMBER_RANGE)),
      .phase = PHASES.at(engine::choiceAt(table, PHASE_KEY, PHASE_NAMES))};
}

/** @returns The attack and the phase the box @p box hurts in, nothing for a
 * box that hurts all the time.
 * @throws InconsistentEntitySheetException when it names only one of them. */
std::optional<ActionKey> activeInFrom(const engine::DataFileTable& box) {
  const bool namesAnAttack = box.table.get().contains(ATTACK_KEY);
  const bool namesAPhase = box.table.get().contains(PHASE_KEY);
  if (namesAnAttack != namesAPhase) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(box, namesAnAttack ? ATTACK_KEY : PHASE_KEY),
        HURT_BOX_ACTION_RULE);
  }
  if (!namesAnAttack) {
    return std::nullopt;
  }
  return actionKeyAt(box);
}

/** @returns When @p box hurts within its phase: from `from_ms`, 0 without it,
 * until `until_ms`, the end of the phase without it.
 * @throws InconsistentEntitySheetException when @p box names a time and no
 * phase (@p activeIn empty), or stops hurting no later than it starts. */
HurtWindow windowFrom(const engine::DataFileTable& box,
                      const std::optional<ActionKey>& activeIn) {
  const toml::table& keys = box.table.get();
  if (!activeIn.has_value() &&
      (keys.contains(FROM_KEY) || keys.contains(UNTIL_KEY))) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(box, keys.contains(FROM_KEY) ? FROM_KEY : UNTIL_KEY),
        HURT_BOX_TIMING_RULE);
  }
  HurtWindow window;
  if (keys.contains(FROM_KEY)) {
    window.from = engine::millisecondsAt(box, FROM_KEY, HURT_BOX_TIME_RANGE);
  }
  if (keys.contains(UNTIL_KEY)) {
    window.until = engine::millisecondsAt(box, UNTIL_KEY, HURT_BOX_TIME_RANGE);
  }
  if (window.until.has_value() && *window.until <= window.from) {
    throw InconsistentEntitySheetException(engine::placeOfKey(box, UNTIL_KEY),
                                           HURT_BOX_WINDOW_RULE);
  }
  return window;
}

/** @returns The value of @p key of @p box as a number of logical units. */
float unitsAt(const engine::DataFileTable& box, std::string_view key,
              engine::WholeNumberRange range) {
  return static_cast<float>(engine::wholeNumberAt(box, key, range));
}

/** @returns The box one table of `[[gameplay.hurt_zone.box]]` describes. */
HurtBox hurtBoxFrom(const engine::DataFileTable& box) {
  engine::refuseUnknownKeys(box, HURT_BOX_KEYS);
  const std::optional<ActionKey> activeIn = activeInFrom(box);
  return HurtBox{.offsetX = unitsAt(box, OFFSET_X_KEY, HURT_BOX_OFFSET_RANGE),
                 .offsetY = unitsAt(box, OFFSET_Y_KEY, HURT_BOX_OFFSET_RANGE),
                 .width = unitsAt(box, WIDTH_KEY, HITBOX_SIDE_RANGE),
                 .height = unitsAt(box, HEIGHT_KEY, HITBOX_SIDE_RANGE),
                 .activeIn = activeIn,
                 .window = windowFrom(box, activeIn)};
}

/** @returns The zone of `[gameplay.hurt_zone]`, its boxes in the order of the
 * file; nothing without it.
 * @throws InconsistentEntitySheetException when the zone holds no box. */
std::optional<HurtZone> hurtZoneFrom(const engine::DataFileTable& gameplay) {
  const std::optional<engine::DataFileTable> zone =
      engine::optionalTableAt(gameplay, HURT_ZONE_KEY);
  if (!zone.has_value()) {
    return std::nullopt;
  }
  engine::refuseUnknownKeys(*zone, HURT_ZONE_KEYS);
  HurtZone read;
  for (const engine::DataFileTable& box : engine::tablesAt(*zone, BOX_KEY)) {
    read.boxes.push_back(hurtBoxFrom(box));
  }
  if (read.boxes.empty()) {
    throw InconsistentEntitySheetException(engine::placeOfKey(*zone, BOX_KEY),
                                           HURT_ZONE_BOXES_RULE);
  }
  return read;
}

/** @returns Whether the sheet holds the marker table
 * `[gameplay.survives_contact]`. */
bool survivesContactFrom(const engine::DataFileTable& gameplay) {
  const std::optional<engine::DataFileTable> marker =
      engine::optionalTableAt(gameplay, SURVIVES_CONTACT_KEY);
  if (marker.has_value()) {
    engine::refuseUnknownKeys(*marker, SURVIVES_CONTACT_KEYS);
  }
  return marker.has_value();
}

/** @returns The action `[gameplay.fragile]` names; nothing without it. */
std::optional<ActionKey> fragileFrom(const engine::DataFileTable& gameplay) {
  const std::optional<engine::DataFileTable> fragile =
      engine::optionalTableAt(gameplay, FRAGILE_KEY);
  if (!fragile.has_value()) {
    return std::nullopt;
  }
  engine::refuseUnknownKeys(*fragile, FRAGILE_KEYS);
  return actionKeyAt(*fragile);
}

/** @throws InconsistentEntitySheetException when @p sheet has a hurt zone,
 * survives a contact or is fragile and its side is not enemy. */
void checkContactPartsBelongToAnEnemy(const EntitySheet& sheet,
                                      const engine::DataFileTable& gameplay) {
  if (sheet.side == EntityType::Enemy) {
    return;
  }
  if (sheet.hurtZone.has_value()) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, HURT_ZONE_KEY), HURT_ZONE_SIDE_RULE);
  }
  if (sheet.survivesContact) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, SURVIVES_CONTACT_KEY),
        SURVIVES_CONTACT_SIDE_RULE);
  }
  if (sheet.fragileIn.has_value()) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, FRAGILE_KEY), FRAGILE_SIDE_RULE);
  }
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

/** @throws InconsistentEntitySheetException when a missile has a gun, or
 * another body bursts. */
void checkWhatItFires(const EntitySheet& sheet,
                      const engine::DataFileTable& gameplay) {
  if (isAMissile(sheet.side) && sheet.gun.has_value()) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, GUN_KEY), MISSILE_GUN_RULE);
  }
  if (!isAMissile(sheet.side) && sheet.burst.has_value()) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, BURST_KEY), BURST_SIDE_RULE);
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
      .hitbox = hitboxOf(gameplay),
      .motion = motionFrom(gameplay),
      .pilot = pilotFrom(gameplay, behaviours),
      .health = pointsIn<int>(gameplay, HEALTH_KEY, HEALTH_RANGE),
      .points = pointsIn<std::uint32_t>(gameplay, SCORE_KEY, SCORE_RANGE),
      .gun = gunFrom(gameplay),
      .hurtZone = hurtZoneFrom(gameplay),
      .survivesContact = survivesContactFrom(gameplay),
      .fragileIn = fragileFrom(gameplay),
      .burst = burstFrom(gameplay)};
  checkPilotMatchesSide(read, gameplay);
  checkShipMotion(read, gameplay);
  checkWhatItFires(read, gameplay);
  checkMissileFliesTowardsItsEdge(read, gameplay);
  checkContactPartsBelongToAnEnemy(read, gameplay);
  return read;
}

}  // namespace rtype::game
