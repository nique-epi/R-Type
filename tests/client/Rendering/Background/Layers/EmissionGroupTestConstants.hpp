#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>

/** @brief The group of the sky events: at most 4 particles at once, the
 * first born after 600 ms, the next ones 800 to 2600 ms apart. */
constexpr std::string_view EVENT_GROUP_NAME = "sky events";
constexpr std::size_t MOST_EVENTS_AT_ONCE = 4;
constexpr std::chrono::milliseconds FIRST_EVENT_DELAY{600};
constexpr std::chrono::milliseconds SHORTEST_EVENT_GAP{800};
constexpr std::chrono::milliseconds LONGEST_EVENT_GAP{2600};

/** @brief Times the tests move the group on by. */
constexpr std::chrono::milliseconds JUST_SHORT_OF_THE_FIRST_EVENT{599};
constexpr std::chrono::milliseconds PAST_THE_FIRST_EVENT{2};
constexpr std::chrono::milliseconds JUST_SHORT_OF_THE_SHORTEST_GAP{797};
constexpr std::chrono::milliseconds REST_OF_THE_LONGEST_GAP{1804};
constexpr std::chrono::milliseconds ONE_FRAME_OF_GROUP{50};
constexpr int FRAMES_OF_A_MINUTE = 1200;

/** @brief Two members, the first four times as likely as the second, and
 * two thousand births: four in five to the first on average, between 1500
 * and 1700 here. */
constexpr double WEIGHT_OF_THE_LIKELY_MEMBER = 4.0;
constexpr double WEIGHT_OF_THE_RARE_MEMBER = 1.0;
constexpr std::size_t TWO_THOUSAND_BIRTHS = 2000;
constexpr std::size_t FEWEST_BIRTHS_OF_THE_LIKELY_MEMBER = 1500;
constexpr std::size_t MOST_BIRTHS_OF_THE_LIKELY_MEMBER = 1700;
