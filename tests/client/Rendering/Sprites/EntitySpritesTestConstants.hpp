#pragma once

#include <cstdint>

/** @brief Network ids checked for their loop start, from 0 included. */
constexpr std::uint32_t CHECKED_NETWORK_IDS = 10000;

/** @brief Two network ids that follow each other, as the server gives two
 * Bydos of a wave. */
constexpr std::uint32_t FIRST_BYDO_ID = 1;
constexpr std::uint32_t NEXT_BYDO_ID = 2;
