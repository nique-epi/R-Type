#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "CompressionConstants.hpp"
#include "CompressionFixtures.hpp"
#include "CompressionTestConstants.hpp"
#include "HuffmanCodec.hpp"
#include "HuffmanCodecFixtures.hpp"
#include "HuffmanCodecTestConstants.hpp"
#include "RealWorldStateBodiesTestConstants.hpp"
#include "SerializationConstants.hpp"

using rtype::network::BITS_PER_BYTE;
using rtype::network::huffmanDecode;
using rtype::network::huffmanEncode;
using rtype::network::MAXIMUM_BODY_SIZE;
using rtype::network::testing::bodyCodedWithPadding;
using rtype::network::testing::BYTES_SEED;
using rtype::network::testing::bytesOf;
using rtype::network::testing::everyByteValue;
using rtype::network::testing::LARGE_WORLD_STATE_BODY;
using rtype::network::testing::LONGEST_RANDOM_STRING;
using rtype::network::testing::MEDIUM_WORLD_STATE_BODY;
using rtype::network::testing::ONE_BYTE_OVER_THE_LARGEST_BODY;
using rtype::network::testing::RANDOM_STRING_COUNT;
using rtype::network::testing::randomBytes;
using rtype::network::testing::SMALL_DECODE_LIMIT;
using rtype::network::testing::SMALL_WORLD_STATE_BODY;

/**
 * Given an empty body
 * When it is coded and decoded
 * Then the decoder gives back an empty body
 */
TEST(HuffmanCodec, RoundTripsAnEmptyBody) {
  const std::vector<std::byte> body;

  const auto decoded = huffmanDecode(huffmanEncode(body), MAXIMUM_BODY_SIZE);

  ASSERT_TRUE(decoded.has_value());
  EXPECT_EQ(decoded.value_or(std::vector<std::byte>{1, std::byte{1}}), body);
}

/**
 * Given a body that holds each of the 256 byte values
 * When it is coded and decoded
 * Then the decoder gives back the same bytes in the same order
 */
TEST(HuffmanCodec, RoundTripsEveryByteValue) {
  const std::vector<std::byte> body = everyByteValue();

  const auto decoded = huffmanDecode(huffmanEncode(body), MAXIMUM_BODY_SIZE);

  ASSERT_TRUE(decoded.has_value());
  EXPECT_EQ(decoded.value_or(std::vector<std::byte>{}), body);
}

/**
 * Given a random body of exactly the largest size a datagram holds
 * When it is coded and decoded with that size as the limit
 * Then the decoder gives back the same bytes
 */
TEST(HuffmanCodec, RoundTripsABodyOfTheLargestSize) {
  const std::vector<std::byte> body =
      randomBytes(MAXIMUM_BODY_SIZE, BYTES_SEED);

  const auto decoded = huffmanDecode(huffmanEncode(body), MAXIMUM_BODY_SIZE);

  ASSERT_TRUE(decoded.has_value());
  EXPECT_EQ(decoded.value_or(std::vector<std::byte>{}), body);
}

/**
 * Given the bodies of three WorldState datagrams the real server sent
 * When each is coded and decoded
 * Then the decoder gives back each one unchanged
 */
TEST(HuffmanCodec, RoundTripsRealWorldStateBodies) {
  const std::vector<std::vector<std::byte>> bodies{
      bytesOf(SMALL_WORLD_STATE_BODY), bytesOf(MEDIUM_WORLD_STATE_BODY),
      bytesOf(LARGE_WORLD_STATE_BODY)};

  for (const std::vector<std::byte>& body : bodies) {
    const auto decoded = huffmanDecode(huffmanEncode(body), MAXIMUM_BODY_SIZE);
    ASSERT_TRUE(decoded.has_value());
    EXPECT_EQ(decoded.value_or(std::vector<std::byte>{}), body);
  }
}

/**
 * Given the bodies of three WorldState datagrams the real server sent
 * When each is coded
 * Then the coded body is shorter than the original
 */
TEST(HuffmanCodec, ShrinksRealWorldStateBodies) {
  const std::vector<std::vector<std::byte>> bodies{
      bytesOf(SMALL_WORLD_STATE_BODY), bytesOf(MEDIUM_WORLD_STATE_BODY),
      bytesOf(LARGE_WORLD_STATE_BODY)};

  for (const std::vector<std::byte>& body : bodies) {
    EXPECT_LT(huffmanEncode(body).size(), body.size());
  }
}

/**
 * Given a coded body of one byte more than a datagram body holds
 * When it is decoded with the largest body size as the limit
 * Then the decoder refuses it
 */
TEST(HuffmanCodec, RefusesABodyOneByteOverTheLimit) {
  const auto coded =
      huffmanEncode(randomBytes(ONE_BYTE_OVER_THE_LARGEST_BODY, BYTES_SEED));

  EXPECT_FALSE(huffmanDecode(coded, MAXIMUM_BODY_SIZE).has_value());
}

/**
 * Given no bytes at all
 * When they are decoded
 * Then the decoder refuses them
 */
TEST(HuffmanCodec, RefusesAnEmptyStream) {
  EXPECT_FALSE(huffmanDecode({}, MAXIMUM_BODY_SIZE).has_value());
}

/**
 * Given the coded form of a real body with its last byte cut off
 * When it is decoded
 * Then the decoder refuses it, since the end symbol is missing
 */
TEST(HuffmanCodec, RefusesAStreamCutBeforeTheEndSymbol) {
  std::vector<std::byte> coded = huffmanEncode(bytesOf(SMALL_WORLD_STATE_BODY));
  coded.pop_back();

  EXPECT_FALSE(huffmanDecode(coded, MAXIMUM_BODY_SIZE).has_value());
}

/**
 * Given a coded body whose last byte holds padding bits
 * When the last padding bit is set to 1
 * Then the decoder refuses it
 */
TEST(HuffmanCodec, RefusesASetPaddingBit) {
  std::vector<std::byte> coded = huffmanEncode(bodyCodedWithPadding());
  coded.back() |= std::byte{1};

  EXPECT_FALSE(huffmanDecode(coded, MAXIMUM_BODY_SIZE).has_value());
}

/**
 * Given the coded form of a real body
 * When a whole byte of zeros follows the end symbol
 * Then the decoder refuses it
 */
TEST(HuffmanCodec, RefusesAWholeByteAfterTheEndSymbol) {
  std::vector<std::byte> coded = huffmanEncode(bytesOf(SMALL_WORLD_STATE_BODY));
  coded.push_back(std::byte{0});

  EXPECT_FALSE(huffmanDecode(coded, MAXIMUM_BODY_SIZE).has_value());
}

/**
 * Given the coded form of a real body
 * When each of its bits is flipped in turn and the result decoded
 * Then a stream that decodes is exactly the coded form of the body it gives, so
 *      two different byte strings never give the same body
 */
TEST(HuffmanCodec, DecodesOnlyWhatItWouldEncodeAgain) {
  const std::vector<std::byte> coded =
      huffmanEncode(bytesOf(SMALL_WORLD_STATE_BODY));

  for (std::size_t byteIndex = 0; byteIndex < coded.size(); ++byteIndex) {
    for (std::size_t bit = 0; bit < BITS_PER_BYTE; ++bit) {
      std::vector<std::byte> mutated = coded;
      mutated[byteIndex] ^= static_cast<std::byte>(1U << bit);
      const auto decoded = huffmanDecode(mutated, MAXIMUM_BODY_SIZE);
      if (decoded.has_value()) {
        EXPECT_EQ(huffmanEncode(decoded.value_or(std::vector<std::byte>{})),
                  mutated);
      }
    }
  }
}

/**
 * Given thousands of random byte strings
 * When each is decoded with a limit of a few bytes
 * Then no result is longer than that limit
 */
TEST(HuffmanCodec, NeverProducesMoreThanTheLimitFromRandomBytes) {
  for (std::size_t count = 0; count < RANDOM_STRING_COUNT; ++count) {
    const std::size_t size = count % LONGEST_RANDOM_STRING;
    const auto decoded = huffmanDecode(
        randomBytes(size, BYTES_SEED + static_cast<std::uint32_t>(count)),
        SMALL_DECODE_LIMIT);
    if (decoded.has_value()) {
      EXPECT_LE(decoded.value_or(std::vector<std::byte>{}).size(),
                SMALL_DECODE_LIMIT);
    }
  }
}
