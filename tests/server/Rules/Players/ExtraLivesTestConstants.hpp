#pragma once

#include <cstdint>

namespace rtype::server::testing {

/** @brief Scores around the steps of the extra lives, 30000 then each 60000
 * past it: 90000, 150000. */
constexpr std::uint32_t JUST_UNDER_THE_FIRST_STEP = 29999;
constexpr std::uint32_t AT_THE_FIRST_STEP = 30000;
constexpr std::uint32_t PAST_THE_FIRST_STEP = 30100;
constexpr std::uint32_t AT_THE_SECOND_STEP = 90000;
constexpr std::uint32_t PAST_THE_THIRD_STEP = 150500;
constexpr std::uint32_t NO_SCORE = 0;

}  // namespace rtype::server::testing
