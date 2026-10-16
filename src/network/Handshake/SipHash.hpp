#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include "HandshakeConstants.hpp"

namespace rtype::network {

/**
 * @brief The 128-bit key of SipHash, as its sixteen bytes.
 */
using SipHashKey = std::array<std::byte, SIPHASH_KEY_SIZE>;

/**
 * @returns SipHash-2-4 of @p message under @p key: the 64-bit output, as the
 * reference implementation writes it in little-endian order, read back as an
 * integer.
 *
 * SipHash is a keyed hash made for short messages: whoever does not know the
 * key cannot predict its output, even after seeing the output of other
 * messages. It is the reference implementation of Aumasson and Bernstein
 * (github.com/veorq/SipHash), with two rounds per block and four at the end.
 */
[[nodiscard]] std::uint64_t sipHash(const SipHashKey& key,
                                    std::span<const std::byte> message);

}  // namespace rtype::network
