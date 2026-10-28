#pragma once

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

/**
 * @brief Why a game in progress finishes, as its line says after "game
 * finished: ": no player is left in it, or no player left in it has a life.
 */
constexpr std::string_view NO_PLAYER_LEFT_FINISH_REASON = "no player left";
constexpr std::string_view NO_LIFE_LEFT_FINISH_REASON =
    "no player has a life left";

}  // namespace rtype::server
