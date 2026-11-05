#pragma once

#include <chrono>
#include <string_view>
#include "ProtocolConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/** @brief How often a game process tells its supervisor where it stands. */
constexpr engine::Duration STATUS_INTERVAL = std::chrono::seconds(1);

/** @brief How long a game process waits without any datagram of its supervisor
 * before it stops: the supervisor answers each status, so five seconds of
 * silence is a supervisor gone (SESSION_TIMEOUT). */
constexpr engine::Duration SUPERVISOR_SILENCE_LIMIT = network::SESSION_TIMEOUT;

/** @brief How long a game process that had players waits without any before it
 * stops: the time for a player whose connection was lost to come back. */
constexpr engine::Duration EMPTY_GAME_GRACE = std::chrono::seconds(10);

/** @brief How long a game process waits for its first player before it
 * stops. */
constexpr engine::Duration IDLE_GAME_TIMEOUT = std::chrono::seconds(30);

/** @brief Why a game process stops, as its journal says after "stopping: ". */
constexpr std::string_view SUPERVISOR_SILENT_REASON =
    "the supervisor is silent";
constexpr std::string_view EMPTY_GAME_REASON = "no player is left";
constexpr std::string_view IDLE_GAME_REASON = "no player came";
constexpr std::string_view SUPERVISOR_ORDER_REASON = "the supervisor asked it";

}  // namespace rtype::server
