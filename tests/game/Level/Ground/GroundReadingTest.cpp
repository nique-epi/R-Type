#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>
#include "BiomeGround.hpp"
#include "GameException.hpp"
#include "GroundConstants.hpp"
#include "GroundReading.hpp"
#include "GroundReadingTestConstants.hpp"
#include "TemporaryFolder.hpp"

using rtype::game::BiomeGround;
using rtype::game::GROUND_FIELD_MISSING_REASON;
using rtype::game::GROUND_FIELD_SIZE_REASON;
using rtype::game::InvalidLdtkFileException;
using rtype::game::ldtkGroundOf;
using ::testing::ElementsAreArray;
using ::testing::HasSubstr;
using ::testing::ThrowsMessage;

/**
 * Given a project of two levels, each giving its floor and its ceiling
 * When its ground is read
 * Then the rows of both levels come laid end to end, in the order of the
 * file
 */
TEST(GroundReading, LaysTheGroundOfTheLevelsEndToEnd) {
  const TemporaryFolder root;
  root.write(GROUND_LDTK_FILE, TWO_LEVELS_WITH_GROUND);

  const std::optional<BiomeGround> ground =
      ldtkGroundOf(root.path() / GROUND_LDTK_FILE);

  ASSERT_TRUE(ground.has_value());
  const BiomeGround read = ground.value_or(BiomeGround{});
  EXPECT_THAT(read.floorRows, ElementsAreArray(FLOORS_OF_THE_TWO_LEVELS));
  EXPECT_THAT(read.ceilingRows, ElementsAreArray(CEILINGS_OF_THE_TWO_LEVELS));
}

/**
 * Given a project exported before the ground existed
 * When its ground is read
 * Then there is none
 */
TEST(GroundReading, AProjectWithoutGroundGivesNone) {
  const TemporaryFolder root;
  root.write(GROUND_LDTK_FILE, PROJECT_WITHOUT_GROUND);

  EXPECT_FALSE(ldtkGroundOf(root.path() / GROUND_LDTK_FILE).has_value());
}

/**
 * Given a level 3 pixels wide whose floor gives 2 rows
 * When its ground is read
 * Then the file is refused for a field that is not a row a column
 */
TEST(GroundReading, RefusesAFieldShorterThanItsLevel) {
  const TemporaryFolder root;
  root.write(GROUND_LDTK_FILE, FLOOR_SHORTER_THAN_ITS_LEVEL);

  EXPECT_THAT(
      [&root] {
        static_cast<void>(ldtkGroundOf(root.path() / GROUND_LDTK_FILE));
      },
      ThrowsMessage<InvalidLdtkFileException>(
          HasSubstr(GROUND_FIELD_SIZE_REASON)));
}

/**
 * Given a project whose first level gives its ground and whose second gives
 * no ceiling
 * When its ground is read
 * Then the file is refused for the missing field
 */
TEST(GroundReading, RefusesALevelWithoutItsCeiling) {
  const TemporaryFolder root;
  root.write(GROUND_LDTK_FILE, SECOND_LEVEL_WITHOUT_CEILING);

  EXPECT_THAT(
      [&root] {
        static_cast<void>(ldtkGroundOf(root.path() / GROUND_LDTK_FILE));
      },
      ThrowsMessage<InvalidLdtkFileException>(
          HasSubstr(GROUND_FIELD_MISSING_REASON)));
}
