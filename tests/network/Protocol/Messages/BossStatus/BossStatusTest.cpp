#include <gtest/gtest.h>
#include "BossStatus.hpp"
#include "BossStatusTestConstants.hpp"
#include "ByteSequence.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"

namespace rtype::network::testing {

/**
 * Given boss 9 fighting with 300 of its 600 points
 * When its status is encoded
 * Then the payload is the id, the health, the full health, then the stage 2
 */
TEST(BossStatus, EncodesTheIdThenTheHealthsThenTheStage) {
  EXPECT_EQ(
      encodeMessage(BossStatus{.bossEntityId = BOSS_ENTITY_ID,
                               .health = BOSS_HALF_HEALTH,
                               .fullHealth = BOSS_FULL_HEALTH,
                               .stage = BossStage::Fighting}),
      byteSequence({0x00, 0x00, 0x00, 0x09, 0x01, 0x2C, 0x02, 0x58, 0x02}));
}

/**
 * Given a status of each stage: arriving, fighting, enraged and defeated
 * When each is encoded then decoded
 * Then each decoded status equals the original
 */
TEST(BossStatus, DecodesWhatItEncodedForEveryStage) {
  const BossStatus arriving{.stage = BossStage::Arriving};
  const BossStatus fighting{.bossEntityId = BOSS_ENTITY_ID,
                            .health = BOSS_FULL_HEALTH,
                            .fullHealth = BOSS_FULL_HEALTH,
                            .stage = BossStage::Fighting};
  const BossStatus enraged{.bossEntityId = BOSS_ENTITY_ID,
                           .health = BOSS_HALF_HEALTH,
                           .fullHealth = BOSS_FULL_HEALTH,
                           .stage = BossStage::Enraged};
  const BossStatus defeated{.bossEntityId = BOSS_ENTITY_ID,
                            .health = 0,
                            .fullHealth = BOSS_FULL_HEALTH,
                            .stage = BossStage::Defeated};

  EXPECT_EQ(decodeMessage<BossStatus>(encodeMessage(arriving)), arriving);
  EXPECT_EQ(decodeMessage<BossStatus>(encodeMessage(fighting)), fighting);
  EXPECT_EQ(decodeMessage<BossStatus>(encodeMessage(enraged)), enraged);
  EXPECT_EQ(decodeMessage<BossStatus>(encodeMessage(defeated)), defeated);
}

/**
 * Given a payload whose stage byte is 5, one above Defeated
 * When it is decoded
 * Then it is refused
 */
TEST(BossStatus, RefusesTheFirstStageByteAboveTheLastStage) {
  EXPECT_THROW(static_cast<void>(decodeMessage<BossStatus>(byteSequence(
                   {0x00, 0x00, 0x00, 0x09, 0x01, 0x2C, 0x02, 0x58, 0x05}))),
               InvalidFieldValueException);
}

/**
 * Given a payload of a boss fighting with 601 of its 600 points
 * When it is decoded
 * Then it is refused
 */
TEST(BossStatus, RefusesMoreHealthThanFull) {
  EXPECT_THROW(static_cast<void>(decodeMessage<BossStatus>(byteSequence(
                   {0x00, 0x00, 0x00, 0x09, 0x02, 0x59, 0x02, 0x58, 0x02}))),
               InvalidFieldValueException);
}

/**
 * Given the status of a boss fighting with a full health of 0
 * When it is encoded
 * Then it is refused
 */
TEST(BossStatus, RefusesToEncodeAFightWithoutFullHealth) {
  EXPECT_THROW(static_cast<void>(
                   encodeMessage(BossStatus{.bossEntityId = BOSS_ENTITY_ID,
                                            .health = 0,
                                            .fullHealth = 0,
                                            .stage = BossStage::Fighting})),
               InvalidFieldValueException);
}

/**
 * Given a payload of a boss arriving that names entity 9
 * When it is decoded
 * Then it is refused: an arriving boss is not in the world yet
 */
TEST(BossStatus, RefusesAnArrivalThatNamesAnEntity) {
  EXPECT_THROW(static_cast<void>(decodeMessage<BossStatus>(byteSequence(
                   {0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x01}))),
               InvalidFieldValueException);
}

/**
 * Given a payload with one byte after the stage
 * When it is decoded
 * Then it is refused
 */
TEST(BossStatus, RefusesAByteAfterTheStage) {
  EXPECT_THROW(
      static_cast<void>(decodeMessage<BossStatus>(byteSequence(
          {0x00, 0x00, 0x00, 0x09, 0x01, 0x2C, 0x02, 0x58, 0x02, 0x00}))),
      TrailingBytesException);
}

/**
 * Given a payload of a boss fighting that names no entity
 * When it is decoded
 * Then it is refused
 */
TEST(BossStatus, RefusesAFightWithoutAnEntity) {
  EXPECT_THROW(static_cast<void>(decodeMessage<BossStatus>(byteSequence(
                   {0x00, 0x00, 0x00, 0x00, 0x01, 0x2C, 0x02, 0x58, 0x02}))),
               InvalidFieldValueException);
}

/**
 * Given the status of a boss defeated with one point left
 * When it is encoded
 * Then it is refused: a defeated boss has none
 */
TEST(BossStatus, RefusesADefeatWithHealthLeft) {
  EXPECT_THROW(static_cast<void>(
                   encodeMessage(BossStatus{.bossEntityId = BOSS_ENTITY_ID,
                                            .health = BOSS_LAST_POINT,
                                            .fullHealth = BOSS_FULL_HEALTH,
                                            .stage = BossStage::Defeated})),
               InvalidFieldValueException);
}

/**
 * Given a payload of a boss fighting with no point left
 * When it is decoded
 * Then it is refused: a boss with no point left is defeated
 */
TEST(BossStatus, RefusesAFightWithNoHealthLeft) {
  EXPECT_THROW(static_cast<void>(decodeMessage<BossStatus>(byteSequence(
                   {0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x02, 0x58, 0x02}))),
               InvalidFieldValueException);
}

/**
 * Given a payload of eight bytes, one short of a status
 * When it is decoded
 * Then it is refused
 */
TEST(BossStatus, RefusesAPayloadOfEightBytes) {
  EXPECT_THROW(static_cast<void>(decodeMessage<BossStatus>(byteSequence(
                   {0x00, 0x00, 0x00, 0x09, 0x01, 0x2C, 0x02, 0x58}))),
               BufferUnderflowException);
}

}  // namespace rtype::network::testing
