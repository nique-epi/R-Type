#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "SerializationConstants.hpp"

namespace rtype::network {

/**
 * @brief Bytes of a SipHash key: 128 bits.
 */
constexpr std::size_t SIPHASH_KEY_SIZE = 2 * UINT64_WIRE_SIZE;

/**
 * @brief Rounds of SipHash-2-4 after each block of the message, and after the
 * last one.
 */
constexpr std::size_t SIPHASH_COMPRESSION_ROUNDS = 2;
constexpr std::size_t SIPHASH_FINALIZATION_ROUNDS = 4;

/**
 * @brief The four words the SipHash state starts from, before the key is
 * mixed in: "somepseudorandomlygeneratedbytes" in ASCII.
 */
constexpr std::uint64_t SIPHASH_INITIAL_V0 = 0x736f6d6570736575;
constexpr std::uint64_t SIPHASH_INITIAL_V1 = 0x646f72616e646f6d;
constexpr std::uint64_t SIPHASH_INITIAL_V2 = 0x6c7967656e657261;
constexpr std::uint64_t SIPHASH_INITIAL_V3 = 0x7465646279746573;

/**
 * @brief The rotations of one SipRound, in bits: the first and second ones of
 * v1 and v3, and the half rotation of v0 and v2.
 */
constexpr int SIP_ROUND_FIRST_V1_ROTATION = 13;
constexpr int SIP_ROUND_SECOND_V1_ROTATION = 17;
constexpr int SIP_ROUND_FIRST_V3_ROTATION = 16;
constexpr int SIP_ROUND_SECOND_V3_ROTATION = 21;
constexpr int SIP_ROUND_HALF_ROTATION = 32;

/**
 * @brief Where the length of the message goes in the last block: its most
 * significant byte.
 */
constexpr std::size_t SIPHASH_LENGTH_SHIFT =
    (UINT64_WIRE_SIZE - 1) * BITS_PER_BYTE;

/**
 * @brief Mixed into v2 before the finalization rounds of a 64-bit output.
 */
constexpr std::uint64_t SIPHASH_FINALIZATION_MARK = 0xff;

/**
 * @brief How long a connection cookie window lasts. A cookie is accepted in
 * the window it was issued in and the next one, so it lives one to two
 * windows. Provisional, see the protocol page.
 */
constexpr std::chrono::seconds COOKIE_WINDOW_DURATION{10};

}  // namespace rtype::network
