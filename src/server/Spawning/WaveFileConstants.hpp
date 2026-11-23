#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include "SipHash.hpp"
#include "WavePlanConstants.hpp"
#include "WaveSpacingKey.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::server {

/** @brief The table of a waves file that holds one table per difficulty. */
constexpr std::string_view DIFFICULTY_TABLE_KEY = "difficulty";

/**
 * @brief The keys of a difficulty: the names of the entity sheets its enemies
 * are in turn, the names of the sheets of the look items its waves bring, then
 * one key per value of its WaveCurve, the durations in whole milliseconds, the
 * last being the number of waves of its level.
 */
constexpr std::string_view ENEMIES_KEY = "enemies";
constexpr std::string_view LOOK_ITEMS_KEY = "look_items";
/** @brief The optional key of a difficulty that spaces its look items: one
 * comes with each wave whose number is a multiple of it, never with the
 * first, from 1 to MOST_WAVES_IN_A_LEVEL; DEFAULT_LOOK_ITEM_EVERY when left
 * out. */
constexpr std::string_view LOOK_ITEM_EVERY_KEY = "look_item_every";
constexpr std::int64_t DEFAULT_LOOK_ITEM_EVERY =
    static_cast<std::int64_t>(LOOK_ITEM_EVERY_WAVE);
/** @brief The optional keys of a difficulty that bring a crystal carrier into
 * its waves: the name of the sheet of an enemy that drops crystals, and how
 * often it comes, with each wave whose number is a multiple of it, never with
 * the first, from 1 to MOST_WAVES_IN_A_LEVEL; DEFAULT_CARRIER_EVERY when left
 * out. */
constexpr std::string_view CARRIER_KEY = "carrier";
constexpr std::string_view CARRIER_EVERY_KEY = "carrier_every";
constexpr std::int64_t DEFAULT_CARRIER_EVERY =
    static_cast<std::int64_t>(CARRIER_EVERY_WAVE);
constexpr std::string_view FIRST_WAVE_BYDOS_KEY = "first_wave_bydos";
constexpr std::string_view BYDOS_ADDED_PER_WAVE_KEY = "bydos_added_per_wave";
constexpr std::string_view FIRST_SPAWN_INTERVAL_KEY = "first_spawn_interval_ms";
constexpr std::string_view SPAWN_INTERVAL_CUT_KEY =
    "spawn_interval_cut_per_wave_ms";
constexpr std::string_view SHORTEST_SPAWN_INTERVAL_KEY =
    "shortest_spawn_interval_ms";
constexpr std::string_view WAVE_PAUSE_KEY = "wave_pause_ms";
constexpr std::string_view LEVEL_WAVES_KEY = "level_waves";
/** @brief The optional key of a difficulty that names the sheet of its boss,
 * a sheet with a table `[gameplay.boss]`. */
constexpr std::string_view BOSS_KEY = "boss";

/** @brief The optional keys of a difficulty that scale, in percent of what
 * the sheets give, from 1 to 1000 (PERCENT_KEY_RANGE) and game::WHOLE_PERCENT
 * when left out: the health of its boss, the health of its other enemies, and
 * the ticks of the guns of those enemies, less for enemies that fire more
 * often. */
constexpr std::string_view BOSS_HEALTH_PERCENT_KEY = "boss_health_percent";
constexpr std::string_view ENEMY_HEALTH_PERCENT_KEY = "enemy_health_percent";
constexpr std::string_view ENEMY_GUN_INTERVAL_PERCENT_KEY =
    "enemy_gun_interval_percent";
constexpr engine::WholeNumberRange PERCENT_KEY_RANGE{.lowest = 1,
                                                     .highest = 1000};

/**
 * @brief Every key the root of a waves file may hold, and every key a
 * difficulty may hold; all are required but `boss`, the three percents,
 * `look_items`, `look_item_every`, `carrier` and `carrier_every`. Any other
 * key is refused, so a misspelt key never goes unnoticed.
 */
constexpr std::array ROOT_KEYS{DIFFICULTY_TABLE_KEY};
constexpr std::array WAVE_PLAN_KEYS{ENEMIES_KEY,
                                    LOOK_ITEMS_KEY,
                                    LOOK_ITEM_EVERY_KEY,
                                    CARRIER_KEY,
                                    CARRIER_EVERY_KEY,
                                    FIRST_WAVE_BYDOS_KEY,
                                    BYDOS_ADDED_PER_WAVE_KEY,
                                    FIRST_SPAWN_INTERVAL_KEY,
                                    SPAWN_INTERVAL_CUT_KEY,
                                    SHORTEST_SPAWN_INTERVAL_KEY,
                                    WAVE_PAUSE_KEY,
                                    LEVEL_WAVES_KEY,
                                    BOSS_KEY,
                                    BOSS_HEALTH_PERCENT_KEY,
                                    ENEMY_HEALTH_PERCENT_KEY,
                                    ENEMY_GUN_INTERVAL_PERCENT_KEY};

/**
 * @brief The most Bydos a first wave may hold or a wave may add, and the most
 * names the enemies of a difficulty may list: a hundred, far more than the
 * playfield shows at once.
 */
constexpr std::int64_t MOST_BYDOS_IN_A_WAVE_FILE = 100;

/**
 * @brief The most waves a level may hold: a hundred, far more than a game
 * plays.
 */
constexpr std::int64_t MOST_WAVES_IN_A_LEVEL = 100;

/**
 * @brief The longest duration a waves file may give, in milliseconds: a
 * minute, far longer than any pause or interval a game needs.
 */
constexpr std::int64_t LONGEST_WAVE_FILE_MILLISECONDS = 60000;

/**
 * @brief What each key accepts. The shortest interval of a difficulty takes
 * the range of the first interval, but no more than the first interval of
 * that difficulty.
 */
constexpr engine::WholeNumberRange ENEMY_LIST_LENGTH{
    .lowest = 1, .highest = MOST_BYDOS_IN_A_WAVE_FILE};
constexpr engine::WholeNumberRange LOOK_ITEM_LIST_LENGTH{
    .lowest = 0, .highest = MOST_BYDOS_IN_A_WAVE_FILE};
constexpr engine::WholeNumberRange FIRST_WAVE_BYDOS_RANGE{
    .lowest = 1, .highest = MOST_BYDOS_IN_A_WAVE_FILE};
constexpr engine::WholeNumberRange BYDOS_ADDED_PER_WAVE_RANGE{
    .lowest = 0, .highest = MOST_BYDOS_IN_A_WAVE_FILE};
constexpr engine::WholeNumberRange SPAWN_INTERVAL_RANGE{
    .lowest = 1, .highest = LONGEST_WAVE_FILE_MILLISECONDS};
constexpr engine::WholeNumberRange SPAWN_INTERVAL_CUT_RANGE{
    .lowest = 0, .highest = LONGEST_WAVE_FILE_MILLISECONDS};
constexpr engine::WholeNumberRange WAVE_PAUSE_RANGE{
    .lowest = 0, .highest = LONGEST_WAVE_FILE_MILLISECONDS};
constexpr engine::WholeNumberRange LEVEL_WAVES_RANGE{
    .lowest = 1, .highest = MOST_WAVES_IN_A_LEVEL};
constexpr engine::WholeNumberRange LOOK_ITEM_EVERY_RANGE{
    .lowest = 1, .highest = MOST_WAVES_IN_A_LEVEL};
constexpr engine::WholeNumberRange CARRIER_EVERY_RANGE{
    .lowest = 1, .highest = MOST_WAVES_IN_A_LEVEL};

/** @brief The keys that space the look items and the crystal carrier of a
 * difficulty, with what each gives when left out and accepts. */
constexpr WaveSpacingKey LOOK_ITEM_SPACING{.key = LOOK_ITEM_EVERY_KEY,
                                           .fallback = DEFAULT_LOOK_ITEM_EVERY,
                                           .range = LOOK_ITEM_EVERY_RANGE};
constexpr WaveSpacingKey CARRIER_SPACING{.key = CARRIER_EVERY_KEY,
                                         .fallback = DEFAULT_CARRIER_EVERY,
                                         .range = CARRIER_EVERY_RANGE};

/**
 * @brief What WaveCatalog::fingerprint() puts between the texts of two waves
 * files laid one over the other: a NUL, which no TOML text holds, so two
 * different sets of files never join into the same text.
 */
constexpr char WAVE_FILE_SEPARATOR = '\0';

/**
 * @brief The key WaveCatalog::fingerprint() hashes a waves file under. Anyone
 * may know it: the fingerprint tells two files apart, it keeps no secret.
 */
constexpr network::SipHashKey WAVES_FINGERPRINT_KEY{};

}  // namespace rtype::server
