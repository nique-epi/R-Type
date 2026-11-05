#include <gtest/gtest.h>
#include <optional>
#include "EntityType.hpp"
#include "EntityTypeLook.hpp"
#include "EntityTypeLooksFixture.hpp"
#include "EntityTypeLooksTestConstants.hpp"

namespace rtype::client {

using game::EntityType;
using testing::EntityTypeLooksFixture;
using testing::LOOKED_UP_TYPE_NAME;
using testing::LOOKED_UP_TYPE_NUMBER;
using testing::NUMBER_PAST_THE_TABLE;
using testing::TYPE_WITHOUT_APPEARANCE_NUMBER;

/**
 * Given a table whose first type is an enemy this client has a sheet for
 * When the look of number 1 is asked for
 * Then it is an enemy
 */
TEST_F(EntityTypeLooksFixture, GivesTheSideTheTableGivesTheType) {
  const std::optional<EntityTypeLook> look =
      looks().lookOf(LOOKED_UP_TYPE_NUMBER);
  ASSERT_TRUE(look.has_value());

  EXPECT_EQ(look.has_value() ? look->side : EntityType::Player,
            EntityType::Enemy);
}

/**
 * Given a table whose first type has a sheet on this client
 * When the look of number 1 is asked for
 * Then it shows the appearance of that sheet
 */
TEST_F(EntityTypeLooksFixture, GivesTheAppearanceOfTheSheetOfTheType) {
  const std::optional<EntityTypeLook> look =
      looks().lookOf(LOOKED_UP_TYPE_NUMBER);
  ASSERT_TRUE(look.has_value());

  EXPECT_EQ(look.has_value() ? &look->appearance.get() : nullptr,
            appearances().named(LOOKED_UP_TYPE_NAME));
}

/**
 * Given a table of two types
 * When the look of number 3 is asked for
 * Then there is none
 */
TEST_F(EntityTypeLooksFixture, KnowsNoLookPastTheTable) {
  EXPECT_FALSE(looks().lookOf(NUMBER_PAST_THE_TABLE).has_value());
}

/**
 * Given a table whose second type has no sheet on this client
 * When the look of number 2 is asked for
 * Then there is none
 */
TEST_F(EntityTypeLooksFixture, KnowsNoLookForATypeWithoutAppearance) {
  EXPECT_FALSE(looks().lookOf(TYPE_WITHOUT_APPEARANCE_NUMBER).has_value());
}

}  // namespace rtype::client
