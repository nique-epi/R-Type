#include <gtest/gtest.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "HandshakeTestConstants.hpp"
#include "SerializationConstants.hpp"
#include "SipHash.hpp"

namespace rtype::network {
namespace {

using testing::ONE_BLOCK;
using testing::ONE_BLOCK_AND_SEVEN_BYTES;
using testing::ONE_BYTE;
using testing::REFERENCE_HASH_OF_0_BYTES;
using testing::REFERENCE_HASH_OF_15_BYTES;
using testing::REFERENCE_HASH_OF_1_BYTE;
using testing::REFERENCE_HASH_OF_2_BYTES;
using testing::REFERENCE_HASH_OF_3_BYTES;
using testing::REFERENCE_HASH_OF_8_BYTES;
using testing::THREE_BYTES;
using testing::TWO_BYTES;

using ReferenceHash = std::array<std::uint8_t, UINT64_WIRE_SIZE>;

/** @returns The key of the reference vectors: the bytes 0 to 15. */
SipHashKey referenceKey() {
  SipHashKey key{};
  for (std::size_t i = 0; i < key.size(); ++i) {
    key.at(i) = static_cast<std::byte>(i);
  }
  return key;
}

/** @returns The message of the reference vectors: the bytes 0 to n - 1. */
std::vector<std::byte> referenceMessage(std::size_t length) {
  std::vector<std::byte> message(length);
  for (std::size_t i = 0; i < length; ++i) {
    message.at(i) = static_cast<std::byte>(i);
  }
  return message;
}

/** @returns @p hash in little-endian order, as the reference writes it. */
ReferenceHash littleEndianBytes(std::uint64_t hash) {
  ReferenceHash bytes{};
  for (std::size_t i = 0; i < bytes.size(); ++i) {
    bytes.at(i) =
        static_cast<std::uint8_t>((hash >> (i * BITS_PER_BYTE)) & BYTE_MASK);
  }
  return bytes;
}

ReferenceHash hashOfReferenceMessage(std::size_t length) {
  return littleEndianBytes(sipHash(referenceKey(), referenceMessage(length)));
}

/**
 * Given the key of the reference vectors
 * When an empty message is hashed
 * Then the output is the reference output for 0 bytes
 */
TEST(SipHash, MatchesTheReferenceForAnEmptyMessage) {
  EXPECT_EQ(hashOfReferenceMessage(0), REFERENCE_HASH_OF_0_BYTES);
}

/**
 * Given the key of the reference vectors
 * When messages of 1, 2 and 3 bytes, shorter than one block, are hashed
 * Then each output is the reference output for its length
 */
TEST(SipHash, MatchesTheReferenceForMessagesShorterThanOneBlock) {
  EXPECT_EQ(hashOfReferenceMessage(ONE_BYTE), REFERENCE_HASH_OF_1_BYTE);
  EXPECT_EQ(hashOfReferenceMessage(TWO_BYTES), REFERENCE_HASH_OF_2_BYTES);
  EXPECT_EQ(hashOfReferenceMessage(THREE_BYTES), REFERENCE_HASH_OF_3_BYTES);
}

/**
 * Given the key of the reference vectors
 * When a message of exactly one eight-byte block is hashed
 * Then the output is the reference output for 8 bytes
 */
TEST(SipHash, MatchesTheReferenceForOneWholeBlock) {
  EXPECT_EQ(hashOfReferenceMessage(ONE_BLOCK), REFERENCE_HASH_OF_8_BYTES);
}

/**
 * Given the key of the reference vectors
 * When a message of one block and seven bytes is hashed
 * Then the output is the reference output for 15 bytes
 */
TEST(SipHash, MatchesTheReferenceForABlockAndAPartialOne) {
  EXPECT_EQ(hashOfReferenceMessage(ONE_BLOCK_AND_SEVEN_BYTES),
            REFERENCE_HASH_OF_15_BYTES);
}

}  // namespace
}  // namespace rtype::network
