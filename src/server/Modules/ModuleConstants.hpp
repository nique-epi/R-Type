#pragma once

#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::server {

/** @brief The names the modules of the server give to depend on each other. */
constexpr std::string_view LOGGER_MODULE_NAME = "logger";
constexpr std::string_view NETWORK_MODULE_NAME = "network";
constexpr std::string_view GAMES_MODULE_NAME = "games";
constexpr std::string_view LOBBY_MODULE_NAME = "lobby";

/**
 * @brief How long the server waits after the last copy of its farewell to the
 * players before it closes its socket: the tick that queued the copy flushes it
 * at its end, and the next tick is certain to have found the flush done. Two
 * ticks cover the tick in progress and that one.
 */
constexpr engine::Duration FAREWELL_FLUSH_WAIT =
    2 * engine::SIMULATION_TICK_DURATION;

}  // namespace rtype::server
