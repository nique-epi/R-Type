#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "RateLimit.hpp"
#include "SipHash.hpp"

namespace rtype::network::testing {

/**
 * @brief A limit small enough to count by hand: three at once, then one every
 * 100 ms; the second drop within two seconds bans for 10 s. The window differs
 * from BAN_WINDOW, so the tests tell the window of the limit from that one.
 */
constexpr std::chrono::milliseconds TEST_INTERVAL{100};
constexpr std::size_t TEST_BURST = 3;
constexpr std::size_t TEST_DROPS_BEFORE_BAN = 2;
constexpr std::chrono::seconds TEST_DROP_WINDOW{2};
constexpr std::chrono::seconds TEST_BAN_DURATION{10};
constexpr RateLimit TEST_RATE_LIMIT{.interval = TEST_INTERVAL,
                                    .burst = TEST_BURST,
                                    .dropsBeforeBan = TEST_DROPS_BEFORE_BAN,
                                    .dropWindow = TEST_DROP_WINDOW,
                                    .banDuration = TEST_BAN_DURATION};

/**
 * @brief The limit of the tests with a ban of 100 ms, shorter than the 300 ms
 * a burst of three takes to come back: only forgetting what the sender spent
 * gives it a whole burst at the end of that ban.
 */
constexpr std::chrono::milliseconds SHORT_BAN_DURATION{100};
constexpr RateLimit SHORT_BAN_RATE_LIMIT{
    .interval = TEST_INTERVAL,
    .burst = TEST_BURST,
    .dropsBeforeBan = TEST_DROPS_BEFORE_BAN,
    .dropWindow = TEST_DROP_WINDOW,
    .banDuration = SHORT_BAN_DURATION};

/** @brief When the tests start. */
constexpr std::chrono::nanoseconds START{0};

/** @brief A silence that would refill far more than a burst. */
constexpr std::chrono::seconds LONG_SILENCE{10};

/** @brief Halfway through a window of drops of TEST_DROP_WINDOW. */
constexpr std::chrono::seconds HALFWAY_THROUGH_THE_DROP_WINDOW =
    TEST_DROP_WINDOW / 2;

/** @brief Halfway through a ban of TEST_BAN_DURATION. */
constexpr std::chrono::seconds HALFWAY_THROUGH_THE_BAN = TEST_BAN_DURATION / 2;

/** @brief How many datagrams a steady or a flooding sender sends in a test. */
constexpr std::size_t STEADY_DATAGRAM_COUNT = 100;
constexpr std::size_t FLOOD_DATAGRAM_COUNT = 1000;

/** @brief The key the table of the tests picks its rows with. */
constexpr SipHashKey TEST_KEY{};

/**
 * @brief How many forged senders pass by in the tests of the table: about 39
 * for each of its 256 rows.
 */
constexpr std::size_t FORGED_SENDER_COUNT = 10000;

/** @brief Where the forged senders of the tests send from, a port each. */
constexpr std::string_view FORGED_ADDRESS = "198.51.100.1";
constexpr std::uint16_t FIRST_FORGED_PORT = 10000;

/**
 * @brief Five forged senders whose row under TEST_KEY is the same, row 190,
 * computed with a SipHash-2-4 written in Python and calibrated on the vectors
 * of SipHashTest. Their names say when each of the first four is banned; they
 * fill the row in the order of their numbers, so the ban that ends first is
 * not the one of its first slot. The fifth arrives once all four are banned.
 */
constexpr std::size_t ROW_SENDER_BANNED_THIRD = 0;
constexpr std::size_t ROW_SENDER_BANNED_FIRST = 63;
constexpr std::size_t ROW_SENDER_BANNED_LAST = 88;
constexpr std::size_t ROW_SENDER_BANNED_SECOND = 150;
constexpr std::size_t ROW_SENDER_ARRIVING_LAST = 171;

/** @brief The time between two bans of the senders of that row. */
constexpr std::chrono::milliseconds TIME_BETWEEN_BANS{1};

}  // namespace rtype::network::testing
