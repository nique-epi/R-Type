#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The game of the design system: three pilots and the taunt
 * MISSION FAILED / SUCCESSFULLY, 26 letters (card "A v2", "Timeline"). */
constexpr std::size_t THREE_PILOTS = 3;
constexpr std::string_view DESIGN_TAUNT_FIRST_LINE = "MISSION FAILED";
constexpr std::string_view DESIGN_TAUNT_SECOND_LINE = "SUCCESSFULLY";

/** @brief The pilots of a test: a score every one of them may have, and a
 * nickname of 16 characters, the longest the protocol allows
 * (MAXIMUM_NICKNAME_SIZE), made of one letter repeated. */
constexpr std::uint32_t BOTTOM_SCORE = 100;
constexpr std::size_t LONGEST_NICKNAME = 16;
constexpr char NICKNAME_LETTER = 'X';

}  // namespace rtype::client::testing
