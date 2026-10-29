#pragma once

#include <cstddef>
#include <string_view>
#include "ProtocolConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief SESSION_TIMEOUT in whole simulation ticks, rounded down: 300 ticks at
 * 60 per second. A game counts the ticks it runs, not the time that passes, so
 * a stall of the server, during which the datagrams of the players wait in
 * their queue, closes no session by itself; a stall long enough for the
 * clients to take the server for lost closes each of their sessions,
 * SESSION_TIMEOUT_TICKS ticks after the last datagram the server read from its
 * client, since those clients send nothing more.
 */
constexpr std::size_t SESSION_TIMEOUT_TICKS =
    engine::wholeTicksIn(network::SESSION_TIMEOUT);

static_assert(SESSION_TIMEOUT_TICKS >= 1,
              "SESSION_TIMEOUT must last at least one simulation tick");

/**
 * @brief SESSION_SILENCE_LIMIT in whole simulation ticks, rounded down: 1200
 * ticks at 60 per second. A player not heard from for SESSION_TIMEOUT_TICKS
 * ticks is away; its seat is freed once it has been silent for this many.
 */
constexpr std::size_t SESSION_CLOSING_TICKS =
    engine::wholeTicksIn(network::SESSION_SILENCE_LIMIT);

static_assert(SESSION_CLOSING_TICKS > SESSION_TIMEOUT_TICKS,
              "RECONNECTION_GRACE must last at least one simulation tick");

/**
 * @brief HEARTBEAT_INTERVAL in whole simulation ticks, rounded down: 60 ticks
 * at 60 per second.
 */
constexpr std::size_t TICKS_PER_HEARTBEAT =
    engine::wholeTicksIn(network::HEARTBEAT_INTERVAL);

static_assert(TICKS_PER_HEARTBEAT >= 1,
              "HEARTBEAT_INTERVAL must last at least one simulation tick");

/**
 * @brief Module name the sessions write their log lines under.
 */
constexpr std::string_view SESSIONS_LOGGER_NAME = "Sessions";

/**
 * @brief Why a game drops a datagram whose sender is not one of its players,
 * as its lines say; a header it does not speak is dropped for
 * FOREIGN_HEADER_DROP_REASON.
 */
constexpr std::string_view NOT_A_PLAYER_DROP_REASON =
    "its sender is not a player of this game";

}  // namespace rtype::server
