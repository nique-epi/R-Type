#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace rtype::network::testing {

/**
 * @returns The bytes holding @p values.
 */
std::vector<std::byte> bytesOf(std::span<const std::uint8_t> values);

/**
 * @returns @p size pseudo-random bytes, always the same for one @p seed.
 */
std::vector<std::byte> randomBytes(std::size_t size, std::uint32_t seed);

}  // namespace rtype::network::testing
