#pragma once

#include <chrono>
#include <optional>
#include "ClockRecalibrations.hpp"
#include "LagometerJournalFigures.hpp"
#include "LagometerReading.hpp"
#include "RoundTripFigures.hpp"
#include "SessionArrivals.hpp"

namespace rtype::client::testing {

/** @brief A period of the journal, and a frame just short of ending it. */
constexpr std::chrono::seconds JOURNAL_PERIOD_IN_THE_TESTS{10};
constexpr std::chrono::milliseconds JUST_SHORT_OF_THE_PERIOD{9'999};

/** @brief The first frame of the journal, during a game: 10 datagrams
 * accepted and 1 missed so far, 12 frames drawn past the newest state, its
 * frame 6 ticks behind the server. */
constexpr LagometerReading JOURNAL_FIRST_FRAME{
    .frame = std::chrono::milliseconds::zero(),
    .figures = std::nullopt,
    .newestRoundTrip = std::nullopt,
    .arrivals = SessionArrivals{.accepted = 10, .missed = 1},
    .bytesReceived = 0,
    .bytesSent = 0,
    .pastTheNewest = 12,
    .delayTicks = 6.0,
    .recalibrations = ClockRecalibrations{.count = 4, .ticks = 0.5}};

/** @brief The frame that ends the period, 10 s later: a ping of 85 ms with a
 * jitter of 33 ms, 300 datagrams accepted and 3 missed so far, 66 frames
 * drawn past the newest state, 6 ticks behind the server. */
constexpr network::RoundTripFigures PING_AT_THE_END{
    .roundTrip = std::chrono::milliseconds{85},
    .jitter = std::chrono::milliseconds{33}};
constexpr LagometerReading JOURNAL_LAST_FRAME{
    .frame = std::chrono::seconds{10},
    .figures = PING_AT_THE_END,
    .newestRoundTrip = std::nullopt,
    .arrivals = SessionArrivals{.accepted = 300, .missed = 3},
    .bytesReceived = 0,
    .bytesSent = 0,
    .pastTheNewest = 66,
    .delayTicks = 6.0,
    .recalibrations = ClockRecalibrations{.count = 22, .ticks = 3.5}};

/** @brief What the period of those two frames holds: 290 accepted and 2
 * missed, 2 frames, 54 drawn past the newest state, 100 ms behind, the clock
 * moved 18 times by 3 ticks (50 ms) in all, and the last frame, 10 s,
 * the longest and the only one over 25 ms. */
constexpr LagometerJournalFigures JOURNAL_PERIOD_FIGURES{
    .period = std::chrono::seconds{10},
    .roundTrip = PING_AT_THE_END,
    .arrivals = SessionArrivals{.accepted = 290, .missed = 2},
    .frames = 2,
    .pastTheNewest = 54,
    .smallestDelay = std::chrono::milliseconds{100},
    .largestDelay = std::chrono::milliseconds{100},
    .recalibrations = ClockRecalibrations{.count = 18, .ticks = 3.0},
    .longestFrame = std::chrono::seconds{10},
    .slowFrames = 1};

/** @brief A frame of exactly LAGOMETER_SLOW_FRAME, 25 ms, which is not slow,
 * and one a millisecond longer, which is. */
constexpr std::chrono::milliseconds AT_THE_SLOW_FRAME{25};
constexpr std::chrono::milliseconds PAST_THE_SLOW_FRAME{26};

/** @brief Delays of frames within a period, in ticks of the server, and the
 * range they make: 6 ticks is 100 ms, 8.4 ticks 140 ms, 7 ticks 116.67 ms. */
constexpr double SIX_TICKS_BEHIND = 6.0;
constexpr double EIGHT_POINT_FOUR_TICKS_BEHIND = 8.4;
constexpr double SEVEN_TICKS_BEHIND = 7.0;
constexpr std::chrono::milliseconds SMALLEST_DELAY_OF_THE_PERIOD{100};
constexpr std::chrono::milliseconds LARGEST_DELAY_OF_THE_PERIOD{140};

/** @brief The line the journal writes for JOURNAL_PERIOD_FIGURES, then for a
 * period before any round trip, 1.5 s long, a frame 2 ticks ahead of the
 * server. */
constexpr const char* JOURNAL_PERIOD_LINE =
    "lagometer over 10 s: ping 85 ms, jitter 33 ms; missed 2 of 292 "
    "datagrams; 2 frames (0/s), 54 drawn past the newest state (5/s); "
    "display 100 to 100 ms behind the server; clock moved 18 times by 50 ms "
    "in all; longest frame 10000 ms, 1 over 25 ms";
constexpr LagometerJournalFigures JOURNAL_WITHOUT_PING{
    .period = std::chrono::milliseconds{1'500},
    .roundTrip = std::nullopt,
    .arrivals = SessionArrivals{},
    .frames = 90,
    .pastTheNewest = 0,
    .smallestDelay = std::chrono::milliseconds{-33},
    .largestDelay = std::chrono::milliseconds{100},
    .recalibrations = ClockRecalibrations{},
    .longestFrame = std::chrono::milliseconds{17},
    .slowFrames = 0};
constexpr const char* JOURNAL_WITHOUT_PING_LINE =
    "lagometer over 1.5 s: ping -- ms, jitter -- ms; missed 0 of 0 "
    "datagrams; 90 frames (60/s), 0 drawn past the newest state (0/s); "
    "display -33 to 100 ms behind the server; clock moved 0 times by 0 ms in "
    "all; longest frame 17 ms, 0 over 25 ms";

}  // namespace rtype::client::testing
