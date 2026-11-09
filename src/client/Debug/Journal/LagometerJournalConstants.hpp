#pragma once

#include <chrono>
#include <string_view>
#include "TrafficRecordingConstants.hpp"
#include "WindowConstants.hpp"

namespace rtype::client {

/** @brief How often the lagometer writes a line in the journal: as often as
 * the traffic, so the two lines of a period sit next to each other. */
constexpr std::chrono::seconds LAGOMETER_JOURNAL_PERIOD =
    network::TRAFFIC_LINE_PERIOD;

/** @brief A frame longer than this, a frame and a half at
 * FRAMES_PER_SECOND_LIMIT (25 ms), shows the picture before it twice on a
 * screen of that rate: the journal counts it as slow. */
constexpr std::chrono::microseconds LAGOMETER_SLOW_FRAME{
    std::chrono::microseconds{std::chrono::seconds{3}} /
    (2 * FRAMES_PER_SECOND_LIMIT)};

/** @brief Module name the lines of the lagometer go under in the journal. */
constexpr std::string_view LAGOMETER_LOGGER_NAME = "Lagometer";

/** @brief The words of the line of the journal, around its figures:
 * lagometer over 10 s: ping 85 ms, jitter 33 ms; missed 2 of 300 datagrams;
 * 600 frames (60/s), 54 drawn past the newest state (5/s); display 98 to 140
 * ms behind the server. */
constexpr std::string_view JOURNAL_PERIOD_WORD = "lagometer over ";
constexpr std::string_view JOURNAL_PING_WORD = " s: ping ";
constexpr std::string_view JOURNAL_JITTER_WORD = " ms, jitter ";
constexpr std::string_view JOURNAL_MISSED_WORD = " ms; missed ";
constexpr std::string_view JOURNAL_OF_WORD = " of ";
constexpr std::string_view JOURNAL_DATAGRAMS_WORD = " datagrams; ";
constexpr std::string_view JOURNAL_FRAMES_WORD = " frames (";
constexpr std::string_view JOURNAL_FRAMES_END_WORD = "/s), ";
constexpr std::string_view JOURNAL_PAST_THE_NEWEST_WORD =
    " drawn past the newest state (";
constexpr std::string_view JOURNAL_DISPLAY_WORD = "/s); display ";
constexpr std::string_view JOURNAL_TO_WORD = " to ";
constexpr std::string_view JOURNAL_BEHIND_WORD = " ms behind the server";
constexpr std::string_view JOURNAL_CLOCK_WORD = "; clock moved ";
constexpr std::string_view JOURNAL_CLOCK_TIMES_WORD = " times by ";
constexpr std::string_view JOURNAL_CLOCK_TOTAL_WORD =
    " ms in all; longest frame ";
constexpr std::string_view JOURNAL_SLOW_WORD = " ms, ";
constexpr std::string_view JOURNAL_SLOW_OVER_WORD = " over ";
constexpr std::string_view JOURNAL_SLOW_END_WORD = " ms";

}  // namespace rtype::client
