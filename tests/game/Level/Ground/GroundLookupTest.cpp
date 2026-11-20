#include <gtest/gtest.h>
#include <optional>
#include "GroundLookup.hpp"
#include "GroundLookupFixtures.hpp"
#include "GroundLookupTestConstants.hpp"
#include "GroundPlace.hpp"

using rtype::game::ceilingRowAt;
using rtype::game::floorRowAt;
using rtype::game::GroundPlace;

/**
 * Given a biome that has just begun: its first map stands at the right edge
 * When the floor is looked up at the right edge
 * Then it is the floor of the first column of the map
 */
TEST(GroundLookup, ABiomeBeginsAtTheRightEdge) {
  const std::optional<float> row =
      floorRowAt(groundOfFourColumns(),
                 GroundPlace{.x = RIGHT_EDGE, .scrolled = JUST_BEGUN});

  EXPECT_EQ(row.value_or(0.0F), FIRST_FLOOR_ROW);
}

/**
 * Given a biome that has just begun
 * When the floor is looked up at the left edge, left of its first map
 * Then there is none
 */
TEST(GroundLookup, NothingLiesLeftOfTheFirstMap) {
  EXPECT_FALSE(floorRowAt(groundOfFourColumns(),
                          GroundPlace{.x = LEFT_EDGE, .scrolled = JUST_BEGUN})
                   .has_value());
}

/**
 * Given a biome scrolled a whole screen: its first map at the left edge
 * When the floor is looked up on the second pixel
 * Then it is the floor of the second column, 4 units a pixel
 */
TEST(GroundLookup, FollowsTheScroll) {
  const std::optional<float> row =
      floorRowAt(groundOfFourColumns(),
                 GroundPlace{.x = SECOND_PIXEL, .scrolled = SCROLLED_A_SCREEN});

  EXPECT_EQ(row.value_or(0.0F), SECOND_FLOOR_ROW);
}

/**
 * Given a biome scrolled a screen and the width of its map
 * When the floor is looked up at the left edge
 * Then the map came back: it is the floor of its first column
 */
TEST(GroundLookup, TheMapsComeBackAfterTheLast) {
  const std::optional<float> row = floorRowAt(
      groundOfFourColumns(),
      GroundPlace{.x = LEFT_EDGE, .scrolled = SCROLLED_A_SCREEN_AND_A_MAP});

  EXPECT_EQ(row.value_or(0.0F), FIRST_FLOOR_ROW);
}

/**
 * Given a biome scrolled a screen, whose third column has no floor
 * When the floor is looked up on the third pixel
 * Then there is none
 */
TEST(GroundLookup, AColumnWithoutFloorGivesNone) {
  EXPECT_FALSE(
      floorRowAt(groundOfFourColumns(),
                 GroundPlace{.x = THIRD_PIXEL, .scrolled = SCROLLED_A_SCREEN})
          .has_value());
}

/**
 * Given a biome scrolled a screen
 * When the ceiling is looked up at the left edge
 * Then it is the ceiling of the first column
 */
TEST(GroundLookup, ReadsTheCeilingTheSameWay) {
  const std::optional<float> row =
      ceilingRowAt(groundOfFourColumns(),
                   GroundPlace{.x = LEFT_EDGE, .scrolled = SCROLLED_A_SCREEN});

  EXPECT_EQ(row.value_or(0.0F), FIRST_CEILING_ROW);
}
