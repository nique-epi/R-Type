#pragma once

#include <cstddef>
#include <initializer_list>
#include <vector>

namespace rtype::network::testing {

/**
 * @brief Builds a byte buffer from literal values, so a test can state the
 * exact bytes the wire format requires.
 */
std::vector<std::byte> byteSequence(std::initializer_list<unsigned int> values);

}  // namespace rtype::network::testing
