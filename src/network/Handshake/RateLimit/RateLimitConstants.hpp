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
 * @brief The connection requests one address and port may send. A client
 * sends a request every 500 ms and the answer to each challenge, so four a
 * second, and after a stall of the server up to ten answers in one tick;
 * twenty drops in a second, about six times what a client sends, ban it.
 */
constexpr std::int64_t REQUESTS_PER_SECOND = 4;
constexpr std::size_t REQUEST_BURST = 10;
constexpr std::size_t REQUEST_DROPS_BEFORE_BAN = 20;
constexpr RateLimit REQUEST_RATE_LIMIT{
    .interval =
        std::chrono::nanoseconds{std::chrono::seconds{1}} / REQUESTS_PER_SECOND,
    .burst = REQUEST_BURST,
    .dropsBeforeBan = REQUEST_DROPS_BEFORE_BAN,
    .dropWindow = BAN_WINDOW,
    .banDuration = BAN_DURATION};

/**
 * @brief The datagrams an admitted player may send with its session token:
 * twice the 60 inputs a second of the protocol, and a second of inputs at once,
 * for a frame that sends several and a network that delivers them together;
 * sixty drops in a second ban it.
 */
constexpr std::int64_t PLAYER_DATAGRAMS_PER_SECOND = 120;
constexpr std::size_t PLAYER_BURST = 60;
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
