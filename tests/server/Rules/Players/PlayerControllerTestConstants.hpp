#pragma once

#include <cstdint>
#include "Position.hpp"

namespace rtype::server::testing {

/** @brief Where the ship of a player appears in the controller tests. */
constexpr game::Position CONTROLLED_SHIP{.x = 200.0F, .y = 300.0F};

/** @brief The number of the input a controller test sends when the order of
 * the inputs does not matter. */
constexpr std::uint32_t ANY_INPUT_NUMBER = 7;

/**
 * @brief The numbers of three inputs of one player that press right, release
 * it and press it again: the third is 9 inputs after the first, within
 * DOUBLE_TAP_INPUTS (13), so on a ship that saw the first press it starts a
 * dash.
 */
constexpr std::uint32_t FIRST_PRESS_INPUT_NUMBER = 161;
constexpr std::uint32_t RELEASE_INPUT_NUMBER = 165;
constexpr std::uint32_t SECOND_PRESS_INPUT_NUMBER = 170;

}  // namespace rtype::server::testing
