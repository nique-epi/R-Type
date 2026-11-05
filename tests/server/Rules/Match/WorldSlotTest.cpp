#include <gtest/gtest.h>
#include <cstdint>
#include "EntityTypesTestConstants.hpp"
#include "ShippedEntitySheets.hpp"
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
  const std::uint32_t shipId = slot.current().spawn(
      game::testing::shippedEntitySheets().named(game::testing::SHIP_SHEET),
      testing::SHIP_START);
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

/**
 * Given a new slot
 * When its entities are counted
 * Then there is none
 */
TEST(WorldSlot, ANewSlotHoldsNoEntity) {
  const WorldSlot slot;

  EXPECT_EQ(slot.entityCount(), 0U);
}

/**
 * Given a slot whose world holds the ship of a player
 * When its entities are counted
 * Then there is one
 */
TEST(WorldSlot, CountsTheEntitiesOfItsWorld) {
  WorldSlot slot;
  slot.current().spawn(
      game::testing::shippedEntitySheets().named(game::testing::SHIP_SHEET),
      testing::SHIP_START);

  EXPECT_EQ(slot.entityCount(), 1U);
}

/**
 * Given a slot whose world holds the ship of a player
 * When the slot is renewed
 * Then it counts the entities of the new world, none
 */
TEST(WorldSlot, CountsTheEntitiesOfTheNewWorldOnceRenewed) {
  WorldSlot slot;
  slot.current().spawn(
      game::testing::shippedEntitySheets().named(game::testing::SHIP_SHEET),
      testing::SHIP_START);

  slot.renew();

  EXPECT_EQ(slot.entityCount(), 0U);
}

}  // namespace rtype::server
