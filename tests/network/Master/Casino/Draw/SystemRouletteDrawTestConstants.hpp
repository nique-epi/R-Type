#pragma once

#include <cstddef>

namespace rtype::network::testing {

/**
 * @brief Draws enough for every number of the wheel to come out: a number is
 * missed by 37 000 draws with a chance of (36/37)^37000, below 10^-400.
 */
constexpr std::size_t DRAWS_FOR_EVERY_NUMBER = 37000;

}  // namespace rtype::network::testing
