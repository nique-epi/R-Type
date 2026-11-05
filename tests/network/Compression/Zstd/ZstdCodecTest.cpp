#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "CompressionConstants.hpp"
#include "CompressionFixtures.hpp"
#include "CompressionTestConstants.hpp"
#include "RealWorldStateBodiesTestConstants.hpp"
#include "SerializationConstants.hpp"
#include "ZstdCodec.hpp"
#include "ZstdCodecFixtures.hpp"
#include "ZstdCodecTestConstants.hpp"

using rtype::network::BITS_PER_BYTE;
using rtype::network::compressBody;
using rtype::network::decompressBody;
using rtype::network::MAXIMUM_BODY_SIZE;
using rtype::network::testing::BYTES_SEED;
using rtype::network::testing::bytesOf;
using rtype::network::testing::everyByteValue;
using rtype::network::testing::FRAME_DESCRIPTOR_OFFSET;
using rtype::network::testing::FRAME_OPTION_BITS;
using rtype::network::testing::frameOfGarbage;
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
 * When it is compressed and decompressed
 * Then the decompressor gives back an empty body
 */
TEST(ZstdCodec, RoundTripsAnEmptyBody) {
  const std::vector<std::byte> body;

  const auto decoded = decompressBody(
      compressBody(body).value_or(std::vector<std::byte>{}), MAXIMUM_BODY_SIZE);

  ASSERT_TRUE(decoded.has_value());
  EXPECT_EQ(decoded.value_or(std::vector<std::byte>{1, std::byte{1}}), body);
}

/**
 * Given a body that holds each of the 256 byte values
 * When it is compressed and decompressed
 * Then the decompressor gives back the same bytes in the same order
 */
TEST(ZstdCodec, RoundTripsEveryByteValue) {
  const std::vector<std::byte> body = everyByteValue();

  const auto decoded = decompressBody(
      compressBody(body).value_or(std::vector<std::byte>{}), MAXIMUM_BODY_SIZE);

  ASSERT_TRUE(decoded.has_value());
  EXPECT_EQ(decoded.value_or(std::vector<std::byte>{}), body);
}

/**
 * Given a random body of exactly the largest size a datagram holds
 * When it is compressed and decompressed with that size as the limit
 * Then the decompressor gives back the same bytes
 */
TEST(ZstdCodec, RoundTripsABodyOfTheLargestSize) {
  const std::vector<std::byte> body =
      randomBytes(MAXIMUM_BODY_SIZE, BYTES_SEED);

  const auto decoded = decompressBody(
      compressBody(body).value_or(std::vector<std::byte>{}), MAXIMUM_BODY_SIZE);

  ASSERT_TRUE(decoded.has_value());
  EXPECT_EQ(decoded.value_or(std::vector<std::byte>{}), body);
}

/**
 * Given the bodies of three WorldState datagrams the real server sent
 * When each is compressed and decompressed
 * Then the decompressor gives back each one unchanged
 */
TEST(ZstdCodec, RoundTripsRealWorldStateBodies) {
  const std::vector<std::vector<std::byte>> bodies{
      bytesOf(SMALL_WORLD_STATE_BODY), bytesOf(MEDIUM_WORLD_STATE_BODY),
      bytesOf(LARGE_WORLD_STATE_BODY)};

  for (const std::vector<std::byte>& body : bodies) {
    const auto decoded =
        decompressBody(compressBody(body).value_or(std::vector<std::byte>{}),
                       MAXIMUM_BODY_SIZE);
    ASSERT_TRUE(decoded.has_value());
    EXPECT_EQ(decoded.value_or(std::vector<std::byte>{}), body);
  }
}

/**
 * Given the bodies of three WorldState datagrams the real server sent
 * When each is compressed
 * Then the frame is shorter than the original
 */
TEST(ZstdCodec, ShrinksRealWorldStateBodies) {
  const std::vector<std::vector<std::byte>> bodies{
      bytesOf(SMALL_WORLD_STATE_BODY), bytesOf(MEDIUM_WORLD_STATE_BODY),
      bytesOf(LARGE_WORLD_STATE_BODY)};

  for (const std::vector<std::byte>& body : bodies) {
    EXPECT_LT(compressBody(body).value_or(body).size(), body.size());
  }
}

/**
 * Given the frame of a body one byte over what a datagram body holds
 * When it is decompressed with the largest body size as the limit
 * Then the decompressor refuses it
 */
TEST(ZstdCodec, RefusesAFrameOneByteOverTheLimit) {
  const auto frame =
      compressBody(randomBytes(ONE_BYTE_OVER_THE_LARGEST_BODY, BYTES_SEED));

  ASSERT_TRUE(frame.has_value());
  EXPECT_FALSE(decompressBody(frame.value_or(std::vector<std::byte>{}),
                              MAXIMUM_BODY_SIZE)
                   .has_value());
}

/**
 * Given no bytes at all
 * When they are decompressed
 * Then the decompressor refuses them
 */
TEST(ZstdCodec, RefusesAnEmptyStream) {
  EXPECT_FALSE(decompressBody({}, MAXIMUM_BODY_SIZE).has_value());
}

/**
 * Given the frame of a real body with its last byte cut off
 * When it is decompressed
 * Then the decompressor refuses it
 */
TEST(ZstdCodec, RefusesAFrameCutBeforeItsEnd) {
  std::vector<std::byte> frame = compressBody(bytesOf(SMALL_WORLD_STATE_BODY))
                                     .value_or(std::vector<std::byte>{});
  frame.pop_back();

  EXPECT_FALSE(decompressBody(frame, MAXIMUM_BODY_SIZE).has_value());
}

/**
 * Given the frame of a real body
 * When a byte follows its end
 * Then the decompressor refuses it
 */
TEST(ZstdCodec, RefusesAByteAfterTheFrame) {
  std::vector<std::byte> frame = compressBody(bytesOf(SMALL_WORLD_STATE_BODY))
                                     .value_or(std::vector<std::byte>{});
  frame.push_back(std::byte{0});

  EXPECT_FALSE(decompressBody(frame, MAXIMUM_BODY_SIZE).has_value());
}

/**
 * Given the frame of a real body followed by a second whole frame
 * When it is decompressed
 * Then the decompressor refuses it, since a body is exactly one frame
 */
TEST(ZstdCodec, RefusesASecondFrameAfterTheFirst) {
  const std::vector<std::byte> frame =
      compressBody(bytesOf(SMALL_WORLD_STATE_BODY))
          .value_or(std::vector<std::byte>{});
  std::vector<std::byte> twoFrames = frame;
  twoFrames.insert(twoFrames.end(), frame.begin(), frame.end());

  EXPECT_FALSE(decompressBody(twoFrames, MAXIMUM_BODY_SIZE).has_value());
}

/**
 * Given the magic number of a zstd frame followed by random bytes
 * When it is decompressed
 * Then the decompressor refuses it
 */
TEST(ZstdCodec, RefusesGarbageAfterTheMagicNumber) {
  EXPECT_FALSE(decompressBody(frameOfGarbage(), MAXIMUM_BODY_SIZE).has_value());
}

/**
 * Given the frame of a real body
 * When each of its bits is flipped in turn and the result decompressed
 * Then no result is longer than the limit, and nothing is thrown
 */
TEST(ZstdCodec, NeverProducesMoreThanTheLimitFromACorruptFrame) {
  const std::vector<std::byte> frame =
      compressBody(bytesOf(SMALL_WORLD_STATE_BODY))
          .value_or(std::vector<std::byte>{});

  for (std::size_t byteIndex = 0; byteIndex < frame.size(); ++byteIndex) {
    for (std::size_t bit = 0; bit < BITS_PER_BYTE; ++bit) {
      std::vector<std::byte> mutated = frame;
      mutated[byteIndex] ^= static_cast<std::byte>(1U << bit);
      const auto decoded = decompressBody(mutated, SMALL_DECODE_LIMIT);
      if (decoded.has_value()) {
        EXPECT_LE(decoded.value_or(std::vector<std::byte>{}).size(),
                  SMALL_DECODE_LIMIT);
      }
    }
  }
}

/**
 * Given thousands of random byte strings
 * When each is decompressed with a limit of a few bytes
 * Then no result is longer than that limit
 */
TEST(ZstdCodec, NeverProducesMoreThanTheLimitFromRandomBytes) {
  for (std::size_t count = 0; count < RANDOM_STRING_COUNT; ++count) {
    const std::size_t size = count % LONGEST_RANDOM_STRING;
    const auto decoded = decompressBody(
        randomBytes(size, BYTES_SEED + static_cast<std::uint32_t>(count)),
        SMALL_DECODE_LIMIT);
    if (decoded.has_value()) {
      EXPECT_LE(decoded.value_or(std::vector<std::byte>{}).size(),
                SMALL_DECODE_LIMIT);
    }
  }
}

/**
 * Given the frame of a real body
 * When its frame header descriptor is read
 * Then the frame names no dictionary and carries no checksum and no content
 *      size, since both programs hold the dictionary and the datagram has its
 *      own size and checksum
 */
TEST(ZstdCodec, FramesCarryNoDictionaryIdChecksumOrContentSize) {
  const std::vector<std::byte> frame =
      compressBody(bytesOf(SMALL_WORLD_STATE_BODY))
          .value_or(std::vector<std::byte>{});

  ASSERT_GT(frame.size(), FRAME_DESCRIPTOR_OFFSET);
  EXPECT_EQ(frame[FRAME_DESCRIPTOR_OFFSET] & std::byte{FRAME_OPTION_BITS},
            std::byte{0});
}
