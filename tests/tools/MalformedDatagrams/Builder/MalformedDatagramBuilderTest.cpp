#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "DatagramCodec.hpp"
#include "MalformedDatagramBuilder.hpp"
#include "MalformedDatagramBuilderFixture.hpp"
#include "MalformedDatagramBuilderTestConstants.hpp"
#include "MalformedDatagramConstants.hpp"
#include "MalformedDatagramKind.hpp"
#include "ProtocolTestConstants.hpp"

using rtype::network::DatagramCodec;
using rtype::network::testing::LARGEST_DATAGRAM_SIZE;
using rtype::tools::MALFORMED_DATAGRAM_KINDS;
using rtype::tools::MalformedDatagramBuilder;
using rtype::tools::MalformedDatagramKind;
using rtype::tools::testing::DRAWS_PER_KIND;
using rtype::tools::testing::MalformedDatagramBuilderFixture;
using rtype::tools::testing::SOME_HEADER;
using rtype::tools::testing::SOME_SEED;

/**
 * Given truncated datagrams
 * When a receiver splits each into messages
 * Then none can be split
 */
TEST_F(MalformedDatagramBuilderFixture, NoTruncatedDatagramSplits) {
  for (const std::vector<std::byte>& datagram :
       draw(MalformedDatagramKind::Truncated)) {
    EXPECT_FALSE(DatagramCodec::messageTypes(datagram).has_value());
  }
}

/**
 * Given datagrams whose message announces more bytes than follow it
 * When a receiver splits each into messages
 * Then none can be split
 */
TEST_F(MalformedDatagramBuilderFixture, NoLyingSizeSplits) {
  for (const std::vector<std::byte>& datagram :
       draw(MalformedDatagramKind::LyingSize)) {
    EXPECT_FALSE(DatagramCodec::messageTypes(datagram).has_value());
  }
}

/**
 * Given datagrams whose message holds more bytes than its type allows
 * When their messages reach the readers
 * Then the reader of its type refuses every one
 */
TEST_F(MalformedDatagramBuilderFixture, TheReaderRefusesEveryMessageTooLong) {
  for (const std::vector<std::byte>& datagram :
       draw(MalformedDatagramKind::TooLong)) {
    dispatch(datagram);
  }

  EXPECT_EQ(refusedCount(), DRAWS_PER_KIND);
}

/**
 * Given datagrams whose message holds 255 in a field the protocol bounds
 * When their messages reach the readers
 * Then the reader of its type refuses every one
 */
TEST_F(MalformedDatagramBuilderFixture, TheReaderRefusesEveryExtremeValue) {
  for (const std::vector<std::byte>& datagram :
       draw(MalformedDatagramKind::ExtremeValue)) {
    dispatch(datagram);
  }

  EXPECT_EQ(refusedCount(), DRAWS_PER_KIND);
}

/**
 * Given datagrams holding a message of a type the protocol does not list
 * When their messages reach the readers
 * Then no reader takes any of them
 */
TEST_F(MalformedDatagramBuilderFixture, NoReaderTakesAnUnknownType) {
  for (const std::vector<std::byte>& datagram :
       draw(MalformedDatagramKind::UnknownType)) {
    dispatch(datagram);
  }

  EXPECT_EQ(skippedCount(), DRAWS_PER_KIND);
}

/**
 * Given oversized datagrams
 * When their sizes are read
 * Then each holds more than the 1200 bytes a datagram may hold
 */
TEST_F(MalformedDatagramBuilderFixture, EveryOversizedDatagramPassesTheLimit) {
  for (const std::vector<std::byte>& datagram :
       draw(MalformedDatagramKind::Oversized)) {
    EXPECT_GT(datagram.size(), LARGEST_DATAGRAM_SIZE);
  }
}

/**
 * Given datagrams of random bytes
 * When their sizes are read
 * Then each holds at most the 1200 bytes a datagram may hold, so the socket
 * of a receiver hands it over
 */
TEST_F(MalformedDatagramBuilderFixture, EveryRandomDatagramFitsTheLimit) {
  for (const std::vector<std::byte>& datagram :
       draw(MalformedDatagramKind::RandomBytes)) {
    EXPECT_LE(datagram.size(), LARGEST_DATAGRAM_SIZE);
  }
}

/**
 * Given datagrams of every kind a receiver reads past the header
 * When their headers are read
 * Then each carries the header it was given, so a datagram of a session
 * reaches the readers of the receiver
 */
TEST_F(MalformedDatagramBuilderFixture, CarriesTheHeaderItIsGiven) {
  for (const MalformedDatagramKind kind :
       {MalformedDatagramKind::TooLong, MalformedDatagramKind::UnknownType,
        MalformedDatagramKind::LyingSize,
        MalformedDatagramKind::ExtremeValue}) {
    for (const std::vector<std::byte>& datagram : draw(kind)) {
      EXPECT_EQ(DatagramCodec::decodeHeader(datagram), SOME_HEADER);
    }
  }
}

/**
 * Given two builders made from the same seed
 * When each builds a datagram of every kind, in the same order
 * Then both write the same bytes
 */
TEST(MalformedDatagramBuilder, TheSameSeedWritesTheSameDatagrams) {
  MalformedDatagramBuilder first{SOME_SEED};
  MalformedDatagramBuilder second{SOME_SEED};

  for (const MalformedDatagramKind kind : MALFORMED_DATAGRAM_KINDS) {
    EXPECT_EQ(first.build(kind, SOME_HEADER), second.build(kind, SOME_HEADER));
  }
}
