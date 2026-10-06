#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include "EntityIndexMap.hpp"

using rtype::engine::EntityIndexMap;

namespace {

constexpr std::uint32_t FIRST_ENTITY = 3;
constexpr std::uint32_t SECOND_ENTITY = 7;
constexpr std::uint32_t THIRD_ENTITY = 1;
constexpr std::uint32_t UNKNOWN_ENTITY = 100;
constexpr std::size_t FIRST_POSITION = 0;
constexpr std::size_t SECOND_POSITION = 1;
constexpr std::size_t THIRD_POSITION = 2;

EntityIndexMap makeMapOfThree() {
  EntityIndexMap map;
  map.append(FIRST_ENTITY);
  map.append(SECOND_ENTITY);
  map.append(THIRD_ENTITY);
  return map;
}

}  // namespace

/**
 * Given an empty map
 * When an entity index is looked up
 * Then it is absent
 */
TEST(EntityIndexMap, FindOnEmptyMapIsAbsent) {
  const EntityIndexMap map;

  EXPECT_FALSE(map.find(FIRST_ENTITY).has_value());
  EXPECT_EQ(map.size(), 0U);
}

/**
 * Given a map with three entity indices
 * When they are looked up
 * Then each one is at the position where it was appended
 */
TEST(EntityIndexMap, AppendedEntitiesTakeContiguousPositions) {
  const EntityIndexMap map = makeMapOfThree();

  EXPECT_EQ(map.find(FIRST_ENTITY), FIRST_POSITION);
  EXPECT_EQ(map.find(SECOND_ENTITY), SECOND_POSITION);
  EXPECT_EQ(map.find(THIRD_ENTITY), THIRD_POSITION);
  EXPECT_EQ(map.size(), THIRD_POSITION + 1);
}

/**
 * Given a map that already holds an entity index
 * When the same index is appended again
 * Then the map still holds it once, at its original position
 */
TEST(EntityIndexMap, AppendingTwiceKeepsOneEntry) {
  EntityIndexMap map = makeMapOfThree();

  map.append(SECOND_ENTITY);

  EXPECT_EQ(map.size(), THIRD_POSITION + 1);
  EXPECT_EQ(map.find(SECOND_ENTITY), SECOND_POSITION);
}

/**
 * Given a map with three entity indices
 * When the first one is erased
 * Then the vacated position is returned and the last entity index takes it
 */
TEST(EntityIndexMap, ErasingFirstMovesLastIntoItsPosition) {
  EntityIndexMap map = makeMapOfThree();

  const std::optional<std::size_t> vacated = map.erase(FIRST_ENTITY);

  EXPECT_EQ(vacated, FIRST_POSITION);
  EXPECT_FALSE(map.find(FIRST_ENTITY).has_value());
  EXPECT_EQ(map.find(THIRD_ENTITY), FIRST_POSITION);
  EXPECT_EQ(map.find(SECOND_ENTITY), SECOND_POSITION);
  EXPECT_EQ(map.size(), THIRD_POSITION);
}

/**
 * Given a map with three entity indices
 * When the last one is erased
 * Then the others keep their positions and the erased one is absent
 */
TEST(EntityIndexMap, ErasingLastLeavesOthersInPlace) {
  EntityIndexMap map = makeMapOfThree();

  const std::optional<std::size_t> vacated = map.erase(THIRD_ENTITY);

  EXPECT_EQ(vacated, THIRD_POSITION);
  EXPECT_FALSE(map.find(THIRD_ENTITY).has_value());
  EXPECT_EQ(map.find(FIRST_ENTITY), FIRST_POSITION);
  EXPECT_EQ(map.find(SECOND_ENTITY), SECOND_POSITION);
  EXPECT_EQ(map.size(), THIRD_POSITION);
}

/**
 * Given a map with a single entity index
 * When it is erased
 * Then the map is empty and the index can be appended again
 */
TEST(EntityIndexMap, ErasingTheOnlyEntryEmptiesTheMap) {
  EntityIndexMap map;
  map.append(FIRST_ENTITY);

  const std::optional<std::size_t> vacated = map.erase(FIRST_ENTITY);
  map.append(FIRST_ENTITY);

  EXPECT_EQ(vacated, FIRST_POSITION);
  EXPECT_EQ(map.find(FIRST_ENTITY), FIRST_POSITION);
  EXPECT_EQ(map.size(), SECOND_POSITION);
}

/**
 * Given a map with three entity indices
 * When an absent entity index is erased
 * Then nothing is removed
 */
TEST(EntityIndexMap, ErasingAbsentEntityChangesNothing) {
  EntityIndexMap map = makeMapOfThree();

  EXPECT_FALSE(map.erase(UNKNOWN_ENTITY).has_value());
  EXPECT_EQ(map.size(), THIRD_POSITION + 1);
}

/**
 * Given a map with three entity indices
 * When the entity index at each position is requested
 * Then they come back in the order they were appended
 */
TEST(EntityIndexMap, EntityIndexAtFollowsAppendOrder) {
  const EntityIndexMap map = makeMapOfThree();

  EXPECT_EQ(map.entityIndexAt(FIRST_POSITION), FIRST_ENTITY);
  EXPECT_EQ(map.entityIndexAt(SECOND_POSITION), SECOND_ENTITY);
  EXPECT_EQ(map.entityIndexAt(THIRD_POSITION), THIRD_ENTITY);
}

/**
 * Given a map with three entity indices
 * When the first one is erased
 * Then the vacated position holds the entity index that was last
 */
TEST(EntityIndexMap, EntityIndexAtSeesTheEntityThatTookTheVacatedPosition) {
  EntityIndexMap map = makeMapOfThree();

  map.erase(FIRST_ENTITY);

  EXPECT_EQ(map.entityIndexAt(FIRST_POSITION), THIRD_ENTITY);
  EXPECT_EQ(map.entityIndexAt(SECOND_POSITION), SECOND_ENTITY);
  EXPECT_EQ(map.size(), SECOND_POSITION + 1);
}
