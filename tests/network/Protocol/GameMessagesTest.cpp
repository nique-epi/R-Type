#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>
#include "ByteSequence.hpp"
#include "ByteWriter.hpp"
#include "EntityDestroyed.hpp"
#include "EntitySpawned.hpp"
#include "GameEvent.hpp"
#include "MessagePayload.hpp"
#include "NetworkException.hpp"
#include "PlayerInput.hpp"
#include "PlayerLeft.hpp"
#include "ProtocolConstants.hpp"
#include "ProtocolFixtures.hpp"
#include "ProtocolTestConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateCodec.hpp"
#include "WorldStateFixtures.hpp"

namespace rtype::network {

using testing::byteSequence;
using testing::EVERY_ACTION;
using testing::EXAMPLE_ENTITY_ID;
using testing::exampleBydo;
using testing::FIRST_BIT_ABOVE_THE_ACTIONS;
using testing::FIRST_SESSION_INPUT;
using testing::LARGEST_TYPE_NUMBER;
using testing::LARGEST_U16;
using testing::LARGEST_U32;
using testing::LARGEST_WIRE_COORDINATE;
using testing::LAST_PLAYER_NUMBER;
using testing::NO_TYPE_NUMBER;
using testing::ONE_REPEATED_INPUT_TOO_MANY;
using testing::oneShipState;
using testing::OWNER_AND_INPUT_BYTE_COUNT;
using testing::PLAYER_NUMBER_ABOVE_THE_LAST;
using testing::REPEATED_INPUT_LIMIT;
using testing::SECOND_PLAYER_NUMBER;
using testing::SMALLEST_WIRE_COORDINATE;
using testing::SOME_EVENT_NUMBER;
using testing::SOME_FIRING_INPUT;
using testing::SOME_INPUT_NUMBER;
using testing::UP_AND_SHOOT;

/**
 * Given input number 0x01020304 with up and shoot held
 * When the PlayerInput is encoded
 * Then the payload is the number on four bytes, the byte 11, then 0 earlier
 * actions
 */
TEST(PlayerInput, EncodesTheInputNumberThenTheActions) {
  const auto expected = byteSequence({0x01, 0x02, 0x03, 0x04, 0x11, 0x00});

  EXPECT_EQ(encodeMessage(PlayerInput{.inputNumber = SOME_INPUT_NUMBER,
                                      .actions = UP_AND_SHOOT,
                                      .earlierActions = {}}),
            expected);
}

/**
 * Given an input numbered with the largest u32, every action held
 * When it is encoded then decoded
 * Then the decoded input equals the original
 */
TEST(PlayerInput, KeepsTheLargestInputNumber) {
  const PlayerInput input{.inputNumber = LARGEST_U32,
                          .actions = EVERY_ACTION,
                          .earlierActions = {}};

  EXPECT_EQ(decodeMessage<PlayerInput>(encodeMessage(input)), input);
}

/**
 * Given an input with all five actions held
 * When it is encoded then decoded
 * Then the decoded input equals the original
 */
TEST(PlayerInput, DecodesWhatItEncoded) {
  const PlayerInput input{.inputNumber = SOME_INPUT_NUMBER,
                          .actions = EVERY_ACTION,
                          .earlierActions = {}};

  EXPECT_EQ(decodeMessage<PlayerInput>(encodeMessage(input)), input);
}

/**
 * Given an input with no key held
 * When it is decoded
 * Then it is accepted with no action
 */
TEST(PlayerInput, AcceptsAnInputWithNoKeyHeld) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x01, 0x00, 0x00});

  EXPECT_EQ(decodeMessage<PlayerInput>(bytes).actions, 0);
}

/**
 * Given the actions 20, the first bit above the five actions, and 80, the
 * last bit of the byte
 * When each is decoded
 * Then each is refused
 */
TEST(PlayerInput, RefusesToReadABitAboveTheActions) {
  const auto firstBitAbove = byteSequence({0x00, 0x00, 0x00, 0x01, 0x20, 0x00});
  const auto lastBit = byteSequence({0x00, 0x00, 0x00, 0x01, 0x80, 0x00});

  EXPECT_THROW((void)decodeMessage<PlayerInput>(firstBitAbove),
               InvalidFieldValueException);
  EXPECT_THROW((void)decodeMessage<PlayerInput>(lastBit),
               InvalidFieldValueException);
}

/**
 * Given an input with a bit above the five actions
 * When it is encoded
 * Then an InvalidFieldValueException is raised
 */
TEST(PlayerInput, RefusesToWriteABitAboveTheActions) {
  EXPECT_THROW(
      (void)encodeMessage(PlayerInput{.inputNumber = SOME_INPUT_NUMBER,
                                      .actions = FIRST_BIT_ABOVE_THE_ACTIONS,
                                      .earlierActions = {}}),
      InvalidFieldValueException);
}

/**
 * Given an input number without its actions
 * When it is decoded
 * Then a BufferUnderflowException is raised
 */
TEST(PlayerInput, RefusesAnInputCutShort) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x01});

  EXPECT_THROW((void)decodeMessage<PlayerInput>(bytes),
               BufferUnderflowException);
}

/**
 * Given a whole input, with no earlier action, followed by one more byte
 * When it is decoded
 * Then a TrailingBytesException is raised
 */
TEST(PlayerInput, RefusesBytesAfterTheEarlierActions) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<PlayerInput>(bytes), TrailingBytesException);
}

/**
 * Given input 0x01020304 holding 0x11 after two earlier inputs, 0x01 then 0x04
 * When the PlayerInput is encoded
 * Then the count 2 and the earlier actions follow, the one just before first
 */
TEST(PlayerInput, EncodesTheEarlierActionsAfterTheirCount) {
  const auto expected =
      byteSequence({0x01, 0x02, 0x03, 0x04, 0x11, 0x02, 0x01, 0x04});

  EXPECT_EQ(encodeMessage(PlayerInput{.inputNumber = SOME_INPUT_NUMBER,
                                      .actions = UP_AND_SHOOT,
                                      .earlierActions = {0x01, 0x04}}),
            expected);
}

/**
 * Given an input that repeats exactly the most earlier inputs a message may
 * When it is encoded then decoded
 * Then the decoded input equals the original
 */
TEST(PlayerInput, KeepsTheMostEarlierActionsAMessageMayHold) {
  const PlayerInput input{
      .inputNumber = SOME_INPUT_NUMBER,
      .actions = EVERY_ACTION,
      .earlierActions = std::vector<std::uint8_t>(REPEATED_INPUT_LIMIT, 0x04)};

  EXPECT_EQ(decodeMessage<PlayerInput>(encodeMessage(input)), input);
}

/**
 * Given an input with one earlier input more than a message may repeat
 * When it is encoded
 * Then an InvalidFieldValueException is raised
 */
TEST(PlayerInput, RefusesToWriteMoreEarlierActionsThanTheLimit) {
  const PlayerInput input{.inputNumber = SOME_INPUT_NUMBER,
                          .actions = 0,
                          .earlierActions = std::vector<std::uint8_t>(
                              ONE_REPEATED_INPUT_TOO_MANY, 0)};

  EXPECT_THROW((void)encodeMessage(input), InvalidFieldValueException);
}

/**
 * Given a payload announcing 16 earlier actions, one more than the limit, and
 * holding them
 * When it is decoded
 * Then an InvalidFieldValueException is raised
 */
TEST(PlayerInput, RefusesToReadMoreEarlierActionsThanTheLimit) {
  std::vector<std::byte> bytes = byteSequence({0x01, 0x02, 0x03, 0x04, 0x00});
  bytes.push_back(static_cast<std::byte>(ONE_REPEATED_INPUT_TOO_MANY));
  bytes.insert(bytes.end(), ONE_REPEATED_INPUT_TOO_MANY, std::byte{0});

  EXPECT_THROW((void)decodeMessage<PlayerInput>(bytes),
               InvalidFieldValueException);
}

/**
 * Given the first input of a session announcing one earlier action, which
 * would be numbered 0
 * When it is decoded or encoded
 * Then an InvalidFieldValueException is raised
 */
TEST(PlayerInput, RefusesEarlierActionsBelowTheFirstInput) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00});

  EXPECT_THROW((void)decodeMessage<PlayerInput>(bytes),
               InvalidFieldValueException);
  EXPECT_THROW(
      (void)encodeMessage(PlayerInput{.inputNumber = FIRST_SESSION_INPUT,
                                      .actions = 0,
                                      .earlierActions = {0}}),
      InvalidFieldValueException);
}

/**
 * Given the second input of a session announcing one earlier action, numbered
 * 1, the first input
 * When it is decoded
 * Then it is accepted with that action
 */
TEST(PlayerInput, AcceptsEarlierActionsDownToTheFirstInput) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x04});

  EXPECT_EQ(decodeMessage<PlayerInput>(bytes).earlierActions,
            std::vector<std::uint8_t>{0x04});
}

/**
 * Given a payload announcing two earlier actions but holding one
 * When it is decoded
 * Then a BufferUnderflowException is raised
 */
TEST(PlayerInput, RefusesEarlierActionsCutShort) {
  const auto bytes = byteSequence({0x01, 0x02, 0x03, 0x04, 0x00, 0x02, 0x01});

  EXPECT_THROW((void)decodeMessage<PlayerInput>(bytes),
               BufferUnderflowException);
}

/**
 * Given an earlier action with the first bit above the five actions
 * When it is decoded or encoded
 * Then an InvalidFieldValueException is raised
 */
TEST(PlayerInput, RefusesABitAboveTheActionsInAnEarlierAction) {
  const auto bytes = byteSequence({0x01, 0x02, 0x03, 0x04, 0x00, 0x01, 0x20});

  EXPECT_THROW((void)decodeMessage<PlayerInput>(bytes),
               InvalidFieldValueException);
  EXPECT_THROW((void)encodeMessage(PlayerInput{
                   .inputNumber = SOME_INPUT_NUMBER,
                   .actions = 0,
                   .earlierActions = {FIRST_BIT_ABOVE_THE_ACTIONS}}),
               InvalidFieldValueException);
}

/**
 * Given Bydo 9 at (100, 50.5)
 * When its EntitySpawned is encoded
 * Then the payload is the id, the type 3, 800 and 404 eighths, then no owner
 * and no firing input
 */
TEST(EntitySpawned, EncodesTheIdTheTypeThenThePosition) {
  const auto expected =
      byteSequence({0x00, 0x00, 0x00, 0x09, 0x03, 0x03, 0x20, 0x01, 0x94, 0x00,
                    0x00, 0x00, 0x00, 0x00});

  EXPECT_EQ(encodeMessage(exampleBydo()), expected);
}

/**
 * Given an entity of the largest u32 id and of the largest type number, at the
 * smallest x and the largest y of the wire range
 * When its spawn is encoded then decoded
 * Then the decoded spawn equals the original
 */
TEST(EntitySpawned, KeepsTheLargestIdTheLargestTypeAndTheEndsOfTheWireRange) {
  const EntitySpawned spawned{.entityId = LARGEST_U32,
                              .entityType = LARGEST_TYPE_NUMBER,
                              .x = SMALLEST_WIRE_COORDINATE,
                              .y = LARGEST_WIRE_COORDINATE};

  EXPECT_EQ(decodeMessage<EntitySpawned>(encodeMessage(spawned)), spawned);
}

/**
 * Given a spawn of type 0, which no entry of the table has
 * When it is encoded
 * Then it is refused
 */
TEST(EntitySpawned, RefusesToWriteTypeZero) {
  EntitySpawned spawned = exampleBydo();
  spawned.entityType = NO_TYPE_NUMBER;

  EXPECT_THROW((void)encodeMessage(spawned), InvalidFieldValueException);
}

/**
 * Given the payload of Bydo 9 whose type byte is 0
 * When it is decoded
 * Then it is refused
 */
TEST(EntitySpawned, RefusesToReadTypeZero) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x09, 0x00, 0x03, 0x20,
                                   0x01, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<EntitySpawned>(bytes),
               InvalidFieldValueException);
}

/**
 * Given Bydo 9 at (100, 50.5)
 * When its EntitySpawned is encoded then decoded
 * Then the decoded message equals the original
 */
TEST(EntitySpawned, DecodesWhatItEncoded) {
  EXPECT_EQ(decodeMessage<EntitySpawned>(encodeMessage(exampleBydo())),
            exampleBydo());
}

/**
 * Given a payload whose entity id is 0
 * When it is decoded
 * Then an InvalidFieldValueException is raised, since no entity has the id 0
 */
TEST(EntitySpawned, RefusesToReadTheEntityIdZero) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x20,
                                   0x01, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<EntitySpawned>(bytes),
               InvalidFieldValueException);
}

/**
 * Given an entity whose id is 0
 * When its EntitySpawned is encoded
 * Then an InvalidFieldValueException is raised
 */
TEST(EntitySpawned, RefusesToWriteTheEntityIdZero) {
  EntitySpawned spawned = exampleBydo();
  spawned.entityId = 0;

  EXPECT_THROW((void)encodeMessage(spawned), InvalidFieldValueException);
}

/**
 * Given an entity whose y is not a number
 * When writing its EntitySpawned is refused
 * Then nothing was written, not even the id and the type that come first
 */
TEST(EntitySpawned, RefusedSpawnWritesNothing) {
  EntitySpawned spawned = exampleBydo();
  spawned.y = std::numeric_limits<float>::quiet_NaN();
  ByteWriter writer;

  EXPECT_THROW(encodePayload(writer, spawned), InvalidCoordinateException);

  EXPECT_TRUE(writer.bytes().empty());
}

/**
 * Given a payload holding the id and the type but only one coordinate
 * When it is decoded
 * Then a BufferUnderflowException is raised
 */
TEST(EntitySpawned, RefusesAPositionCutShort) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x09, 0x03, 0x03, 0x20});

  EXPECT_THROW((void)decodeMessage<EntitySpawned>(bytes),
               BufferUnderflowException);
}

/**
 * Given a missile of player 2 fired by input 300
 * When its EntitySpawned is encoded
 * Then its last five bytes are the owner 2, then 300 on four bytes
 */
TEST(EntitySpawned, EncodesTheOwnerThenTheFiringInput) {
  EntitySpawned missile = exampleBydo();
  missile.owner = SECOND_PLAYER_NUMBER;
  missile.firingInput = SOME_FIRING_INPUT;

  const std::vector<std::byte> payload = encodeMessage(missile);

  EXPECT_EQ(std::vector<std::byte>(payload.end() - OWNER_AND_INPUT_BYTE_COUNT,
                                   payload.end()),
            byteSequence({0x02, 0x00, 0x00, 0x01, 0x2C}));
}

/**
 * Given missiles of players 4 and 5
 * When each EntitySpawned is encoded
 * Then the owner 4 is written and the owner 5 refused: no fifth player
 */
TEST(EntitySpawned, WritesAnOwnerOfZeroToFourOnly) {
  EntitySpawned lastPlayer = exampleBydo();
  lastPlayer.owner = LAST_PLAYER_NUMBER;
  EntitySpawned pastTheLast = exampleBydo();
  pastTheLast.owner = PLAYER_NUMBER_ABOVE_THE_LAST;

  EXPECT_EQ(decodeMessage<EntitySpawned>(encodeMessage(lastPlayer)).owner,
            LAST_PLAYER_NUMBER);
  EXPECT_THROW((void)encodeMessage(pastTheLast), InvalidFieldValueException);
}

/**
 * Given the payload of an EntitySpawned whose owner byte is 5
 * When it is decoded
 * Then it is refused
 */
TEST(EntitySpawned, RefusesToReadAnOwnerPastTheLastPlayer) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x09, 0x03, 0x03, 0x20,
                                   0x01, 0x94, 0x05, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<EntitySpawned>(bytes),
               InvalidFieldValueException);
}

/**
 * Given a whole EntitySpawned followed by one more byte
 * When it is decoded
 * Then a TrailingBytesException is raised
 */
TEST(EntitySpawned, RefusesBytesAfterTheFiringInput) {
  const auto bytes =
      byteSequence({0x00, 0x00, 0x00, 0x09, 0x03, 0x03, 0x20, 0x01, 0x94, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<EntitySpawned>(bytes),
               TrailingBytesException);
}

/**
 * Given entity 9 killed
 * When its EntityDestroyed is encoded
 * Then the payload is the id, then the reason 1
 */
TEST(EntityDestroyed, EncodesTheIdThenTheReason) {
  const auto expected = byteSequence({0x00, 0x00, 0x00, 0x09, 0x01});

  EXPECT_EQ(encodeMessage(EntityDestroyed{.entityId = EXAMPLE_ENTITY_ID,
                                          .reason = DestructionReason::Killed}),
            expected);
}

/**
 * Given the reasons 1 and 2
 * When each payload is decoded
 * Then they give Killed and Removed
 */
TEST(EntityDestroyed, DecodesBothReasonsOfTheProtocolPage) {
  const auto killed = byteSequence({0x00, 0x00, 0x00, 0x09, 0x01});
  const auto removed = byteSequence({0x00, 0x00, 0x00, 0x09, 0x02});

  EXPECT_EQ(decodeMessage<EntityDestroyed>(killed).reason,
            DestructionReason::Killed);
  EXPECT_EQ(decodeMessage<EntityDestroyed>(removed).reason,
            DestructionReason::Removed);
}

/**
 * Given the reasons 0 and 3, just outside the reasons of the protocol page
 * When each payload is decoded
 * Then each is refused
 */
TEST(EntityDestroyed, RefusesToReadAReasonOutsideTheList) {
  const auto belowTheFirst = byteSequence({0x00, 0x00, 0x00, 0x09, 0x00});
  const auto aboveTheLast = byteSequence({0x00, 0x00, 0x00, 0x09, 0x03});

  EXPECT_THROW((void)decodeMessage<EntityDestroyed>(belowTheFirst),
               InvalidFieldValueException);
  EXPECT_THROW((void)decodeMessage<EntityDestroyed>(aboveTheLast),
               InvalidFieldValueException);
}

/**
 * Given a payload whose entity id is 0
 * When it is decoded
 * Then an InvalidFieldValueException is raised
 */
TEST(EntityDestroyed, RefusesToReadTheEntityIdZero) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x00, 0x01});

  EXPECT_THROW((void)decodeMessage<EntityDestroyed>(bytes),
               InvalidFieldValueException);
}

/**
 * Given a whole EntityDestroyed followed by one more byte
 * When it is decoded
 * Then a TrailingBytesException is raised
 */
TEST(EntityDestroyed, RefusesBytesAfterTheReason) {
  const auto bytes = byteSequence({0x00, 0x00, 0x00, 0x09, 0x01, 0x00});

  EXPECT_THROW((void)decodeMessage<EntityDestroyed>(bytes),
               TrailingBytesException);
}

/**
 * Given event 0x0102 telling that player 2 was hit
 * When it is encoded
 * Then the payload is the number, the kind 2, the player 2 and the entity 0
 */
TEST(GameEvent, EncodesEveryFieldInOrder) {
  const GameEvent event{.eventNumber = SOME_EVENT_NUMBER,
                        .kind = GameEventKind::PlayerHit,
                        .playerNumber = SECOND_PLAYER_NUMBER,
                        .entityId = 0};
  const auto expected =
      byteSequence({0x01, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00});

  EXPECT_EQ(encodeMessage(event), expected);
}

/**
 * Given a damage event of the largest u16 number, on the entity of the
 * largest u32 id
 * When it is encoded then decoded
 * Then the decoded event equals the original
 */
TEST(GameEvent, KeepsTheLargestEventNumberAndEntityId) {
  const GameEvent event{.eventNumber = LARGEST_U16,
                        .kind = GameEventKind::EntityDamaged,
                        .playerNumber = NO_PLAYER_NUMBER,
                        .entityId = LARGEST_U32};

  EXPECT_EQ(decodeMessage<GameEvent>(encodeMessage(event)), event);
}

/**
 * Given an event telling that entity 9 was damaged
 * When it is encoded then decoded
 * Then the decoded event equals the original
 */
TEST(GameEvent, DecodesWhatItEncoded) {
  const GameEvent event{.eventNumber = SOME_EVENT_NUMBER,
                        .kind = GameEventKind::EntityDamaged,
                        .playerNumber = NO_PLAYER_NUMBER,
                        .entityId = EXAMPLE_ENTITY_ID};

  EXPECT_EQ(decodeMessage<GameEvent>(encodeMessage(event)), event);
}

/**
 * Given an event numbered 0, which follows 65535 when the number wraps
 * When it is decoded
 * Then it is accepted
 */
TEST(GameEvent, AcceptsTheEventNumberZero) {
  const auto bytes =
      byteSequence({0x00, 0x00, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00});

  EXPECT_EQ(decodeMessage<GameEvent>(bytes).eventNumber, 0);
}

/**
 * Given the kinds 0 and 4, just outside the kinds of the protocol page
 * When each payload is decoded
 * Then each is refused
 */
TEST(GameEvent, RefusesToReadAKindOutsideTheList) {
  const auto belowTheFirst =
      byteSequence({0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00});
  const auto aboveTheLast =
      byteSequence({0x00, 0x01, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<GameEvent>(belowTheFirst),
               InvalidFieldValueException);
  EXPECT_THROW((void)decodeMessage<GameEvent>(aboveTheLast),
               InvalidFieldValueException);
}

/**
 * Given an EntityDamaged that names player 2
 * When it is decoded
 * Then it is refused, since a damaged entity names no player
 */
TEST(GameEvent, RefusesADamagedEntityNamingAPlayer) {
  const auto bytes =
      byteSequence({0x00, 0x01, 0x01, 0x02, 0x00, 0x00, 0x00, 0x09});

  EXPECT_THROW((void)decodeMessage<GameEvent>(bytes),
               InvalidFieldValueException);
}

/**
 * Given an EntityDamaged whose entity is 0
 * When it is decoded
 * Then it is refused, since it must name the damaged entity
 */
TEST(GameEvent, RefusesADamagedEntityWithoutEntity) {
  const auto bytes =
      byteSequence({0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<GameEvent>(bytes),
               InvalidFieldValueException);
}

/**
 * Given a PlayerHit whose player is 0
 * When it is decoded
 * Then it is refused, since it must name the player who was hit
 */
TEST(GameEvent, RefusesAPlayerHitWithoutPlayer) {
  const auto bytes =
      byteSequence({0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<GameEvent>(bytes),
               InvalidFieldValueException);
}

/**
 * Given a PlayerEliminated that names entity 9
 * When it is decoded
 * Then it is refused, since an eliminated player names no entity
 */
TEST(GameEvent, RefusesAnEliminatedPlayerNamingAnEntity) {
  const auto bytes =
      byteSequence({0x00, 0x01, 0x03, 0x01, 0x00, 0x00, 0x00, 0x09});

  EXPECT_THROW((void)decodeMessage<GameEvent>(bytes),
               InvalidFieldValueException);
}

/**
 * Given a PlayerEliminated naming a fifth player
 * When it is encoded
 * Then an InvalidFieldValueException is raised
 */
TEST(GameEvent, RefusesToWriteAPlayerOutsideOneToFour) {
  const GameEvent event{.eventNumber = SOME_EVENT_NUMBER,
                        .kind = GameEventKind::PlayerEliminated,
                        .playerNumber = PLAYER_NUMBER_ABOVE_THE_LAST,
                        .entityId = 0};

  EXPECT_THROW((void)encodeMessage(event), InvalidFieldValueException);
}

/**
 * Given a whole GameEvent followed by one more byte
 * When it is decoded
 * Then a TrailingBytesException is raised
 */
TEST(GameEvent, RefusesBytesAfterTheEntityId) {
  const auto bytes =
      byteSequence({0x00, 0x01, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00});

  EXPECT_THROW((void)decodeMessage<GameEvent>(bytes), TrailingBytesException);
}

/**
 * Given player 4 timed out
 * When its PlayerLeft is encoded
 * Then the payload is the number 4, then the reason 2
 */
TEST(PlayerLeft, EncodesTheNumberThenTheReason) {
  const auto expected = byteSequence({0x04, 0x02});

  EXPECT_EQ(encodeMessage(PlayerLeft{.playerNumber = LAST_PLAYER_NUMBER,
                                     .reason = DepartureReason::TimedOut}),
            expected);
}

/**
 * Given the reasons 1 and 2
 * When each payload is decoded
 * Then they give Left and TimedOut
 */
TEST(PlayerLeft, DecodesBothReasonsOfTheProtocolPage) {
  const auto left = byteSequence({0x01, 0x01});
  const auto timedOut = byteSequence({0x01, 0x02});

  EXPECT_EQ(decodeMessage<PlayerLeft>(left).reason, DepartureReason::Left);
  EXPECT_EQ(decodeMessage<PlayerLeft>(timedOut).reason,
            DepartureReason::TimedOut);
}

/**
 * Given the reasons 0 and 3, just outside the reasons of the protocol page
 * When each payload is decoded
 * Then each is refused
 */
TEST(PlayerLeft, RefusesToReadAReasonOutsideTheList) {
  const auto belowTheFirst = byteSequence({0x01, 0x00});
  const auto aboveTheLast = byteSequence({0x01, 0x03});

  EXPECT_THROW((void)decodeMessage<PlayerLeft>(belowTheFirst),
               InvalidFieldValueException);
  EXPECT_THROW((void)decodeMessage<PlayerLeft>(aboveTheLast),
               InvalidFieldValueException);
}

/**
 * Given the player numbers 0 and 5
 * When each payload is decoded
 * Then each is refused
 */
TEST(PlayerLeft, RefusesToReadAPlayerNumberOutsideOneToFour) {
  const auto belowTheFirst = byteSequence({0x00, 0x01});
  const auto aboveTheLast = byteSequence({0x05, 0x01});

  EXPECT_THROW((void)decodeMessage<PlayerLeft>(belowTheFirst),
               InvalidFieldValueException);
  EXPECT_THROW((void)decodeMessage<PlayerLeft>(aboveTheLast),
               InvalidFieldValueException);
}

/**
 * Given a whole PlayerLeft followed by one more byte
 * When it is decoded
 * Then a TrailingBytesException is raised
 */
TEST(PlayerLeft, RefusesBytesAfterTheReason) {
  const auto bytes = byteSequence({0x01, 0x01, 0x00});

  EXPECT_THROW((void)decodeMessage<PlayerLeft>(bytes), TrailingBytesException);
}

/**
 * Given the WorldState payload of the protocol page
 * When it is decoded through decodeMessage, as the dispatcher does
 * Then it gives the state WorldStateCodec gives
 */
TEST(WorldState, DecodesThroughTheSameEntryAsTheOtherMessages) {
  const auto payload = WorldStateCodec::encode(oneShipState()).front();

  EXPECT_EQ(decodeMessage<WorldState>(payload), oneShipState());
}

}  // namespace rtype::network
