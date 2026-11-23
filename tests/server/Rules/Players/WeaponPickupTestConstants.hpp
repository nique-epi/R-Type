#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

/** @brief The weapons of the shipped ship, by name and by number (their place
 * in its sheet, plus one), a name it does not hold, and their levels. */
constexpr std::string_view RED_WEAPON_NAME = "red";
constexpr std::string_view BLUE_WEAPON_NAME = "blue";
constexpr std::string_view GREEN_WEAPON_NAME = "green";
constexpr std::uint8_t RED_WEAPON_NUMBER = 1;
constexpr std::uint8_t BLUE_WEAPON_NUMBER = 2;
constexpr std::uint8_t FIRST_LEVEL = 1;
constexpr std::uint8_t SECOND_LEVEL = 2;
constexpr std::size_t RED_INDEX = 0;

/** @brief Ticks before the next shot of a gun held, above every interval of
 * the shipped weapons, and below. */
constexpr std::size_t LONG_WAIT_TICKS = 600;
constexpr std::size_t SHORT_WAIT_TICKS = 3;

}  // namespace rtype::server::testing
