#pragma once

#include <cstdint>
#include "Wallet.hpp"

/** @brief A wallet ten chips short of the largest balance, 999999. */
constexpr rtype::network::Wallet TEN_CHIPS_SHORT_OF_THE_MAXIMUM{
    .kopecks = 0, .chips = 999'989};
constexpr std::uint32_t TEN_CHIPS = 10;
constexpr std::uint32_t ELEVEN_CHIPS = 11;
constexpr std::uint64_t A_HUNDRED_CHIPS = 100;
constexpr std::uint32_t LARGEST_CHIPS = 999'999;
