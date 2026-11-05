#pragma once

#include <chrono>
#include <cstdint>

namespace rtype::network::testing {

/**
 * @brief A time on the clock of this side when it builds a Heartbeat, and the
 * same time as the Heartbeat carries it, in microseconds.
 */
constexpr std::chrono::seconds FIRST_SENDING_TIME{1};
constexpr std::uint32_t FIRST_SENDING_TIME_ON_THE_WIRE = 1'000'000;

/**
 * @brief Two Heartbeats of the peer, the times they arrive, and the time this
 * side sends its own after the second: 3 ms after it arrived.
 */
constexpr std::uint32_t OLDER_PEER_SENT_AT = 7;
constexpr std::uint32_t NEWER_PEER_SENT_AT = 9;
constexpr std::chrono::milliseconds OLDER_PEER_ARRIVAL{2000};
constexpr std::chrono::milliseconds NEWER_PEER_ARRIVAL{2500};
constexpr std::chrono::milliseconds SENDING_AFTER_THE_NEWER{2503};
constexpr std::uint32_t THREE_MILLISECONDS_HELD = 3000;

/**
 * @brief A Heartbeat this side sends at 10 s, whose echo the peer held 3 ms
 * and which comes back 45 ms after it left: a round trip of 42 ms.
 */
constexpr std::chrono::seconds MEASURED_SENDING_TIME{10};
constexpr std::chrono::milliseconds ECHO_BACK_AFTER{45};
constexpr std::chrono::milliseconds MEASURED_ROUND_TRIP{42};

/**
 * @brief 1 ms before the microsecond counter of a Heartbeat wraps from
 * 4294967295 to 0: an echo that comes back 45 ms later carries a sentAt above
 * the time of its arrival.
 */
constexpr std::chrono::microseconds ONE_MILLISECOND_BEFORE_THE_WRAP{
    4'294'967'296 - 1'000};

/**
 * @brief 1 ms before the second wrap of the microsecond counter, 2 * 2^32 - 1
 * ms: a time whose full value is above 2^32 µs, so that the counter of a
 * Heartbeat sent then is not the time itself.
 */
constexpr std::chrono::microseconds ONE_MILLISECOND_BEFORE_THE_SECOND_WRAP{
    (2 * 4'294'967'296) - 1'000};

/**
 * @brief A time held longer than the 45 ms since the Heartbeat was sent.
 */
constexpr std::uint32_t HELD_LONGER_THAN_THE_TRIP = 50'000;

/**
 * @brief A time held exactly as long as the 45 ms since the Heartbeat was
 * sent: a round trip of 0.
 */
constexpr std::uint32_t HELD_AS_LONG_AS_THE_TRIP = 45'000;

/**
 * @brief How long after it left an echo of a Heartbeat held for nothing comes
 * back to give exactly the longest round trip, 20 s.
 */
constexpr std::chrono::seconds LONGEST_ROUND_TRIP_ECHO_BACK_AFTER{20};

/**
 * @brief The largest time held a Heartbeat carries, 2^32 - 1 µs.
 */
constexpr std::uint32_t LARGEST_TIME_HELD = 4'294'967'295;

/**
 * @brief An echo that comes back at 30 s of a sentAt of 5 s, 25 s earlier:
 * a round trip longer than the 20 s a session lives without a datagram.
 */
constexpr std::chrono::seconds LATE_ARRIVAL{30};
constexpr std::uint32_t SENT_AT_TWENTY_FIVE_SECONDS_EARLIER = 5'000'000;

/**
 * @brief When a Heartbeat of the peer arrives, then the times this side sends
 * its own: the 20 s a session lives without a datagram after it, and one
 * microsecond more.
 */
constexpr std::chrono::seconds PEER_ARRIVAL{1};
constexpr std::chrono::seconds SENDING_THE_LONGEST_ROUND_TRIP_AFTER{21};
constexpr std::chrono::microseconds SENDING_PAST_THE_LONGEST_ROUND_TRIP_AFTER{
    21'000'001};
constexpr std::uint32_t HELD_THE_LONGEST_ROUND_TRIP = 20'000'000;

}  // namespace rtype::network::testing
