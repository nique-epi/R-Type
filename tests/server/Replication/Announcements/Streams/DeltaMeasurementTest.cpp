#include <gtest/gtest.h>
#include <cstddef>
#include <iostream>
#include "DeltaStateSenderFixture.hpp"
#include "DeltaStateSenderTestConstants.hpp"
#include "StateEncoding.hpp"

namespace rtype::server::testing {

/**
 * Given ten seconds of a game of 30 enemies that all stand still, a player
 * acknowledging every state at once
 * When the bytes sent to the player are counted
 * Then the first state is in full and each of the 199 others lists no entity:
 * far less than 200 states in full
 */
TEST_F(DeltaStateSenderFixture, SendsNoEntityWhenEveryEntityStandsStill) {
  const std::size_t bytes = bytesOfAMeasuredRun(0);

  std::cout << "still scene: " << bytes / MEASURED_SECONDS
            << " bytes/s to one player, "
            << (MEASURED_STATES * FULL_STATE_BYTES) / MEASURED_SECONDS
            << " in full\n";
  EXPECT_EQ(bytes, FULL_STATE_BYTES +
                       ((MEASURED_STATES - 1) * STATE_BYTES_WITHOUT_ENTITIES));
}

/**
 * Given ten seconds of a game of 30 enemies of which 15 move at every tick
 * When the bytes sent to the player are counted
 * Then each state after the first lists the 15 that moved
 */
TEST_F(DeltaStateSenderFixture,
       ListsOnlyTheEntitiesThatMovedWhenHalfOfThemMove) {
  const std::size_t bytes = bytesOfAMeasuredRun(HALF_THE_ENTITIES);

  std::cout << "half moving: " << bytes / MEASURED_SECONDS
            << " bytes/s to one player, "
            << (MEASURED_STATES * FULL_STATE_BYTES) / MEASURED_SECONDS
            << " in full\n";
  EXPECT_EQ(bytes, FULL_STATE_BYTES + ((MEASURED_STATES - 1) *
                                       (STATE_BYTES_WITHOUT_ENTITIES +
                                        (HALF_THE_ENTITIES * ENTITY_BYTES))));
}

/**
 * Given ten seconds of a game of 30 enemies that all move at every tick
 * When the bytes sent to the player are counted
 * Then each state lists every entity, as a state in full does: a delta never
 * costs more than a state in full
 */
TEST_F(DeltaStateSenderFixture, CostsAsMuchAsFullStatesWhenEveryEntityMoves) {
  const std::size_t bytes = bytesOfAMeasuredRun(MEASURED_ENTITIES);

  std::cout << "all moving: " << bytes / MEASURED_SECONDS
            << " bytes/s to one player, "
            << (MEASURED_STATES * FULL_STATE_BYTES) / MEASURED_SECONDS
            << " in full\n";
  EXPECT_EQ(bytes, MEASURED_STATES * FULL_STATE_BYTES);
}

/**
 * Given a sender that writes every state in full, and ten seconds of a game of
 * 30 enemies that all stand still
 * When the bytes sent to the player are counted
 * Then each of the 200 states lists every entity, as a state in full does
 */
TEST_F(DeltaStateSenderFixture, ListsEveryEntityInEveryStateWhenWritingInFull) {
  encodeStatesAs(StateEncoding::Full);

  const std::size_t bytes = bytesOfAMeasuredRun(0);

  EXPECT_EQ(bytes, MEASURED_STATES * FULL_STATE_BYTES);
}

}  // namespace rtype::server::testing
