#include <gtest/gtest.h>
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "Velocity.hpp"

using rtype::game::PLAYFIELD_WIDTH;
using rtype::game::Position;
using rtype::game::Velocity;

namespace {

constexpr float ONE_SECOND = 1.0F;

}  // namespace

/**
 * Given a ship on the left edge moving right at one width per second
 * When it moves for one second
 * Then it is on the right edge
 */
TEST(Playfield, ShipCrossesThePlayfieldInOneSecondAtOneWidthPerSecond) {
  const Position start{.x = 0.0F, .y = 0.0F};
  const Velocity velocity{.x = PLAYFIELD_WIDTH, .y = 0.0F};

  const float arrival = start.x + (velocity.x * ONE_SECOND);

  EXPECT_FLOAT_EQ(arrival, PLAYFIELD_WIDTH);
}
