#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ByteSequence.hpp"
#include "EnemyAction.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"

namespace rtype::network {

using testing::byteSequence;

/**
 * Given entity 9, attack 2, phase Strike
 * When the EnemyAction is encoded
 * Then the payload is 00 00 00 09 02 02, the bytes the protocol page gives
 */
TEST(EnemyAction, EncodesTheIdTheAttackAndThePhaseInThatOrder) {
  const auto expected = byteSequence({0x00, 0x00, 0x00, 0x09, 0x02, 0x02});

  EXPECT_EQ(encodeMessage(EnemyAction{
                .entityId = 9, .attack = 2, .phase = EnemyActionPhase::Strike}),
            expected);
}

/**
 * Given an EnemyAction of each of the four phases
 * When it is encoded then decoded
 * Then the decoded message equals the original
 */
TEST(EnemyAction, DecodesWhatItEncodedForEveryPhase) {
  for (const EnemyActionPhase phase :
       {EnemyActionPhase::Signal, EnemyActionPhase::Strike,
        EnemyActionPhase::Recover, EnemyActionPhase::Enraged}) {
    SCOPED_TRACE(static_cast<int>(phase));
    const EnemyAction action{.entityId = 9, .attack = 1, .phase = phase};

    EXPECT_EQ(decodeMessage<EnemyAction>(encodeMessage(action)), action);
  }
}

/**
 * Given the phase bytes 0 and 5, just outside the phases 1 to 4
 * When each payload is decoded
 * Then an InvalidFieldValueException is raised
 */
TEST(EnemyAction, RefusesAPhaseOutsideTheList) {
  for (const unsigned int phase : {0U, 5U}) {
    SCOPED_TRACE(phase);

    EXPECT_THROW(static_cast<void>(decodeMessage<EnemyAction>(
                     byteSequence({0x00, 0x00, 0x00, 0x09, 0x02, phase}))),
                 InvalidFieldValueException);
  }
}

/**
 * Given the entity id 0, which no entity has
 * When an EnemyAction naming it is decoded, then encoded
 * Then an InvalidFieldValueException is raised each time
 */
TEST(EnemyAction, RefusesTheNoEntityId) {
  EXPECT_THROW(static_cast<void>(decodeMessage<EnemyAction>(
                   byteSequence({0x00, 0x00, 0x00, 0x00, 0x02, 0x02}))),
               InvalidFieldValueException);
  EXPECT_THROW(
      static_cast<void>(encodeMessage(EnemyAction{
          .entityId = 0, .attack = 2, .phase = EnemyActionPhase::Strike})),
      InvalidFieldValueException);
}

/**
 * Given a payload of five bytes
 * When it is decoded
 * Then a BufferUnderflowException is raised
 */
TEST(EnemyAction, RefusesAPayloadThatIsCutShort) {
  EXPECT_THROW(static_cast<void>(decodeMessage<EnemyAction>(
                   byteSequence({0x00, 0x00, 0x00, 0x09, 0x02}))),
               BufferUnderflowException);
}

/**
 * Given a payload of seven bytes
 * When it is decoded
 * Then a TrailingBytesException is raised
 */
TEST(EnemyAction, RefusesBytesAfterThePhase) {
  EXPECT_THROW(static_cast<void>(decodeMessage<EnemyAction>(
                   byteSequence({0x00, 0x00, 0x00, 0x09, 0x02, 0x02, 0x00}))),
               TrailingBytesException);
}

/**
 * Given the attack numbers 0, 7 and 255
 * When an EnemyAction of each is encoded then decoded
 * Then the attack is kept: the protocol does not list the attacks
 */
TEST(EnemyAction, KeepsAnyAttackNumber) {
  for (const std::uint8_t attack : {0, 7, 255}) {
    SCOPED_TRACE(static_cast<int>(attack));
    const EnemyAction action{
        .entityId = 9, .attack = attack, .phase = EnemyActionPhase::Signal};

    EXPECT_EQ(decodeMessage<EnemyAction>(encodeMessage(action)), action);
  }
}

}  // namespace rtype::network
