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
 * SESSION_CLOSING_TICKS ticks after the last datagram the server read from its
 * client, since those clients send nothing more. The Heartbeats are counted in
 * real time instead (see ConnectedPlayer::sinceHeartbeat).
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
 * @brief The time between two Heartbeats the server sends a player:
 * HEARTBEAT_INTERVAL rounded down to whole simulation ticks, 30 ticks or
 * 499.99998 ms at 60 per second. It is the time the counter of a player must
 * reach, and the counter adds the real time since the previous tick started
 * and restarts from zero at each send: two Heartbeats to a player are 30 ticks
 * apart, or 31 (516.7 ms) when the tick that makes one due started less late
 * than the tick of the previous send. Waiting for the whole interval would
 * wait 31 ticks every time. The delays of 5 s and 20 s do not depend on the
 * difference.
 */
constexpr engine::Duration TIME_BETWEEN_HEARTBEATS =
    engine::SIMULATION_TICK_DURATION *
    static_cast<engine::Duration::rep>(
        engine::wholeTicksIn(network::HEARTBEAT_INTERVAL));

static_assert(TIME_BETWEEN_HEARTBEATS > engine::Duration::zero(),
              "HEARTBEAT_INTERVAL must last at least one simulation tick");

/**
 * @brief Logger name the sessions write their log lines under.
 */
constexpr std::string_view SESSIONS_LOGGER_NAME = "Sessions";

/**
 * @brief Why a game drops a datagram whose sender is not one of its players,
 * as its lines say; a header it does not speak is dropped for
 * FOREIGN_HEADER_DROP_REASON.
 */
constexpr std::string_view NOT_A_PLAYER_DROP_REASON =
    "its sender is not a player of this game";

/**
 * @brief Why a game drops a datagram of a player that is not newer than the
 * newest one it accepted from them: a duplicate, or one that came late.
 */
constexpr std::string_view NOT_NEWER_DROP_REASON =
    "it is not newer than the newest datagram of its sender";

}  // namespace rtype::server
