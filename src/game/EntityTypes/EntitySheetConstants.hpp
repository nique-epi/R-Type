#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "EnemyPhase.hpp"
#include "EntityType.hpp"
#include "MotionKind.hpp"
#include "TimeConstants.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::game {

/** @brief The folder of the assets that holds one sheet per entity type. */
constexpr std::string_view ENTITY_SHEET_FOLDER_IN_ASSETS = "entities";

/** @brief The two tables of a sheet: the server reads the first, the client
 * the second. Nothing else may stand at the root. */
constexpr std::string_view GAMEPLAY_KEY = "gameplay";
constexpr std::string_view APPEARANCE_KEY = "appearance";
constexpr std::array SHEET_ROOT_KEYS{GAMEPLAY_KEY, APPEARANCE_KEY};

/**
 * @brief The key of `[appearance]` that names the sprite sheet of the type,
 * relative to the assets folder, such as `sprites/entities.png`: the client
 * draws from it, and a content pack carries the file it names.
 */
constexpr std::string_view SPRITE_SHEET_KEY = "sheet";

/**
 * @brief The optional tables of `[appearance]` that name what a server may ask
 * the client to draw: `effects`, one table per effect played once, and
 * `looks`, one table per idle loop a body may take instead of its own. Each
 * table is named with a name of the content (isContentName()).
 */
constexpr std::string_view EFFECTS_TABLE_KEY = "effects";
constexpr std::string_view LOOKS_TABLE_KEY = "looks";

/** @brief The most effects, and the most looks, one sheet names: a look is
 * numbered on one byte, and a sheet holding more would be hard to read. */
constexpr std::size_t MOST_EFFECTS = 16;
constexpr std::size_t MOST_LOOKS = 16;

/** @brief What separates the folders of a path inside the assets folder. */
constexpr char ASSET_PATH_SEPARATOR = '/';

/** @brief The keys of `[gameplay]`: two values, then one table each. */
constexpr std::string_view SIDE_KEY = "side";
constexpr std::string_view PILOT_KEY = "pilot";
constexpr std::string_view HITBOX_KEY = "hitbox";
constexpr std::string_view MOTION_KEY = "motion";
constexpr std::string_view BEHAVIOUR_KEY = "behaviour";
constexpr std::string_view HEALTH_KEY = "health";
constexpr std::string_view SCORE_KEY = "score";
constexpr std::string_view GUN_KEY = "gun";
constexpr std::string_view HURT_ZONE_KEY = "hurt_zone";
constexpr std::string_view SURVIVES_CONTACT_KEY = "survives_contact";
constexpr std::string_view FRAGILE_KEY = "fragile";
constexpr std::string_view BURST_KEY = "burst";
constexpr std::string_view VOLLEY_KEY = "volley";
constexpr std::string_view BOSS_KEY = "boss";
constexpr std::string_view LOOK_ITEM_KEY = "look_item";
constexpr std::array GAMEPLAY_KEYS{
    SIDE_KEY,    PILOT_KEY, HITBOX_KEY, MOTION_KEY,    BEHAVIOUR_KEY,
    HEALTH_KEY,  SCORE_KEY, GUN_KEY,    HURT_ZONE_KEY, SURVIVES_CONTACT_KEY,
    FRAGILE_KEY, BURST_KEY, VOLLEY_KEY, BOSS_KEY,      LOOK_ITEM_KEY};

/** @brief The keys of `[gameplay.hitbox]`, in logical units: its size, and
 * where its center is from the Position of the body, 0 without them. */
constexpr std::string_view WIDTH_KEY = "width";
constexpr std::string_view HEIGHT_KEY = "height";
constexpr std::string_view OFFSET_X_KEY = "offset_x";
constexpr std::string_view OFFSET_Y_KEY = "offset_y";
constexpr std::array HITBOX_KEYS{WIDTH_KEY, HEIGHT_KEY, OFFSET_X_KEY,
                                 OFFSET_Y_KEY};

/** @brief The keys of `[gameplay.motion]`: a ship has a kind only, a steady
 * body its top speed too. */
constexpr std::string_view KIND_KEY = "kind";
constexpr std::string_view TOP_SPEED_KEY = "top_speed";
constexpr std::array SHIP_MOTION_KEYS{KIND_KEY};
constexpr std::array STEADY_MOTION_KEYS{KIND_KEY, TOP_SPEED_KEY};

/** @brief The key of `[gameplay.health]` and of `[gameplay.score]`. */
constexpr std::string_view POINTS_KEY = "points";
constexpr std::array POINTS_KEYS{POINTS_KEY};

/** @brief The keys of `[gameplay.gun]`, durations in milliseconds. */
constexpr std::string_view INTERVAL_KEY = "interval_ms";
constexpr std::string_view FIRST_SHOT_KEY = "first_shot_ms";
constexpr std::string_view PROJECTILE_KEY = "projectile";
constexpr std::array GUN_KEYS{INTERVAL_KEY, FIRST_SHOT_KEY, PROJECTILE_KEY};

/** @brief The keys of `[gameplay.burst]`: when a missile bursts, into how
 * many shards, and the sheet of its shards. */
constexpr std::string_view AFTER_KEY = "after_ms";
constexpr std::string_view COUNT_KEY = "count";
constexpr std::string_view SHARD_KEY = "shard";
constexpr std::array BURST_KEYS{AFTER_KEY, COUNT_KEY, SHARD_KEY};

/** @brief The keys of `[gameplay.hurt_zone]`: its boxes, an array of tables
 * `[[gameplay.hurt_zone.box]]`, and its beams, `[[gameplay.hurt_zone.beam]]`,
 * which it may leave out. */
constexpr std::string_view BOX_KEY = "box";
constexpr std::string_view BEAM_KEY = "beam";
constexpr std::array HURT_ZONE_KEYS{BOX_KEY, BEAM_KEY};

/** @brief The keys of a box of a hurt zone, in logical units; a box names the
 * attack and the phase it hurts in together, or neither. */
constexpr std::string_view ATTACK_KEY = "attack";
constexpr std::string_view PHASE_KEY = "phase";
/** @brief When a box hurts within its phase, in milliseconds into it. */
constexpr std::string_view FROM_KEY = "from_ms";
constexpr std::string_view UNTIL_KEY = "until_ms";
constexpr std::array HURT_BOX_KEYS{OFFSET_X_KEY, OFFSET_Y_KEY, WIDTH_KEY,
                                   HEIGHT_KEY,   ATTACK_KEY,   PHASE_KEY,
                                   FROM_KEY,     UNTIL_KEY};

/** @brief The keys of a beam of a hurt zone: its pivot, length and thickness
 * in logical units, its angles in degrees from the left (a positive one turns
 * it down), and the action it hurts in, required, with its window. */
constexpr std::string_view LENGTH_KEY = "length";
constexpr std::string_view THICKNESS_KEY = "thickness";
constexpr std::string_view FROM_DEGREES_KEY = "from_deg";
constexpr std::string_view TO_DEGREES_KEY = "to_deg";
constexpr std::array HURT_BEAM_KEYS{
    OFFSET_X_KEY,   OFFSET_Y_KEY, LENGTH_KEY, THICKNESS_KEY, FROM_DEGREES_KEY,
    TO_DEGREES_KEY, ATTACK_KEY,   PHASE_KEY,  FROM_KEY,      UNTIL_KEY};
constexpr engine::WholeNumberRange BEAM_LENGTH_RANGE{.lowest = 1,
                                                     .highest = 4096};
constexpr engine::WholeNumberRange BEAM_ANGLE_RANGE{.lowest = -180,
                                                    .highest = 180};

/** @brief The keys of a volley, `[[gameplay.volley]]`: the action it is
 * fired on, the sheet and the count of its bodies and where it starts,
 * required; its step, spread (degrees), distance and gap, 0 without them. */
constexpr std::string_view BODY_KEY = "body";
constexpr std::string_view STEP_X_KEY = "step_x";
constexpr std::string_view STEP_Y_KEY = "step_y";
constexpr std::string_view SPREAD_KEY = "spread_deg";
constexpr std::string_view DISTANCE_KEY = "distance";
constexpr std::string_view GAP_FROM_KEY = "gap_from";
constexpr std::string_view GAP_COUNT_KEY = "gap_count";
/** @brief The chance a volley fires when its action starts, in percent,
 * from 1 to 100; 100 when the table leaves it out. */
constexpr std::string_view CHANCE_KEY = "chance_percent";
/** @brief The heading of a volley of one body, or the middle of the fan
 * of a spread, in degrees from the left, a positive angle turning down;
 * 0 when left out. */
constexpr std::string_view HEADING_KEY = "heading_deg";
constexpr engine::WholeNumberRange VOLLEY_HEADING_RANGE{.lowest = -80,
                                                        .highest = 80};
/** @brief How far the start of a volley may move at random each time it
 * fires, on each axis, in units; 0 when left out. */
constexpr std::string_view JITTER_X_KEY = "jitter_x";
constexpr std::string_view JITTER_Y_KEY = "jitter_y";
constexpr engine::WholeNumberRange VOLLEY_JITTER_RANGE{.lowest = 0,
                                                       .highest = 960};
constexpr engine::WholeNumberRange VOLLEY_CHANCE_RANGE{.lowest = 1,
                                                       .highest = 100};
constexpr std::uint8_t VOLLEY_SURE_CHANCE = 100;
constexpr std::array VOLLEY_KEYS{
    ATTACK_KEY,   PHASE_KEY,    BODY_KEY,     COUNT_KEY,
    OFFSET_X_KEY, OFFSET_Y_KEY, STEP_X_KEY,   STEP_Y_KEY,
    SPREAD_KEY,   DISTANCE_KEY, GAP_FROM_KEY, GAP_COUNT_KEY,
    CHANCE_KEY,   HEADING_KEY,  JITTER_X_KEY, JITTER_Y_KEY};
constexpr engine::WholeNumberRange VOLLEY_COUNT_RANGE{.lowest = 1,
                                                      .highest = 16};
/** @brief A volley may start anywhere on the playfield from its enemy: a
 * spore mine is laid across it. */
constexpr engine::WholeNumberRange VOLLEY_OFFSET_RANGE{.lowest = -1920,
                                                       .highest = 1920};
constexpr engine::WholeNumberRange VOLLEY_SPREAD_RANGE{.lowest = 0,
                                                       .highest = 360};
constexpr engine::WholeNumberRange VOLLEY_DISTANCE_RANGE{.lowest = 0,
                                                         .highest = 1024};
constexpr engine::WholeNumberRange VOLLEY_GAP_RANGE{.lowest = 0, .highest = 15};

/** @brief The keys of `[gameplay.boss]`, all required: the percentage of its
 * full health it enrages below, and the attacks of its rage and its arrival. */
constexpr std::string_view RAGE_BELOW_PERCENT_KEY = "rage_below_percent";
constexpr std::string_view RAGE_ATTACK_KEY = "rage_attack";
constexpr std::string_view ARRIVAL_ATTACK_KEY = "arrival_attack";
constexpr std::array BOSS_KEYS{RAGE_BELOW_PERCENT_KEY, RAGE_ATTACK_KEY,
                               ARRIVAL_ATTACK_KEY};
constexpr engine::WholeNumberRange RAGE_PERCENT_RANGE{.lowest = 1,
                                                      .highest = 99};

/** @brief `[gameplay.survives_contact]` is a marker: it holds no key. */
constexpr std::array<std::string_view, 0> SURVIVES_CONTACT_KEYS{};

/** @brief The keys of `[gameplay.fragile]`: the action in which any hit
 * destroys the enemy, both required. */
constexpr std::array FRAGILE_KEYS{ATTACK_KEY, PHASE_KEY};

/** @brief The phases a box may name, and the EnemyPhase of each, in the same
 * order. */
constexpr std::array PHASE_NAMES{
    std::string_view{"signal"}, std::string_view{"strike"},
    std::string_view{"recover"}, std::string_view{"enraged"}};
constexpr std::array PHASES{EnemyPhase::Signal, EnemyPhase::Strike,
                            EnemyPhase::Recover, EnemyPhase::Enraged};
static_assert(PHASE_NAMES.size() == PHASES.size(), "one name per phase");

/** @brief The pilot of a body its player steers; any other pilot names a
 * behaviour. */
constexpr std::string_view PLAYER_PILOT_NAME = "player";

/** @brief The sides a sheet may name, and the EntityType of each, in the same
 * order. */
constexpr std::array SIDE_NAMES{
    std::string_view{"player"}, std::string_view{"enemy"},
    std::string_view{"player_missile"}, std::string_view{"enemy_missile"},
    std::string_view{"look_item"}};
constexpr std::array SIDES{EntityType::Player, EntityType::Enemy,
                           EntityType::PlayerMissile, EntityType::EnemyMissile,
                           EntityType::LookItem};
static_assert(SIDE_NAMES.size() == SIDES.size(), "one name per side");

/** @brief The motions a sheet may name, and the MotionKind of each. */
constexpr std::array MOTION_KIND_NAMES{std::string_view{"ship"},
                                       std::string_view{"steady"}};
constexpr std::array MOTION_KINDS{MotionKind::Ship, MotionKind::Steady};
static_assert(MOTION_KIND_NAMES.size() == MOTION_KINDS.size(),
              "one name per motion");

/** @brief What each number of a sheet accepts. */
constexpr engine::WholeNumberRange HITBOX_SIDE_RANGE{.lowest = 1,
                                                     .highest = 1024};
constexpr engine::WholeNumberRange TOP_SPEED_RANGE{.lowest = 1,
                                                   .highest = 4096};
constexpr engine::WholeNumberRange HEALTH_RANGE{.lowest = 1, .highest = 1000};
constexpr engine::WholeNumberRange SCORE_RANGE{.lowest = 0, .highest = 1000000};
constexpr engine::WholeNumberRange GUN_INTERVAL_RANGE{.lowest = 1,
                                                      .highest = 60000};
constexpr engine::WholeNumberRange FIRST_SHOT_RANGE{.lowest = 0,
                                                    .highest = 60000};
constexpr engine::WholeNumberRange HURT_BOX_OFFSET_RANGE{.lowest = -1024,
                                                         .highest = 1024};
constexpr engine::WholeNumberRange ATTACK_NUMBER_RANGE{.lowest = 0,
                                                       .highest = 255};
constexpr engine::WholeNumberRange HURT_BOX_TIME_RANGE{.lowest = 0,
                                                       .highest = 60000};
constexpr engine::WholeNumberRange BURST_AFTER_RANGE{.lowest = 1,
                                                     .highest = 60000};
constexpr engine::WholeNumberRange SHARD_COUNT_RANGE{.lowest = 1, .highest = 8};

/** @brief Half a simulation tick: added before a duration is cut into whole
 * ticks, so the count is the nearest one. */
constexpr engine::Duration HALF_A_TICK = engine::SIMULATION_TICK_DURATION / 2;

/** @brief The fewest ticks between two shots of a gun, whatever its
 * interval. */
constexpr std::size_t MINIMUM_GUN_INTERVAL_TICKS = 1;

/** @brief The fewest ticks a missile lives before it bursts. */
constexpr std::size_t MINIMUM_BURST_TICKS = 1;

/** @brief Why a sheet is refused, after the key it is about. */
constexpr std::string_view PLAYER_PILOT_RULE =
    "a player pilots a body of side player, and only such a body";
constexpr std::string_view SHIP_MOTION_RULE =
    "a body of side player moves as a ship, and only such a body";
constexpr std::string_view SHIP_HITBOX_RULE =
    "a ship collides with the box shipAfter() keeps on screen";
constexpr std::string_view MISSILE_GUN_RULE = "a missile has no gun";
constexpr std::string_view BURST_SIDE_RULE =
    "only a missile bursts into shards";
constexpr std::string_view BOSS_SIDE_RULE =
    "only a body of side enemy is a boss";
constexpr std::string_view BOSS_HEALTH_RULE =
    "a boss has health, which its bar shows and its rage reads";
constexpr std::string_view VOLLEY_SIDE_RULE =
    "only a body of side enemy fires volleys";
constexpr std::string_view VOLLEY_GAP_RULE =
    "a gap leaves at least one body of the volley: gap_count is below count";
constexpr std::string_view VOLLEY_BODY_SIDE_RULE =
    "a volley is made of enemies or of missiles of an enemy";
constexpr std::string_view VISUAL_NAME_RULE =
    "an effect or a look is named with 1 to 24 lowercase letters, digits or "
    "underscores";
constexpr std::string_view MOST_EFFECTS_RULE =
    "a sheet names at most 16 effects";
constexpr std::string_view MOST_LOOKS_RULE = "a sheet names at most 16 looks";
constexpr std::string_view SHARD_SIDE_RULE =
    "a missile bursts into missiles of its own side";
constexpr std::string_view BEHAVIOUR_TABLE_RULE =
    "only a behaviour that declares settings has a behaviour table";
constexpr std::string_view MISSILE_HEADING_RULE =
    "a missile flies towards the edge that removes it: the right one for a "
    "missile of a player, the left one for a missile of an enemy";
constexpr std::string_view HURT_ZONE_SIDE_RULE =
    "only a body of side enemy has a hurt zone";
constexpr std::string_view SURVIVES_CONTACT_SIDE_RULE =
    "only a body of side enemy survives a contact";
constexpr std::string_view FRAGILE_SIDE_RULE =
    "only a body of side enemy is fragile in one of its actions";
constexpr std::string_view HURT_ZONE_BOXES_RULE =
    "a hurt zone holds at least one box";
constexpr std::string_view HURT_BOX_ACTION_RULE =
    "a box names the attack and the phase it hurts in together, or neither";
constexpr std::string_view HURT_BOX_TIMING_RULE =
    "only a box that names its attack and its phase hurts from or until a time";
constexpr std::string_view HURT_BOX_WINDOW_RULE =
    "a box stops hurting after it starts: until_ms is above from_ms";
constexpr std::string_view HURT_BEAM_SWEEP_RULE =
    "a beam that sweeps (to_deg other than from_deg) names until_ms, the end "
    "of its sweep";
constexpr std::string_view LOOK_ITEM_SIDE_RULE =
    "look_item is given by a sheet of side look_item, and only by it";
constexpr std::string_view LOOK_ITEM_HEADING_RULE =
    "a look item flies to the left, where the playfield removes it";
constexpr std::string_view PROJECTILE_SIDE_RULE =
    "a player fires missiles of a player, an enemy missiles of an enemy";

}  // namespace rtype::game
