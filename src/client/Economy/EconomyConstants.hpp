#pragma once

#include <cstddef>

namespace rtype::client {

/** @brief How an amount is written: its digits in groups of
 * KOPECKS_DIGIT_GROUP from the right, KOPECKS_GROUP_SEPARATOR between two. */
constexpr std::size_t KOPECKS_DIGIT_GROUP = 3;
constexpr char KOPECKS_GROUP_SEPARATOR = ' ';

}  // namespace rtype::client
