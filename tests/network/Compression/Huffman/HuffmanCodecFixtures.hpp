#pragma once

#include <cstddef>
#include <vector>

namespace rtype::network::testing {

/**
 * @returns The 256 values of a byte, each once, in increasing order.
 */
std::vector<std::byte> everyByteValue();

/**
 * @returns A body of zero bytes whose code does not fill its last byte, so the
 * coded body ends in padding bits.
 */
std::vector<std::byte> bodyCodedWithPadding();

}  // namespace rtype::network::testing
