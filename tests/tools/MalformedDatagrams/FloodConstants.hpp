#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "ProtocolConstants.hpp"
#include "RateLimitConstants.hpp"

namespace rtype::tools {

/**
 * @brief The name of the tool, as its usage and its errors print it.
 */
constexpr std::string_view TOOL_PROGRAM_NAME = "malformed_datagrams";

/**
 * @brief The options of the tool: flood a server, or wait for a client.
 */
constexpr std::string_view SERVER_OPTION = "--server";
constexpr std::string_view CLIENT_OPTION = "--client";

/**
 * @brief How long a flood lasts: the minute after which a receiver must
 * still run, its memory stable.
 */
constexpr std::chrono::seconds FLOOD_DURATION{60};

/**
 * @brief How many datagrams a flood of the server sends per second: fewer than
 * PLAYER_DATAGRAMS_PER_SECOND (120), whatever kinds pass its sorting, so the
 * tool, a player with its own token, stays under its rate limit and tests the
 * readers of the game rather than the limit.
 */
constexpr std::size_t SERVER_FLOOD_DATAGRAMS_PER_SECOND = 100;

static_assert(static_cast<std::int64_t>(SERVER_FLOOD_DATAGRAMS_PER_SECOND) <
                  network::PLAYER_DATAGRAMS_PER_SECOND,
              "a flood of the server must stay under the rate limit of a "
              "player");

/**
 * @brief How many datagrams a flood of a client sends per second. A client
 * limits nothing, and takes up to INCOMING_DATAGRAM_QUEUE_CAPACITY (256)
 * datagrams of its server between two frames, so a thousand per second reach
 * its readers.
 */
constexpr std::size_t CLIENT_FLOOD_DATAGRAMS_PER_SECOND = 1000;

/**
 * @brief The seed of every flood: a run sends the same datagrams in the same
 * order, so a failure can be replayed.
 */
constexpr std::uint32_t FLOOD_SEED = 1;

/**
 * @brief The pause of a flood once the datagrams due so far are sent.
 */
constexpr std::chrono::milliseconds PAUSE_BETWEEN_SENDS{1};

/**
 * @brief The pause between two reads of a socket that had nothing waiting.
 */
constexpr std::chrono::milliseconds POLL_INTERVAL{1};

/**
 * @brief How long the tool waits for each answer of a server.
 */
constexpr std::chrono::seconds ANSWER_TIMEOUT{1};

/**
 * @brief How long the tool waits for a client to ask to connect.
 */
constexpr std::chrono::seconds CLIENT_WAIT_TIMEOUT{60};

/**
 * @brief The session token of the ConnectionAccepted the tool sends a client:
 * any value but NO_SESSION_TOKEN.
 */
constexpr std::uint64_t TOOL_SESSION_TOKEN = 1;

/**
 * @brief The sequence of the last datagram before a flood, the
 * ConnectionAccepted of a client: a flood numbers its datagrams from the next
 * one, so each is newer than the one before.
 */
constexpr std::uint16_t SEQUENCE_BEFORE_THE_FLOOD = 1;

/**
 * @brief How long the tool waits, after a flood, for a datagram the client
 * sends in its session: a client in a session sends a Heartbeat every
 * HEARTBEAT_INTERVAL, whatever else it sends, so three intervals leave room
 * for a late one.
 */
constexpr std::chrono::milliseconds CLIENT_SIGN_OF_LIFE_TIMEOUT =
    3 * network::HEARTBEAT_INTERVAL;

}  // namespace rtype::tools
