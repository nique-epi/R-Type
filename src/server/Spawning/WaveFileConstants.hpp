#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include "SipHash.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::server {

/** @brief The table of a waves file that holds one table per difficulty. */
constexpr std::string_view DIFFICULTY_TABLE_KEY = "difficulty";

/**
 * @brief The keys of a difficulty: the names of the entity sheets its enemies
 * are in turn, then one
 * key per value of its WaveCurve, the durations in whole milliseconds, the
 * last being the number of waves of its level.
 */
constexpr std::string_view ENEMIES_KEY = "enemies";
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

/** @brief The optional key of a difficulty that scales the health of its
 * boss, in percent of the health its sheet gives, from 1 to 1000;
 * DEFAULT_BOSS_HEALTH_PERCENT when left out. */
constexpr std::string_view BOSS_HEALTH_PERCENT_KEY = "boss_health_percent";
constexpr engine::WholeNumberRange BOSS_HEALTH_PERCENT_RANGE{.lowest = 1,
                                                             .highest = 1000};
constexpr int DEFAULT_BOSS_HEALTH_PERCENT = 100;

/**
 * @brief Every key the root of a waves file may hold, and every key a
 * difficulty may hold; all are required but `boss` and
 * `boss_health_percent`. Any other key is refused,
 * so a misspelt key never goes unnoticed.
 */
constexpr std::array ROOT_KEYS{DIFFICULTY_TABLE_KEY};
constexpr std::array WAVE_PLAN_KEYS{ENEMIES_KEY,
                                    FIRST_WAVE_BYDOS_KEY,
                                    BYDOS_ADDED_PER_WAVE_KEY,
                                    FIRST_SPAWN_INTERVAL_KEY,
                                    SPAWN_INTERVAL_CUT_KEY,
                                    SHORTEST_SPAWN_INTERVAL_KEY,
                                    WAVE_PAUSE_KEY,
                                    LEVEL_WAVES_KEY,
                                    BOSS_KEY,
                                    BOSS_HEALTH_PERCENT_KEY};

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

/**
 * @brief The key WaveCatalog::fingerprint() hashes a waves file under. Anyone
 * may know it: the fingerprint tells two files apart, it keeps no secret.
 */
constexpr network::SipHashKey WAVES_FINGERPRINT_KEY{};

}  // namespace rtype::server
