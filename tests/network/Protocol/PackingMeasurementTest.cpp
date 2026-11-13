#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>
#include "MessagePayload.hpp"
#include "PackingMeasurementFixtures.hpp"
#include "PackingMeasurementTestConstants.hpp"
#include "PlayerInput.hpp"
#include "ProtocolTestConstants.hpp"
#include "RealWorldStateBodiesTestConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateCodec.hpp"

using rtype::network::encodeMessage;
using rtype::network::PlayerInput;
using rtype::network::WorldState;
using rtype::network::WorldStateCodec;
using rtype::network::testing::CAPTURED_ENTITIES;
using rtype::network::testing::CAPTURED_PLAYERS;
using rtype::network::testing::capturedWorldState;
using rtype::network::testing::INPUT_FIXED_BYTES;
using rtype::network::testing::INPUTS_IN_A_TYPICAL_MESSAGE;
using rtype::network::testing::LARGE_WORLD_STATE_BODY;
using rtype::network::testing::PACKED_ACTIONS_BYTES;
using rtype::network::testing::PACKED_ENTITY_BYTES;
using rtype::network::testing::PACKED_PLAYER_BYTES;
using rtype::network::testing::PACKED_STATE_FIXED_BYTES;
using rtype::network::testing::SOME_INPUT_NUMBER;
using rtype::network::testing::TYPICAL_REPEATED_INPUTS;
using rtype::network::testing::UNPACKED_ACTIONS_BYTES;
using rtype::network::testing::UNPACKED_ENTITY_BYTES;
using rtype::network::testing::UNPACKED_PLAYER_BYTES;
using rtype::network::testing::unpackedPlayerInputBytes;
using rtype::network::testing::unpackedWorldStateBytes;
using rtype::network::testing::UP_AND_SHOOT;

/**
 * Given the body of a world state the real server sent during a game of four
 * When it is decoded
 * Then it lists four players and 22 entities, the state the others measure
 */
TEST(PackingMeasurement, TheCapturedStateIsAGameOfFour) {
  const WorldState state = capturedWorldState(LARGE_WORLD_STATE_BODY);

  EXPECT_EQ(state.players.size(), CAPTURED_PLAYERS);
  EXPECT_EQ(state.entities.size(), CAPTURED_ENTITIES);
}

/**
 * Given the typical state of a game of four
 * When WorldStateCodec encodes it
 * Then its payload is 6 bytes, plus 16 per player, plus 10 per entity
 */
TEST(PackingMeasurement, APackedStateTakesTheSizeOfTheProtocol) {
  const WorldState state = capturedWorldState(LARGE_WORLD_STATE_BODY);

  const std::vector<std::vector<std::byte>> payloads =
      WorldStateCodec::encode(state);

  ASSERT_EQ(payloads.size(), 1U);
  std::cout << "typical world state: " << payloads.front().size()
            << " bytes packed\n";
  EXPECT_EQ(payloads.front().size(),
            PACKED_STATE_FIXED_BYTES +
                (CAPTURED_PLAYERS * PACKED_PLAYER_BYTES) +
                (CAPTURED_ENTITIES * PACKED_ENTITY_BYTES));
}

/**
 * Given the typical state of a game of four
 * When it is written without packing
 * Then it takes 14 bytes per entity and 21 per player instead of 10 and 16
 */
TEST(PackingMeasurement, AnUnpackedStateTakesMoreBytes) {
  const WorldState state = capturedWorldState(LARGE_WORLD_STATE_BODY);

  const std::size_t unpacked = unpackedWorldStateBytes(state).size();

  std::cout << "typical world state: " << unpacked << " bytes unpacked\n";
  EXPECT_EQ(unpacked, PACKED_STATE_FIXED_BYTES +
                          (CAPTURED_PLAYERS * UNPACKED_PLAYER_BYTES) +
                          (CAPTURED_ENTITIES * UNPACKED_ENTITY_BYTES));
}

/**
 * Given an input with up and shoot held, repeating five unconfirmed inputs
 * When it is written packed, then without packing
 * Then each input costs one byte of actions packed and five unpacked
 */
TEST(PackingMeasurement, AnUnpackedInputTakesAByteForEachAction) {
  const PlayerInput input{.inputNumber = SOME_INPUT_NUMBER,
                          .actions = UP_AND_SHOOT,
                          .earlierActions = std::vector<std::uint8_t>(
                              TYPICAL_REPEATED_INPUTS, UP_AND_SHOOT)};

  const std::size_t packed = encodeMessage(input).size();
  const std::size_t unpacked = unpackedPlayerInputBytes(input).size();

  std::cout << "typical input: " << packed << " bytes packed, " << unpacked
            << " unpacked\n";
  EXPECT_EQ(packed, INPUT_FIXED_BYTES +
                        (INPUTS_IN_A_TYPICAL_MESSAGE * PACKED_ACTIONS_BYTES));
  EXPECT_EQ(unpacked, INPUT_FIXED_BYTES + (INPUTS_IN_A_TYPICAL_MESSAGE *
                                           UNPACKED_ACTIONS_BYTES));
}
