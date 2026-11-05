#pragma once

#include <cstddef>
#include <vector>

namespace rtype::network::testing {

/**
 * @returns The 256 values of a byte, each once, in increasing order.
 */
std::vector<std::byte> everyByteValue();

/**
 * @returns The magic number of a zstd frame followed by random bytes, so it
 * looks like a frame at its start and is none.
 */
std::vector<std::byte> frameOfGarbage();

}  // namespace rtype::network::testing
