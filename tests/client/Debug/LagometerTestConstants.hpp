#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "RoundTripSample.hpp"
#include "SessionArrivals.hpp"

namespace rtype::client::testing {

/** @brief A round trip of 120 ms, the first one measured. */
constexpr RoundTripSample FIRST_SAMPLE{
    .number = 1, .roundTrip = std::chrono::milliseconds{120}};

/** @brief What the session counted before the lagometer read anything, then
 * at its first round trip: 18 more accepted and 2 more missed, 10 % lost. */
constexpr SessionArrivals ARRIVALS_BEFORE{.accepted = 10, .missed = 0};
constexpr SessionArrivals ARRIVALS_AT_FIRST_SAMPLE{.accepted = 28, .missed = 2};
constexpr std::uint64_t ACCEPTED_SINCE_BEFORE = 18;
constexpr std::uint64_t MISSED_SINCE_BEFORE = 2;
constexpr std::uint64_t TEN_PERCENT_LOST = 10;

/** @brief One round trip more than the curve keeps. */
constexpr std::size_t ONE_PAST_THE_CURVE = 61;

/** @brief The round trip of the second of them, the oldest kept: each one
 * lasts as many milliseconds as its number. */
constexpr std::chrono::milliseconds OLDEST_KEPT_ROUND_TRIP{2};

/** @brief Round trips at the lowest top of the scale, 50 ms, and just above
 * it, which doubles it to 100 ms. */
constexpr std::chrono::milliseconds AT_THE_LOWEST_TOP{50};
constexpr std::chrono::microseconds JUST_ABOVE_THE_LOWEST_TOP{50'001};
constexpr std::chrono::milliseconds DOUBLED_TOP{100};

/** @brief Frames of 20 ms: 50 of them make one second, 49 fall short. */
constexpr std::chrono::milliseconds TWENTY_MILLISECONDS{20};
/** @brief A frame 1 ms short of ending the second after 49 frames of 20 ms. */
constexpr std::chrono::milliseconds NINETEEN_MILLISECONDS{19};
constexpr std::size_t FRAMES_IN_A_SECOND = 50;
constexpr std::uint64_t FIFTY_FRAMES_PER_SECOND = 50;

/** @brief Bytes received before the first frame, then after a second: 12000
 * bytes in that second. */
constexpr std::uint64_t BYTES_BEFORE = 1000;
constexpr std::uint64_t BYTES_AFTER_A_SECOND = 13000;
constexpr std::uint64_t BYTES_IN_THAT_SECOND = 12000;

/** @brief The figures of a full panel: 41.6 ms and 3.4 ms read 42 and 3;
 * 12499 bytes read 12 KB, 12500 read 13. */
constexpr std::chrono::microseconds UNROUNDED_ROUND_TRIP{41'600};
constexpr std::chrono::microseconds UNROUNDED_JITTER{3'400};
constexpr std::uint64_t SOME_LOSS_PERCENT = 2;
constexpr std::uint64_t SIXTY_FRAMES_PER_SECOND = 60;
constexpr std::uint64_t BYTES_BELOW_HALF_A_KILOBYTE = 12'499;
constexpr std::uint64_t BYTES_AT_HALF_A_KILOBYTE = 12'500;

/** @brief The three lines of a panel that knows nothing yet, then of the full
 * panel. */
constexpr const char* EMPTY_PING_LINE = "PING -- MS JIT --";
constexpr const char* EMPTY_LOSS_LINE = "LOSS 0% FPS --";
constexpr const char* EMPTY_TRAFFIC_LINE = "IN -- OUT -- KB/S";
constexpr const char* FULL_PING_LINE = "PING 42 MS JIT 3";
constexpr const char* FULL_LOSS_LINE = "LOSS 2% FPS 60";
constexpr const char* FULL_TRAFFIC_LINE = "IN 12 OUT 13 KB/S";

/**
 * @brief Where the curve lies at a scale of 1, in coordinates of the
 * playfield: the panel starts 4 pixels in, the curve 4 pixels further, 36
 * pixels down, 80 pixels wide and 32 tall, each pixel 4 units.
 */
constexpr float CURVE_LEFT = 32.0F;
constexpr float CURVE_RIGHT = 352.0F;
constexpr float CURVE_TOP = 160.0F;
constexpr float CURVE_BOTTOM = 288.0F;
constexpr float CURVE_MIDDLE = 224.0F;
/** @brief One step between two points: 320 units over 59 steps. */
constexpr float CURVE_STEP = 320.0F / 59.0F;
/** @brief Vertices of the three lines of the scale. */
constexpr std::size_t SCALE_LINE_VERTICES = 6;

/** @brief Two round trips, 25 ms then 50 ms, on a scale whose top is 50 ms:
 * the first at half the height, the second at the top. */
constexpr RoundTripSample HALF_THE_TOP{
    .number = 1, .roundTrip = std::chrono::milliseconds{25}};
constexpr RoundTripSample AT_THE_TOP{
    .number = 2, .roundTrip = std::chrono::milliseconds{50}};

/** @brief A round trip at whose point the session missed one datagram. */
constexpr SessionArrivals ONE_MISSED{.accepted = 1, .missed = 1};

}  // namespace rtype::client::testing
