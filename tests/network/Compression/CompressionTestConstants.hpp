#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::network::testing {

/**
 * @brief The values of a byte, so a body holding each of them once.
 */
constexpr std::size_t BYTE_VALUE_COUNT = 256;

/**
 * @brief The largest value of a byte.
 */
constexpr std::uint32_t LARGEST_BYTE_VALUE = BYTE_VALUE_COUNT - 1;

/**
 * @brief The seed of the pseudo-random bytes of the tests, so a failure comes
 * back identical.
 */
constexpr std::uint32_t BYTES_SEED = 7;

}  // namespace rtype::network::testing
