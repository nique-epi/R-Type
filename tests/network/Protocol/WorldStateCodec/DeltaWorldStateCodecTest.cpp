#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ByteSequence.hpp"
#include "DeltaWorldState.hpp"
#include "DeltaWorldStateCodec.hpp"
#include "DeltaWorldStateCodecTestConstants.hpp"
#include "DeltaWorldStateFixtures.hpp"
#include "NetworkException.hpp"
#include "ProtocolConstants.hpp"

using rtype::network::BufferUnderflowException;
using rtype::network::DeltaWorldState;
using rtype::network::DeltaWorldStateCodec;
using rtype::network::InvalidFieldValueException;
using rtype::network::TooManyDeltaPartsException;
using rtype::network::TrailingBytesException;
using rtype::network::testing::BASE_DISTANCE_BEYOND_THE_LARGEST;
using rtype::network::testing::BASE_DISTANCE_OFFSET;
using rtype::network::testing::byteSequence;
using rtype::network::testing::CHANGED_FILLING_TWO_PARTS;
using rtype::network::testing::CHANGED_FITTING_WITH_FOUR_PLAYERS;
using rtype::network::testing::CHANGED_LEFT_FOR_THE_SECOND_PART;
using rtype::network::testing::CHANGED_NEEDING_TOO_MANY_PARTS;
using rtype::network::testing::deltaOfLists;
using rtype::network::testing::LARGEST_BASE_DISTANCE;
using rtype::network::testing::LARGEST_LIST_COUNT;
using rtype::network::testing::LARGEST_PART_COUNT;
using rtype::network::testing::NO_PART_COUNT;
using rtype::network::testing::oneShipDelta;
using rtype::network::testing::PART_COUNT_BEYOND_THE_LARGEST;
using rtype::network::testing::PART_COUNT_OFFSET;
using rtype::network::testing::PART_INDEX_OFFSET;
using rtype::network::testing::REMOVED_ID_BYTES;
using rtype::network::testing::REMOVED_IDS_LEFT_FOR_THE_SECOND_PART;
using rtype::network::testing::REMOVED_IDS_OVERFLOWING_A_LIST;
using rtype::network::testing::TWO_PARTS;
using rtype::network::testing::withByteAt;

/**
 * Given a delta of one player, one changed entity and one removed id
 * When it is encoded
 * Then the payload is tick, baseDistance, partIndex, partCount, the players,
 * the changed entities and the removed ids, each list after its count
 */
TEST(DeltaWorldStateCodec, EncodesTheLayoutOfTheProtocolPage) {
  const auto payloads = DeltaWorldStateCodec::encode(oneShipDelta());

  ASSERT_EQ(payloads.size(), 1U);
  EXPECT_EQ(
      payloads.front(),
      byteSequence({0x00, 0x00, 0x00, 0x03, 0x02, 0x00, 0x01, 0x01, 0x01, 0x00,
                    0x00, 0x00, 0x07, 0x03, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x09, 0x01,
                    0x03, 0x20, 0x01, 0x94, 0x01, 0x00, 0x00, 0x00, 0x05}));
}

/**
 * Given a delta that fits one datagram
 * When it is encoded then decoded
 * Then the decoded part equals the original
 */
TEST(DeltaWorldStateCodec, DecodesWhatItEncoded) {
  const DeltaWorldState delta = oneShipDelta();

  const auto payloads = DeltaWorldStateCodec::encode(delta);

  ASSERT_EQ(payloads.size(), 1U);
  EXPECT_EQ(DeltaWorldStateCodec::decode(payloads.front()), delta);
}

/**
 * Given four players and more changed entities than one datagram holds
 * When the delta is encoded
 * Then two parts of the same tick are produced, the first full, and their
 * changed entities put end to end are the original ones
 */
TEST(DeltaWorldStateCodec, SplitsChangedEntitiesThatDoNotFitOnePart) {
  const DeltaWorldState delta = deltaOfLists(CHANGED_FILLING_TWO_PARTS, 0);

  const auto payloads = DeltaWorldStateCodec::encode(delta);

  ASSERT_EQ(payloads.size(), 2U);
  const DeltaWorldState first = DeltaWorldStateCodec::decode(payloads.at(0));
  const DeltaWorldState second = DeltaWorldStateCodec::decode(payloads.at(1));
  EXPECT_EQ(first.changed.size(), CHANGED_FITTING_WITH_FOUR_PLAYERS);
  EXPECT_EQ(second.changed.size(), CHANGED_LEFT_FOR_THE_SECOND_PART);
  EXPECT_EQ(first.partCount, TWO_PARTS);
  EXPECT_EQ(second.partIndex, 1U);
  EXPECT_EQ(second.tick, delta.tick);
  EXPECT_EQ(second.players, delta.players);
}

/**
 * Given more removed ids than the count of one list holds
 * When the delta is encoded
 * Then the first part holds 255 ids and the second the others
 */
TEST(DeltaWorldStateCodec, CapsAListAtWhatItsCountHolds) {
  const DeltaWorldState delta = deltaOfLists(0, REMOVED_IDS_OVERFLOWING_A_LIST);

  const auto payloads = DeltaWorldStateCodec::encode(delta);

  ASSERT_EQ(payloads.size(), 2U);
  EXPECT_EQ(DeltaWorldStateCodec::decode(payloads.at(0)).removed.size(),
            LARGEST_LIST_COUNT);
  EXPECT_EQ(DeltaWorldStateCodec::decode(payloads.at(1)).removed.size(),
            REMOVED_IDS_LEFT_FOR_THE_SECOND_PART);
}

/**
 * Given a delta that needs one datagram more than the largest count of parts
 * When it is encoded
 * Then TooManyDeltaPartsException is thrown
 */
TEST(DeltaWorldStateCodec, RefusesAStateNeedingTooManyParts) {
  const DeltaWorldState delta = deltaOfLists(CHANGED_NEEDING_TOO_MANY_PARTS, 0);

  EXPECT_THROW(static_cast<void>(DeltaWorldStateCodec::encode(delta)),
               TooManyDeltaPartsException);
}

/**
 * Given a payload whose baseDistance is the largest the protocol allows
 * When it is decoded
 * Then the part is read with that distance
 */
TEST(DeltaWorldStateCodec, AcceptsTheLargestReferenceDistance) {
  const auto payload =
      withByteAt(DeltaWorldStateCodec::encode(oneShipDelta()).front(),
                 BASE_DISTANCE_OFFSET, LARGEST_BASE_DISTANCE);

  EXPECT_EQ(DeltaWorldStateCodec::decode(payload).baseDistance,
            LARGEST_BASE_DISTANCE);
}

/**
 * Given a payload whose baseDistance is one above the largest
 * When it is decoded
 * Then InvalidFieldValueException is thrown
 */
TEST(DeltaWorldStateCodec, RefusesAReferenceBeyondTheLargestDistance) {
  const auto payload =
      withByteAt(DeltaWorldStateCodec::encode(oneShipDelta()).front(),
                 BASE_DISTANCE_OFFSET, BASE_DISTANCE_BEYOND_THE_LARGEST);

  EXPECT_THROW(static_cast<void>(DeltaWorldStateCodec::decode(payload)),
               InvalidFieldValueException);
}

/**
 * Given payloads whose partCount is 0 and one above the largest
 * When each is decoded
 * Then InvalidFieldValueException is thrown for both
 */
TEST(DeltaWorldStateCodec, RefusesAPartCountOutsideOneToTheLargest) {
  const auto valid = DeltaWorldStateCodec::encode(oneShipDelta()).front();

  EXPECT_THROW(static_cast<void>(DeltaWorldStateCodec::decode(
                   withByteAt(valid, PART_COUNT_OFFSET, NO_PART_COUNT))),
               InvalidFieldValueException);
  EXPECT_THROW(static_cast<void>(DeltaWorldStateCodec::decode(withByteAt(
                   valid, PART_COUNT_OFFSET, PART_COUNT_BEYOND_THE_LARGEST))),
               InvalidFieldValueException);
}

/**
 * Given a payload of the largest part count whose partIndex is the last part
 * When it is decoded
 * Then it is accepted
 */
TEST(DeltaWorldStateCodec, AcceptsTheLastPartOfTheLargestCount) {
  const auto valid = DeltaWorldStateCodec::encode(oneShipDelta()).front();
  const auto payload =
      withByteAt(withByteAt(valid, PART_COUNT_OFFSET, LARGEST_PART_COUNT),
                 PART_INDEX_OFFSET, LARGEST_PART_COUNT - 1);

  EXPECT_EQ(DeltaWorldStateCodec::decode(payload).partIndex,
            LARGEST_PART_COUNT - 1);
}

/**
 * Given a payload whose partIndex equals its partCount
 * When it is decoded
 * Then InvalidFieldValueException is thrown
 */
TEST(DeltaWorldStateCodec, RefusesAPartIndexNotBelowThePartCount) {
  const auto payload =
      withByteAt(DeltaWorldStateCodec::encode(oneShipDelta()).front(),
                 PART_INDEX_OFFSET, 1);

  EXPECT_THROW(static_cast<void>(DeltaWorldStateCodec::decode(payload)),
               InvalidFieldValueException);
}

/**
 * Given a payload whose last removed id is 0
 * When it is decoded
 * Then InvalidFieldValueException is thrown
 */
TEST(DeltaWorldStateCodec, RefusesARemovedIdOfZero) {
  auto payload = DeltaWorldStateCodec::encode(oneShipDelta()).front();
  for (std::size_t index = 1; index <= REMOVED_ID_BYTES; ++index) {
    payload = withByteAt(payload, payload.size() - index, 0);
  }

  EXPECT_THROW(static_cast<void>(DeltaWorldStateCodec::decode(payload)),
               InvalidFieldValueException);
}

/**
 * Given a payload followed by one more byte
 * When it is decoded
 * Then TrailingBytesException is thrown
 */
TEST(DeltaWorldStateCodec, RefusesBytesAfterTheLastRemovedId) {
  auto payload = DeltaWorldStateCodec::encode(oneShipDelta()).front();
  payload.push_back(std::byte{0});

  EXPECT_THROW(static_cast<void>(DeltaWorldStateCodec::decode(payload)),
               TrailingBytesException);
}

/**
 * Given a payload cut one byte short
 * When it is decoded
 * Then BufferUnderflowException is thrown
 */
TEST(DeltaWorldStateCodec, RefusesAPayloadCutShort) {
  auto payload = DeltaWorldStateCodec::encode(oneShipDelta()).front();
  payload.pop_back();

  EXPECT_THROW(static_cast<void>(DeltaWorldStateCodec::decode(payload)),
               BufferUnderflowException);
}
