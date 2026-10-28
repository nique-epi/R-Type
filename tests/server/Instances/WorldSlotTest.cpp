#include <gtest/gtest.h>
#include <cstdint>
#include "WorldSlot.hpp"
#include "WorldSlotTestConstants.hpp"

namespace rtype::server {

/**
 * Given a slot whose world holds the ship of a player
 * When the slot is renewed
 * Then the new world holds no ship under that network id
 */
TEST(WorldSlot, RenewedWorldHoldsNothingOfThePreviousOne) {
  WorldSlot slot;
  const std::uint32_t shipId = slot.current().spawnPlayer(testing::SHIP_START);
  ASSERT_NE(slot.current().shipPositionOf(shipId), nullptr);

  slot.renew();

  EXPECT_EQ(slot.current().shipPositionOf(shipId), nullptr);
}

/**
 * Given a slot
 * When it is renewed
 * Then it gives the new world as the current one
 */
TEST(WorldSlot, RenewGivesTheWorldItNowHolds) {
  WorldSlot slot;

  const game::World& renewed = slot.renew();

  EXPECT_EQ(&renewed, &slot.current());
}

}  // namespace rtype::server
