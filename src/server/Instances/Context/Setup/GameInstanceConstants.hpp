#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief How long a game stays Finished before it waits for players again.
 */
constexpr std::size_t FINISHED_STATE_SECONDS = 5;

/**
 * @brief FINISHED_STATE_SECONDS, in simulation ticks.
 */
constexpr std::size_t FINISHED_STATE_TICKS =
    FINISHED_STATE_SECONDS * engine::SIMULATION_TICKS_PER_SECOND;

/**
 * @brief How long a game waits, once every player is ready, for every player to
 * tell it the game screen is shown, before it starts without the others.
 */
constexpr std::size_t START_BARRIER_SECONDS = 10;

/**
 * @brief START_BARRIER_SECONDS, in simulation ticks.
 */
constexpr std::size_t START_BARRIER_TICKS =
    START_BARRIER_SECONDS * engine::SIMULATION_TICKS_PER_SECOND;

/**
 * @brief Ticks between two copies of GameStarted sent to the players who did
 * not yet tell the game they are loaded: half a second.
 */
constexpr std::size_t GAME_STARTED_REPEAT_TICKS =
    engine::SIMULATION_TICKS_PER_SECOND / 2;

/**
 * @brief Logger name the games write their log lines under.
 */
constexpr std::string_view INSTANCES_LOGGER_NAME = "Instances";

constexpr std::string_view GAME_INSTALL_NAME = "game";
constexpr std::string_view RTYPE_RULES_MODULE_NAME = "rtype rules";
constexpr std::string_view REPLICATION_MODULE_NAME = "replication";
constexpr std::string_view RESULTS_MODULE_NAME = "results";
constexpr std::string_view CHAT_MODULE_NAME = "chat";
constexpr std::string_view LOOKS_MODULE_NAME = "looks";

/**
 * @brief The least time between two answers to the same player: the time after
 * which its client asks again. Provisional, see the protocol page.
 */
constexpr engine::Duration LEADERBOARD_ANSWER_INTERVAL =
    std::chrono::milliseconds(500);

/**
 * @brief LEADERBOARD_ANSWER_INTERVAL, in whole simulation ticks.
 */
constexpr std::size_t TICKS_BETWEEN_LEADERBOARD_ANSWERS =
    engine::wholeTicksIn(LEADERBOARD_ANSWER_INTERVAL);

constexpr std::string_view GAME_INPUT_STAGE = "game inputs";
constexpr std::string_view GAME_SIMULATION_STAGE = "game simulation";
constexpr std::string_view GAME_OUTPUT_STAGE = "game outputs";
constexpr std::array<std::string_view, 3> GAME_STAGES{
    GAME_INPUT_STAGE, GAME_SIMULATION_STAGE, GAME_OUTPUT_STAGE};

/**
 * @brief Why a game in progress finishes, as its line says after "game
 * finished: ": no player is left in it, no player left in it has a life, the
 * last wave of the level is over, its boss is defeated, or the level a script
 * directs is cleared or failed.
 */
constexpr std::string_view NO_PLAYER_LEFT_FINISH_REASON = "no player left";
constexpr std::string_view NO_LIFE_LEFT_FINISH_REASON =
    "no player has a life left";
constexpr std::string_view LAST_WAVE_OVER_FINISH_REASON =
    "the last wave is over";
constexpr std::string_view BOSS_DEFEATED_FINISH_REASON = "the boss is defeated";
constexpr std::string_view LEVEL_CLEARED_FINISH_REASON = "the level is cleared";
constexpr std::string_view LEVEL_FAILED_FINISH_REASON =
    "the script of the level failed";

/** @brief In a game started at its boss, the ticks of invulnerability each
 * ship is kept at, at every tick: more than the one tick LifeKeeper takes
 * off, so the ship never loses it. */
constexpr std::size_t SHIELDED_SHIP_TICKS = 2;

}  // namespace rtype::server
