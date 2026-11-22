#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::game {

/** @brief The pilots of a game, numbered from FIRST_PILOT_NUMBER: a tally
 * kept for each pilot holds MOST_PILOTS places, pilot n at place
 * n - FIRST_PILOT_NUMBER. The server checks they match the players a game
 * holds on the wire. */
constexpr std::size_t MOST_PILOTS = 4;
constexpr std::uint8_t FIRST_PILOT_NUMBER = 1;

}  // namespace rtype::game
