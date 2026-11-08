#pragma once

#include <array>
#include <cstddef>
#include <string_view>
#include "EntityTypesTestConstants.hpp"

namespace rtype::server::testing {

/** @brief The names the tests give the waves files they lay one over the
 * other, in their order, which the errors show. */
constexpr std::array<std::string_view, 3> LAYERED_FILE_NAMES{
    "assets.toml", "resource.toml", "later.toml"};

/**
 * @brief Files laid over calm, each giving one key of calm on line 2: its
 * first wave of 9 Bydos, then of 4, and its enemies, a single Weaver.
 */
constexpr std::string_view NINE_BYDOS_OVER_CALM =
    "[difficulty.calm]\nfirst_wave_bydos = 9\n";
constexpr std::string_view FOUR_BYDOS_OVER_CALM =
    "[difficulty.calm]\nfirst_wave_bydos = 4\n";
constexpr std::string_view A_WEAVER_OVER_CALM =
    "[difficulty.calm]\nenemies = [\"weaver\"]\n";
constexpr std::size_t NINE_BYDOS = 9;
constexpr std::size_t FOUR_BYDOS = 4;
constexpr std::array A_WEAVER_ALONE{game::testing::WEAVER_SHEET};

/**
 * @brief Files laid over calm that break it on line 2: a first wave without
 * Bydos, a misspelt key, a first interval of 1000 ms, shorter than the
 * shortest of calm (1500 ms, line 6 of the first file), and text that is not
 * TOML.
 */
constexpr std::string_view NO_BYDO_OVER_CALM =
    "[difficulty.calm]\nfirst_wave_bydos = 0\n";
constexpr std::string_view MISSPELT_KEY_OVER_CALM =
    "[difficulty.calm]\nfrist_wave_bydos = 9\n";
constexpr std::string_view SHORT_FIRST_INTERVAL_OVER_CALM =
    "[difficulty.calm]\nfirst_spawn_interval_ms = 1000\n";
constexpr std::string_view NOT_TOML_OVER_CALM =
    "[difficulty.calm]\nfirst_wave_bydos = 9 Bydos\n";

/** @brief A file that names a difficulty gale of its own, with one key of
 * the nine it needs. */
constexpr std::string_view PART_OF_GALE =
    "[difficulty.gale]\nwave_pause_ms = 0\n";

/** @brief A file laid over the 16 difficulties of fileOfDifficulties(), one
 * that changes the first of them, and one that adds a seventeenth, both on
 * line 1. */
constexpr std::string_view FIRST_LEVEL_CHANGED =
    "[difficulty.level0]\nfirst_wave_bydos = 9\n";
constexpr std::string_view SEVENTEENTH_LEVEL_ADDED =
    "[difficulty.extra]\nfirst_wave_bydos = 9\n";

/** @brief The names of calm and storm, in alphabetical order. */
constexpr std::array CALM_THEN_STORM{std::string_view{"calm"},
                                     std::string_view{"storm"}};

/**
 * @brief What the errors say: each names the file and the line the value it is
 * about comes from. A value of `first_wave_bydos = ` starts on column 20, one
 * of `shortest_spawn_interval_ms = ` on column 30.
 */
constexpr std::string_view NO_BYDO_IN_THE_RESOURCE_MESSAGE =
    "resource.toml:2:20: difficulty.calm.first_wave_bydos must be an integer "
    "from 1 to 100";
constexpr std::string_view MISSPELT_KEY_IN_THE_RESOURCE_MESSAGE =
    "resource.toml:2:1: unknown key difficulty.calm.frist_wave_bydos";
constexpr std::string_view SHORTEST_OF_THE_ASSETS_MESSAGE =
    "assets.toml:6:30: difficulty.calm.shortest_spawn_interval_ms must be an "
    "integer from 1 to 1000";
constexpr std::string_view SYNTAX_ERROR_IN_THE_RESOURCE_START =
    "resource.toml:2:";
constexpr std::string_view LINE_1_OF_THE_RESOURCE = "resource.toml:1:";
constexpr std::string_view GALE_MISSING_KEY_TEXT =
    "difficulty.gale.first_spawn_interval_ms is missing";

}  // namespace rtype::server::testing
