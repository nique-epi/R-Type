#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include <utility>
#include "BydoKind.hpp"
#include "WholeNumberRange.hpp"

namespace rtype::server {

/** @brief The table of a waves file that holds one table per difficulty. */
constexpr std::string_view DIFFICULTY_TABLE_KEY = "difficulty";

/**
 * @brief Joins the name of a table of a waves file and one of its keys in the
 * errors: `difficulty.normal.wave_pause_ms`.
 */
constexpr char WAVE_FILE_KEY_SEPARATOR = '.';

/**
 * @brief The keys of a difficulty: the enemies its Bydos are in turn, then one
 * key per value of its WaveCurve, the durations in whole milliseconds.
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

/**
 * @brief Every key the root of a waves file may hold, and every key a
 * difficulty must hold: any other key is refused, so a misspelt key never
 * goes unnoticed.
 */
constexpr std::array ROOT_KEYS{DIFFICULTY_TABLE_KEY};
constexpr std::array WAVE_PLAN_KEYS{ENEMIES_KEY,
                                    FIRST_WAVE_BYDOS_KEY,
                                    BYDOS_ADDED_PER_WAVE_KEY,
                                    FIRST_SPAWN_INTERVAL_KEY,
                                    SPAWN_INTERVAL_CUT_KEY,
                                    SHORTEST_SPAWN_INTERVAL_KEY,
                                    WAVE_PAUSE_KEY};

/**
 * @brief The name a waves file gives each kind of Bydo. A new kind, a boss for
 * one, gets its name here, and its waves can bring it at once.
 */
constexpr std::string_view DRIFTER_NAME = "drifter";
constexpr std::string_view WEAVER_NAME = "weaver";
constexpr std::array ENEMY_NAMES{
    std::pair{DRIFTER_NAME, game::BydoKind::Drifter},
    std::pair{WEAVER_NAME, game::BydoKind::Weaver}};

/**
 * @brief The most Bydos a first wave may hold or a wave may add, and the most
 * names the enemies of a difficulty may list: a hundred, far more than the
 * playfield shows at once.
 */
constexpr std::int64_t MOST_BYDOS_IN_A_WAVE_FILE = 100;

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
constexpr WholeNumberRange ENEMY_LIST_LENGTH{
    .lowest = 1, .highest = MOST_BYDOS_IN_A_WAVE_FILE};
constexpr WholeNumberRange FIRST_WAVE_BYDOS_RANGE{
    .lowest = 1, .highest = MOST_BYDOS_IN_A_WAVE_FILE};
constexpr WholeNumberRange BYDOS_ADDED_PER_WAVE_RANGE{
    .lowest = 0, .highest = MOST_BYDOS_IN_A_WAVE_FILE};
constexpr WholeNumberRange SPAWN_INTERVAL_RANGE{
    .lowest = 1, .highest = LONGEST_WAVE_FILE_MILLISECONDS};
constexpr WholeNumberRange SPAWN_INTERVAL_CUT_RANGE{
    .lowest = 0, .highest = LONGEST_WAVE_FILE_MILLISECONDS};
constexpr WholeNumberRange WAVE_PAUSE_RANGE{
    .lowest = 0, .highest = LONGEST_WAVE_FILE_MILLISECONDS};

}  // namespace rtype::server
