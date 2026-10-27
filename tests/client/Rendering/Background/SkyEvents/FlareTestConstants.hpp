#pragma once

#include <cstddef>
#include "BackgroundConstants.hpp"

/** @brief A time of the sky event clock in the middle of any flare. */
constexpr float MIDDLE_OF_ANY_FLARE = 1.0F;

/** @brief Longest flare arm, FLARE_LONGEST_ARM rounded to whole pixels, and the
 * pixels of a full flare: its four arms, its diagonals and its core. */
constexpr std::size_t FULL_ARM_LENGTH = 3;
constexpr std::size_t PIXELS_OF_A_FULL_FLARE =
    (rtype::client::CROSS_DIRECTIONS.size() * FULL_ARM_LENGTH) +
    rtype::client::DIAGONAL_DIRECTIONS.size() + 1;
