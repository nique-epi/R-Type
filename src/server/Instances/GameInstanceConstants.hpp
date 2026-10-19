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

}  // namespace rtype::server
