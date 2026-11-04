#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "ProtocolConstants.hpp"
#include "RateLimit.hpp"

namespace rtype::network {

/**
 * @brief The window both limits below count the drops of a sender in
 * (RateLimit::dropWindow), opened by its first drop: the drop that reaches
 * RateLimit::dropsBeforeBan within it bans the sender.
 */
constexpr std::chrono::seconds BAN_WINDOW{1};

/**
 * @brief How long a ban lasts: more than the 5 s after which a silent player
 * shows away, less than SESSION_SILENCE_LIMIT, so a player banned for its
 * flood comes back without losing its session. Provisional, like the limits
 * below.
 */
constexpr std::chrono::seconds BAN_DURATION{10};

static_assert(BAN_DURATION < SESSION_SILENCE_LIMIT,
              "a ban must end before the session of a banned player closes");

/**
 * @brief The datagrams a joining client sends every
 * CONNECTION_REQUEST_INTERVAL at most: a ConnectionRequest, and its answer to
 * the challenge of the server.
 */
constexpr std::int64_t DATAGRAMS_PER_REQUEST_INTERVAL = 2;

/**
 * @brief The requests one attempt sends before it gives up: one at its start,
 * then one every CONNECTION_REQUEST_INTERVAL while CONNECTION_ATTEMPT_TIMEOUT
 * has not passed, so the timeout divided by the interval and rounded up, 10.
 * After a stall of the server, the client answers the challenge of each of
 * them at once.
 */
constexpr auto REQUESTS_BEFORE_GIVING_UP = static_cast<std::size_t>(
    (CONNECTION_ATTEMPT_TIMEOUT + CONNECTION_REQUEST_INTERVAL -
     std::chrono::nanoseconds{1}) /
    CONNECTION_REQUEST_INTERVAL);

/**
 * @brief The connection requests one address and port may send, from the pace
 * of a joining client: DATAGRAMS_PER_REQUEST_INTERVAL every
 * CONNECTION_REQUEST_INTERVAL, so one every 250 ms; at once, the answers to
 * the REQUESTS_BEFORE_GIVING_UP requests of an attempt after a stall: 10.
 * Twenty drops within BAN_WINDOW ban the sender.
 */
constexpr std::size_t REQUEST_BURST = REQUESTS_BEFORE_GIVING_UP;
constexpr std::size_t REQUEST_DROPS_BEFORE_BAN = 20;
constexpr RateLimit REQUEST_RATE_LIMIT{
    .interval = std::chrono::nanoseconds{CONNECTION_REQUEST_INTERVAL} /
                DATAGRAMS_PER_REQUEST_INTERVAL,
    .burst = REQUEST_BURST,
    .dropsBeforeBan = REQUEST_DROPS_BEFORE_BAN,
    .dropWindow = BAN_WINDOW,
    .banDuration = BAN_DURATION};

/**
 * @brief How many times the inputs of a player its limit lets through: the
 * other messages a player sends during a game, and a frame that sends several
 * inputs, fit in the rest.
 */
constexpr std::int64_t PLAYER_RATE_HEADROOM = 2;

/**
 * @brief The datagrams an admitted player may send with its session token,
 * from the PLAYER_INPUTS_PER_SECOND of the protocol: PLAYER_RATE_HEADROOM
 * times as many a second, 120, and a second of inputs at once, 60, for a
 * network that delivers them together; sixty drops within BAN_WINDOW ban it.
 */
constexpr std::int64_t PLAYER_DATAGRAMS_PER_SECOND =
    PLAYER_RATE_HEADROOM * static_cast<std::int64_t>(PLAYER_INPUTS_PER_SECOND);
constexpr std::size_t PLAYER_BURST = PLAYER_INPUTS_PER_SECOND;
constexpr std::size_t PLAYER_DROPS_BEFORE_BAN = 60;
constexpr RateLimit PLAYER_RATE_LIMIT{
    .interval = std::chrono::nanoseconds{std::chrono::seconds{1}} /
                PLAYER_DATAGRAMS_PER_SECOND,
    .burst = PLAYER_BURST,
    .dropsBeforeBan = PLAYER_DROPS_BEFORE_BAN,
    .dropWindow = BAN_WINDOW,
    .banDuration = BAN_DURATION};

/**
 * @brief The table of RequestAllowances: 256 rows of 4 slots, so 1024 senders
 * of connection requests kept at once.
 */
constexpr std::size_t REQUEST_ALLOWANCE_ROWS = 256;
constexpr std::size_t REQUEST_ALLOWANCE_SLOTS_PER_ROW = 4;

}  // namespace rtype::network
