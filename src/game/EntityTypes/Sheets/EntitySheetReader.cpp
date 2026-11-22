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
#include "BossSheet.hpp"
#include "BurstSheet.hpp"
#include "CollisionSheet.hpp"
#include "ContentName.hpp"
#include "ContentNameConstants.hpp"
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EntitySheet.hpp"
#include "EntitySheetConstants.hpp"
#include "EntityType.hpp"
#include "GameException.hpp"
#include "GunSheet.hpp"
#include "IBehaviour.hpp"
#include "IHitHandler.hpp"
#include "IsAMissile.hpp"
#include "MotionKind.hpp"
#include "MotionSheet.hpp"
#include "PilotSheet.hpp"
#include "Position.hpp"
#include "SheetActionReading.hpp"
#include "SheetText.hpp"
#include "SheetTextReading.hpp"
#include "SheetVisualNames.hpp"
#include "ShipControlConstants.hpp"
#include "ShipLookConstants.hpp"
#include "ShipLookNumbers.hpp"
#include "TimeConstants.hpp"
#include "VolleySheetReading.hpp"
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

/** @returns The layer `layer` names and the layers `hits` lists, none
 * without the key; the filter stays empty until EntitySheets gives each layer
 * its bit.
 * @throws InvalidCollisionLayerNameException when the layer is not named as an
 * entity type is. */
CollisionSheet collisionFrom(const engine::DataFileTable& gameplay) {
  CollisionSheet collision{
      .layer = engine::textAt(gameplay, LAYER_KEY), .hits = {}, .filter = {}};
  if (!identifiers::isContentName(collision.layer)) {
    throw InvalidCollisionLayerNameException(
        engine::placeOfKey(gameplay, LAYER_KEY),
        identifiers::MAXIMUM_CONTENT_NAME_LENGTH);
  }
  if (gameplay.table.get().contains(HITS_KEY)) {
    collision.hits = engine::textsAt(gameplay, HITS_KEY);
  }
  return collision;
}

/** @returns The motion `[gameplay.motion]` describes: a ship and a body that
 * follows the scroll with their kind only, a steady body with its top speed
 * too. */
MotionSheet motionFrom(const engine::DataFileTable& gameplay) {
  const engine::DataFileTable motion = engine::tableAt(gameplay, MOTION_KEY);
  const MotionKind kind =
      MOTION_KINDS.at(engine::choiceAt(motion, KIND_KEY, MOTION_KIND_NAMES));
  if (kind != MotionKind::Steady) {
    engine::refuseUnknownKeys(motion, KIND_ONLY_MOTION_KEYS);
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

/** @returns The handler `on_hit` names among those of @p behaviours; nullptr
 * without the key.
 * @throws engine::UnknownDataFileChoiceException when it names none of them.
 */
IHitHandler* onHitFrom(const engine::DataFileTable& gameplay,
                       const BehaviourRegistry& behaviours) {
  if (!gameplay.table.get().contains(ON_HIT_KEY)) {
    return nullptr;
  }
  const std::vector<std::string_view> names = behaviours.hitHandlerNames();
  return behaviours.findHitHandler(
      names.at(engine::choiceAt(gameplay, ON_HIT_KEY, names)));
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

/** @returns The boss `[gameplay.boss]` describes; nothing without it. */
std::optional<BossSheet> bossFrom(const engine::DataFileTable& gameplay) {
  const std::optional<engine::DataFileTable> boss =
      engine::optionalTableAt(gameplay, BOSS_KEY);
  if (!boss.has_value()) {
    return std::nullopt;
  }
  engine::refuseUnknownKeys(*boss, BOSS_KEYS);
  return BossSheet{
      .rageBelowPercent = static_cast<int>(engine::wholeNumberAt(
          *boss, RAGE_BELOW_PERCENT_KEY, RAGE_PERCENT_RANGE)),
      .rageAttack = static_cast<std::uint8_t>(
          engine::wholeNumberAt(*boss, RAGE_ATTACK_KEY, ATTACK_NUMBER_RANGE)),
      .arrivalAttack = static_cast<std::uint8_t>(engine::wholeNumberAt(
          *boss, ARRIVAL_ATTACK_KEY, ATTACK_NUMBER_RANGE)),
      .bounty = static_cast<std::uint32_t>(
          engine::wholeNumberOrZeroAt(*boss, BOUNTY_KEY, BOUNTY_RANGE))};
}

/** @returns Whether the sheet holds the marker table @p key of `[gameplay]`,
 * such as `[gameplay.survives_contact]`. */
bool holdsMarker(const engine::DataFileTable& gameplay, std::string_view key) {
  const std::optional<engine::DataFileTable> marker =
      engine::optionalTableAt(gameplay, key);
  if (marker.has_value()) {
    engine::refuseUnknownKeys(*marker, MARKER_KEYS);
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

/** @returns The number of the item `look_item` names in the catalog of ship
 * looks; nothing without the key.
 * @throws engine::UnknownDataFileChoiceException when it names no item. */
std::optional<std::uint8_t> lookItemFrom(
    const engine::DataFileTable& gameplay) {
  if (!gameplay.table.get().contains(LOOK_ITEM_KEY)) {
    return std::nullopt;
  }
  return identifiers::lookNumberOf(engine::choiceAt(
      gameplay, LOOK_ITEM_KEY,
      std::span<const std::string_view>{identifiers::SHIP_LOOK_ITEM_NAMES}
          .first(identifiers::SHIP_LOOK_ACCESSORY_COUNT)));
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
  if (sheet.staysOutside) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, STAYS_OUTSIDE_KEY),
        STAYS_OUTSIDE_SIDE_RULE);
  }
  if (!sheet.volleys.empty()) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, VOLLEY_KEY), VOLLEY_SIDE_RULE);
  }
  if (sheet.boss.has_value()) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, BOSS_KEY), BOSS_SIDE_RULE);
  }
}

/** @throws InconsistentEntitySheetException when @p sheet is a boss without
 * health. */
void checkBossHasHealth(const EntitySheet& sheet,
                        const engine::DataFileTable& gameplay) {
  if (sheet.boss.has_value() && !sheet.health.has_value()) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, BOSS_KEY), BOSS_HEALTH_RULE);
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
  if (movesAsAShip &&
      (sheet.hitbox.width != SHIP_WIDTH || sheet.hitbox.height != SHIP_HEIGHT ||
       sheet.hitbox.offsetX != 0.0F || sheet.hitbox.offsetY != 0.0F)) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, HITBOX_KEY), SHIP_HITBOX_RULE);
  }
}

/** @throws InconsistentEntitySheetException when @p sheet lists several boxes
 * and its side is not enemy: a ship, a missile and an item keep one box. */
void checkHitboxPartsBelongToAnEnemy(const EntitySheet& sheet,
                                     const engine::DataFileTable& gameplay) {
  if (!sheet.hitboxParts.empty() && sheet.side != EntityType::Enemy) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, HITBOX_KEY), HITBOX_PARTS_SIDE_RULE);
  }
}

/** @throws InconsistentEntitySheetException when a sheet of side look_item
 * gives no item, or another side gives one. */
void checkLookItemMatchesSide(const EntitySheet& sheet,
                              const engine::DataFileTable& gameplay) {
  if (sheet.lookItem.has_value() != (sheet.side == EntityType::LookItem)) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, SIDE_KEY), LOOK_ITEM_SIDE_RULE);
  }
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

/** @throws InconsistentEntitySheetException when the pilot of a look item does
 * not steer it, on its first tick, to the left: such an item that nobody picks
 * up would stay for the rest of the game. */
void checkLookItemFliesLeft(const EntitySheet& sheet,
                            const engine::DataFileTable& gameplay) {
  if (sheet.side != EntityType::LookItem || !sheet.pilot.has_value()) {
    return;
  }
  if (firstHorizontalSteeringOf(*sheet.pilot, sheet.motion) >= 0.0F) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, PILOT_KEY), LOOK_ITEM_HEADING_RULE);
  }
}

/** @throws InconsistentEntitySheetException when @p sheet names a damage and
 * is no missile. */
void checkDamageBelongsToAMissile(const EntitySheet& sheet,
                                  const engine::DataFileTable& gameplay) {
  if (sheet.damage.has_value() && !isAMissile(sheet.side)) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, DAMAGE_KEY), DAMAGE_SIDE_RULE);
  }
}

/** @throws InconsistentEntitySheetException when a sheet of side obstacle
 * has no obstacle table, or another sheet has one. */
void checkObstacleMatchesSide(const EntitySheet& sheet,
                              const engine::DataFileTable& gameplay) {
  if (sheet.obstacle.has_value() != (sheet.side == EntityType::Obstacle)) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, SIDE_KEY), OBSTACLE_SIDE_RULE);
  }
}

/** @throws InconsistentEntitySheetException when a body that follows the
 * scroll is neither an enemy nor an obstacle. */
void checkScrollMotionSide(const EntitySheet& sheet,
                           const engine::DataFileTable& gameplay) {
  if (sheet.motion.kind == MotionKind::Scroll &&
      sheet.side != EntityType::Enemy && sheet.side != EntityType::Obstacle) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, MOTION_KEY), SCROLL_MOTION_SIDE_RULE);
  }
}

/** @throws InconsistentEntitySheetException when a ship names an on_hit. */
void checkOnHitBelongsToABody(const EntitySheet& sheet,
                              const engine::DataFileTable& gameplay) {
  if (sheet.onHit != nullptr && sheet.side == EntityType::Player) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, ON_HIT_KEY), ON_HIT_SIDE_RULE);
  }
}

/** @throws InconsistentEntitySheetException when @p sheet gives a mask and
 * lists several hitbox boxes: which one it is touched on would be unclear. */
void checkMaskStandsAlone(const EntitySheet& sheet,
                          const engine::DataFileTable& gameplay) {
  if (sheet.mask.has_value() && !sheet.hitboxParts.empty()) {
    throw InconsistentEntitySheetException(
        engine::placeOfKey(gameplay, MASK_KEY), MASK_AND_PARTS_RULE);
  }
}

/** @throws InconsistentEntitySheetException when the values of @p sheet, each
 * valid, do not hold together. */
void checkSheetHoldsTogether(const EntitySheet& sheet,
                             const engine::DataFileTable& gameplay) {
  checkPilotMatchesSide(sheet, gameplay);
  checkShipMotion(sheet, gameplay);
  checkHitboxPartsBelongToAnEnemy(sheet, gameplay);
  checkMaskStandsAlone(sheet, gameplay);
  checkWhatItFires(sheet, gameplay);
  checkMissileFliesTowardsItsEdge(sheet, gameplay);
  checkContactPartsBelongToAnEnemy(sheet, gameplay);
  checkBossHasHealth(sheet, gameplay);
  checkLookItemMatchesSide(sheet, gameplay);
  checkLookItemFliesLeft(sheet, gameplay);
  checkDamageBelongsToAMissile(sheet, gameplay);
  checkObstacleMatchesSide(sheet, gameplay);
  checkScrollMotionSide(sheet, gameplay);
  checkOnHitBelongsToABody(sheet, gameplay);
}

}  // namespace

EntitySheet gameplayOf(const SheetText& sheet,
                       const BehaviourRegistry& behaviours) {
  const toml::table root = engine::parseDataFile(sheet.text, sheet.source);
  const engine::DataFileTable sheetRoot = engine::rootOf(root, sheet.source);
  const engine::DataFileTable gameplay = gameplayTableOf(sheetRoot);
  EntitySheet read{
      .name = sheet.name,
      .number = 0,
      .side = sideFrom(gameplay),
      .collision = collisionFrom(gameplay),
      .hitbox = hitboxOf(gameplay),
      .hitboxParts = hitboxPartsOf(gameplay),
      .mask = maskOf(gameplay, sheet),
      .motion = motionFrom(gameplay),
      .pilot = pilotFrom(gameplay, behaviours),
      .health = pointsIn<int>(gameplay, HEALTH_KEY, HEALTH_RANGE),
      .points = pointsIn<std::uint32_t>(gameplay, SCORE_KEY, SCORE_RANGE),
      .gun = gunFrom(gameplay),
      .hurtZone = hurtZoneOf(gameplay),
      .survivesContact = holdsMarker(gameplay, SURVIVES_CONTACT_KEY),
      .fragileIn = fragileFrom(gameplay),
      .staysOutside = holdsMarker(gameplay, STAYS_OUTSIDE_KEY),
      .burst = burstFrom(gameplay),
      .volleys = volleysFrom(gameplay),
      .boss = bossFrom(gameplay),
      .lookItem = lookItemFrom(gameplay),
      .visualNames = visualNamesOf(sheetTableOf(sheetRoot, APPEARANCE_KEY)),
      .damage = pointsIn<int>(gameplay, DAMAGE_KEY, DAMAGE_RANGE),
      .obstacle = obstacleContactOf(gameplay),
      .onHit = onHitFrom(gameplay, behaviours)};
  checkSheetHoldsTogether(read, gameplay);
  return read;
}

}  // namespace rtype::game
