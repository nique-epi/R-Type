#include <gtest/gtest.h>
#include "ByteSequence.hpp"
#include "EnemyAction.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "ProtocolTestConstants.hpp"
#include "WorldStateFixtures.hpp"

namespace rtype::network {

using testing::byteSequence;
using testing::EXAMPLE_ENTITY_ID;
using testing::SOME_ATTACK_NUMBER;

/**
 * Given entity 9 entering the strike of attack 3
 * When its EnemyAction is encoded
 * Then the payload is the id, the attack 3, then the phase 2
 */
TEST(EnemyAction, EncodesTheIdThenTheAttackThenThePhase) {
  const auto expected = byteSequence({0x00, 0x00, 0x00, 0x09, 0x03, 0x02});

  EXPECT_EQ(encodeMessage(EnemyAction{.entityId = EXAMPLE_ENTITY_ID,
                                      .attack = SOME_ATTACK_NUMBER,
                                      .phase = EnemyActionPhase::Strike}),
            expected);
}

/**
 * Given the phases 1 to 4
 * When each payload is decoded
 * Then they give Signal, Strike, Recover and Enraged
 */
TEST(EnemyAction, DecodesEveryPhaseOfTheProtocolPage) {
  const auto signal = byteSequence({0x00, 0x00, 0x00, 0x09, 0x03, 0x01});
  const auto strike = byteSequence({0x00, 0x00, 0x00, 0x09, 0x03, 0x02});
  const auto recover = byteSequence({0x00, 0x00, 0x00, 0x09, 0x03, 0x03});
  const auto enraged = byteSequence({0x00, 0x00, 0x00, 0x09, 0x03, 0x04});

  EXPECT_EQ(decodeMessage<EnemyAction>(signal).phase, EnemyActionPhase::Signal);
  EXPECT_EQ(decodeMessage<EnemyAction>(strike).phase, EnemyActionPhase::Strike);
  EXPECT_EQ(decodeMessage<EnemyAction>(recover).phase,
            EnemyActionPhase::Recover);
  EXPECT_EQ(decodeMessage<EnemyAction>(enraged).phase,
            EnemyActionPhase::Enraged);
}

/**
 * Given the attack 3 of entity 9 in a decoded payload
 * When the payload is decoded
 * Then the attack is 3, whatever the phase
 */
TEST(EnemyAction, DecodesTheAttackNumber) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x09, 0x03, 0x01});

  EXPECT_EQ(decodeMessage<EnemyAction>(bytes).attack, SOME_ATTACK_NUMBER);
}

/**
 * Given the phases 0 and 5, just outside the phases of the protocol page
 * When each payload is decoded
 * Then each is refused
 */
TEST(EnemyAction, RefusesToReadAPhaseOutsideTheList) {
  const auto belowTheFirst = byteSequence({0x00, 0x00, 0x00, 0x09, 0x03, 0x00});
  const auto aboveTheLast = byteSequence({0x00, 0x00, 0x00, 0x09, 0x03, 0x05});

  EXPECT_THROW((void)decodeMessage<EnemyAction>(belowTheFirst),
               InvalidFieldValueException);
  EXPECT_THROW((void)decodeMessage<EnemyAction>(aboveTheLast),
               InvalidFieldValueException);
}

/**
 * Given an EnemyAction naming a phase outside the list
 * When it is encoded
 * Then an InvalidFieldValueException is raised
 */
TEST(EnemyAction, RefusesToWriteAPhaseOutsideTheList) {
  const EnemyAction message{.entityId = EXAMPLE_ENTITY_ID,
                            .attack = SOME_ATTACK_NUMBER,
                            .phase = static_cast<EnemyActionPhase>(0)};

  EXPECT_THROW((void)encodeMessage(message), InvalidFieldValueException);
}

/**
 * Given a payload whose entity id is 0
 * When it is decoded
 * Then an InvalidFieldValueException is raised
 */
TEST(EnemyAction, RefusesToReadTheEntityIdZero) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x00, 0x03, 0x01});

  EXPECT_THROW((void)decodeMessage<EnemyAction>(bytes),
               InvalidFieldValueException);
}

/**
 * Given a payload cut after the attack
 * When it is decoded
 * Then a BufferUnderflowException is raised
 */
TEST(EnemyAction, RefusesAPayloadWithoutItsPhase) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x09, 0x03});

  EXPECT_THROW((void)decodeMessage<EnemyAction>(bytes),
               BufferUnderflowException);
}

/**
 * Given a whole EnemyAction followed by one more byte
 * When it is decoded
 * Then a TrailingBytesException is raised
 */
TEST(EnemyAction, RefusesBytesAfterThePhase) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x09, 0x03, 0x01, 0x00});

  EXPECT_THROW((void)decodeMessage<EnemyAction>(bytes), TrailingBytesException);
}

}  // namespace rtype::network
