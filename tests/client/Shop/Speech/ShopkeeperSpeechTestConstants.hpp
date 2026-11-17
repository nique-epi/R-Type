#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::client::testing {

/** @brief The first words of the first two greetings, said to a player who
 * owns no item, then four, the first of three greetings again after the
 * third. */
constexpr std::size_t NO_ITEM = 0;
constexpr std::string_view FIRST_GREETING_WORD = "PRIVET,";
constexpr std::size_t FOUR_ITEMS = 4;
constexpr std::string_view SECOND_GREETING_WORD = "AH,";

}  // namespace rtype::client::testing
