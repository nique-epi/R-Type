#pragma once

#include <chrono>
#include <string_view>

namespace rtype::server {

/**
 * @brief How often the server tells the master its state when nothing changes,
 * the least time between two heartbeats when games change, and how long it
 * waits before it registers again after a refusal or a master that does not
 * answer.
 */
constexpr std::chrono::milliseconds HEARTBEAT_INTERVAL{5'000};
constexpr std::chrono::milliseconds MINIMUM_NOTICE_GAP{1'000};
constexpr std::chrono::milliseconds REGISTRATION_RETRY{5'000};

/** @brief The name a server announces while server-name is left as it is,
 * and why a name is refused. */
constexpr std::string_view DEFAULT_SERVER_NAME = "R-Type server";
constexpr std::string_view INVALID_SERVER_NAME_TEXT =
    "a server name is 1 to 32 bytes of printable UTF-8";

/**
 * @brief The statuses after which the server registers again: the master does
 * not know its key, or does not know it registered.
 */
constexpr int UNKNOWN_KEY_STATUS = 401;
constexpr int UNREGISTERED_SERVER_STATUS = 409;

/** @brief The game version the server announces, from the CMake project. */
constexpr std::string_view SERVER_GAME_VERSION = RTYPE_GAME_VERSION;

}  // namespace rtype::server
