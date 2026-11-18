#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include "ClothBand.hpp"
#include "ClothPlace.hpp"

namespace rtype::client::testing {

/** @brief The cloth of a single zero roulette, as the rules of the server
 * describe it: 0 to 36, streets of three, dozens from 1, 13 and 25. */
constexpr unsigned int HIGHEST_NUMBER = 36;
constexpr unsigned int STREET_LENGTH = 3;
constexpr std::array<unsigned int, 3> DOZEN_FIRSTS{1, 13, 25};

/** @brief How many bets of each kind a single zero cloth holds: 37 straights,
 * 24 splits in a street, 33 across, 12 streets, 22 corners, 11 six lines,
 * 3 dozens, 3 columns, one bet of each even chance. */
constexpr std::size_t STRAIGHTS = 37;
constexpr std::size_t SPLITS = 24;
constexpr std::size_t SPLITS_ACROSS = 33;
constexpr std::size_t STREETS = 12;
constexpr std::size_t CORNERS = 22;
constexpr std::size_t SIX_LINES = 11;
constexpr std::size_t DOZENS = 3;
constexpr std::size_t COLUMNS = 3;
constexpr std::size_t ONE_BET = 1;

/** @brief Places of the cloth and the bet each holds, read on the picture of
 * the cloth: the band of the numbers counts its steps across from the zero,
 * a number then a line, and down from the top row (3, 6, 9...). */
constexpr ClothPlace ON_SEVENTEEN{.across = 11, .down = 2};
constexpr std::uint8_t SEVENTEEN = 17;
constexpr ClothPlace ON_THE_ZERO{.across = 0, .down = 4};
constexpr ClothPlace BETWEEN_TWO_AND_THREE{.across = 1, .down = 1};
constexpr std::uint8_t TWO = 2;
constexpr ClothPlace BETWEEN_ONE_AND_FOUR{.across = 2, .down = 4};
constexpr std::uint8_t ONE = 1;
constexpr ClothPlace AMONG_ONE_TWO_FOUR_FIVE{.across = 2, .down = 3};
constexpr ClothPlace AMONG_THIRTY_TWO_TO_THIRTY_SIX{.across = 22, .down = 1};
constexpr std::uint8_t THIRTY_TWO = 32;
constexpr ClothPlace UNDER_THE_LAST_STREET{.across = 23, .down = 5};
constexpr std::uint8_t THIRTY_FOUR = 34;
constexpr ClothPlace UNDER_THE_LAST_SIX_LINE{.across = 22, .down = 5};
constexpr std::uint8_t THIRTY_ONE = 31;
constexpr ClothPlace ON_THE_TOP_COLUMN{.across = 24, .down = 0};
constexpr std::uint8_t THREE = 3;
constexpr ClothPlace ON_THE_THIRD_DOZEN{.band = ClothBand::Dozens, .across = 2};
constexpr std::uint8_t TWENTY_FIVE = 25;
constexpr ClothPlace ON_THE_RED_DIAMOND{.band = ClothBand::EvenChances,
                                        .across = 2};
constexpr ClothPlace ON_NINETEEN_TO_THIRTY_SIX{.band = ClothBand::EvenChances,
                                               .across = 5};

}  // namespace rtype::client::testing
