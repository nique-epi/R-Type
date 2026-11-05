#pragma once

#include <cstddef>
#include <cstdint>
#include "CompressionConstants.hpp"

namespace rtype::network::testing {

/**
 * @brief One byte more than a datagram body holds.
 */
constexpr std::size_t ONE_BYTE_OVER_THE_LARGEST_BODY = MAXIMUM_BODY_SIZE + 1;

/**
 * @brief How many random byte strings are decoded, and the longest of them.
 */
constexpr std::size_t RANDOM_STRING_COUNT = 2000;
constexpr std::size_t LONGEST_RANDOM_STRING = 40;

/**
 * @brief The size a decoder is allowed to produce in the test that gives it
 * random strings, so a result over it is a bug.
 */
constexpr std::size_t SMALL_DECODE_LIMIT = 8;

/**
 * @brief The most bytes of zeros tried to find a body whose code does not fill
 * its last byte.
 */
constexpr std::size_t PADDING_SEARCH_LIMIT = 16;

/**
 * @brief The weight of a complete code: the sum of 2 to the power of
 * (MAXIMUM_CODE_LENGTH - length) over its symbols equals 2 to the power of
 * MAXIMUM_CODE_LENGTH.
 */
constexpr std::uint32_t COMPLETE_CODE_WEIGHT = 1U << MAXIMUM_CODE_LENGTH;

}  // namespace rtype::network::testing
