#pragma once

#include <array>
#include <cstddef>
#include <string_view>

namespace rtype::tools {

constexpr std::string_view SCRIPT_BENCH_PROGRAM_NAME = "script_budget_bench";

constexpr std::string_view SCRIPTED_BODIES_OPTION = "--scripted";
constexpr std::string_view BULLETS_OPTION = "--bullets";
constexpr std::string_view TICKS_OPTION = "--ticks";
constexpr std::string_view WAITING_OPTION = "--waiting";

constexpr std::size_t DEFAULT_SCRIPTED_BODIES = 300;
constexpr std::size_t DEFAULT_BULLETS = 1000;
/** @brief One minute of a game at 60 ticks per second. */
constexpr std::size_t DEFAULT_BENCH_TICKS = 3600;

/** @brief The ships of a full game: the script reads the nearest one. */
constexpr std::size_t BENCH_SHIPS = 4;
/** @brief The rows the bodies are spread over, so that only some of them are
 * level with a ship and charge. */
constexpr std::size_t BENCH_LANES = 16;

constexpr std::string_view BENCH_SCRIPTED_TYPE = "crimson_drifter";
constexpr std::string_view BENCH_WAITING_TYPE = "bench_waiting";
constexpr std::string_view BENCH_BULLET_TYPE = "enemy_missile";
constexpr std::string_view BENCH_SHIP_TYPE = "ship";

/** @brief The folders of the repository the bench reads its sheets and its
 * script from. */
constexpr std::array<std::string_view, 3> BENCH_CONTENT_FOLDERS{
    "assets", "resources/crimson-drifter",
    "tests/tools/ScriptBudgetBench/content"};

/** @brief The rank of the median among the ticks, in percent. */
constexpr std::size_t MEDIAN_PERCENT = 50;
/** @brief The rank that leaves out the slowest tick of a hundred. */
constexpr std::size_t HIGH_PERCENT = 99;
/** @brief The rank of the slowest tick. */
constexpr std::size_t WORST_PERCENT = 100;

}  // namespace rtype::tools
