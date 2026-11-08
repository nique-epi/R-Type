#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The line of a game of the default difficulty with one life and no
 * friendly fire, as a player reads it. */
constexpr std::string_view LAST_LIFE_LINE = "NORMAL, 1 LIFE, FRIENDLY FIRE OFF";

/** @brief One life: the count a line writes in the singular. */
constexpr std::uint8_t ONE_LIFE = 1;

/** @brief A name of the waves file and the same name in capitals. */
constexpr std::string_view LOWER_NAME = "veteran";
constexpr std::string_view UPPER_NAME = "VETERAN";

}  // namespace rtype::client::testing
