#pragma once

#include <cstdint>
#include "PocketShades.hpp"

namespace rtype::client {

/** @returns The shades of the pocket of @p number: green for the zero, red
 * for the numbers of RED_ROULETTE_NUMBERS, black for the others. */
[[nodiscard]] PocketShades pocketShadesOf(std::uint8_t number);

/** @returns The color of the pocket of @p number as the result says it: RED,
 * BLACK or GREEN. */
[[nodiscard]] const char* pocketColorWordOf(std::uint8_t number);

}  // namespace rtype::client
