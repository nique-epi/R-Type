#pragma once

#include <array>
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
 * @brief Module name the games write their log lines under.
 */
constexpr std::string_view INSTANCES_LOGGER_NAME = "Instances";

constexpr std::string_view GAME_INSTALL_NAME = "game";
constexpr std::string_view RTYPE_RULES_MODULE_NAME = "rtype rules";
constexpr std::string_view REPLICATION_MODULE_NAME = "replication";

constexpr std::string_view GAME_INPUT_STAGE = "game inputs";
constexpr std::string_view GAME_SIMULATION_STAGE = "game simulation";
constexpr std::string_view GAME_OUTPUT_STAGE = "game outputs";
constexpr std::array<std::string_view, 3> GAME_STAGES{
    GAME_INPUT_STAGE, GAME_SIMULATION_STAGE, GAME_OUTPUT_STAGE};

/**
 * @brief Why a game in progress finishes, as its line says after "game
 * finished: ": no player is left in it, or no player left in it has a life.
 */
constexpr std::string_view NO_PLAYER_LEFT_FINISH_REASON = "no player left";
constexpr std::string_view NO_LIFE_LEFT_FINISH_REASON =
    "no player has a life left";

}  // namespace rtype::server
