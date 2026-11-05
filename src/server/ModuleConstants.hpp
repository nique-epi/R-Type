#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>
#include "TimeConstants.hpp"

namespace rtype::server {

/** @brief The names the modules of the server give to depend on each other. */
constexpr std::string_view LOGGER_MODULE_NAME = "logger";
constexpr std::string_view NETWORK_MODULE_NAME = "network";
constexpr std::string_view STORAGE_MODULE_NAME = "storage";
constexpr std::string_view GAMES_MODULE_NAME = "games";
constexpr std::string_view LOBBY_MODULE_NAME = "lobby";
constexpr std::string_view INFORMATION_MODULE_NAME = "information";
constexpr std::string_view METRICS_MODULE_NAME = "metrics";
constexpr std::string_view MASTER_MODULE_NAME = "master";
constexpr std::string_view SUPERVISOR_LINK_MODULE_NAME = "supervisor link";
constexpr std::string_view SUPERVISED_GAMES_MODULE_NAME = "supervised games";
constexpr std::string_view SUPERVISOR_LOBBY_MODULE_NAME = "supervisor lobby";

/**
 * @brief How many times the server sends each player its Disconnection when it
 * shuts down, and the time between two copies: it closes at once and cannot
 * wait for an acknowledgement, so the copies make up for a loss. Provisional,
 * see the protocol page.
 */
constexpr std::size_t FAREWELL_COPIES = 3;
constexpr engine::Duration FAREWELL_COPY_INTERVAL =
    std::chrono::milliseconds(50);

/**
 * @brief How long the server waits after the last copy of its farewell to the
 * players before it closes its socket: the tick that queued the copy flushes it
 * at its end, and the next tick is certain to have found the flush done. Two
 * ticks cover the tick in progress and that one.
 */
constexpr engine::Duration FAREWELL_FLUSH_WAIT =
    2 * engine::SIMULATION_TICK_DURATION;

}  // namespace rtype::server
