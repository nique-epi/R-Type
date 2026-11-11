#include <gtest/gtest.h>
#include <algorithm>
#include <cstdint>
#include <limits>
#include <vector>
#include "NetworkException.hpp"
#include "ProtocolTestConstants.hpp"
#include "StateDelta.hpp"
#include "WorldState.hpp"
#include "WorldStateDelta.hpp"
#include "WorldStateDeltaTestConstants.hpp"
#include "WorldStateFixtures.hpp"

using rtype::network::applyDelta;
using rtype::network::diffStates;
using rtype::network::EntityState;
using rtype::network::InvalidCoordinateException;
using rtype::network::StateDelta;
using rtype::network::testing::ADDED_X;
using rtype::network::testing::ADDED_Y;
using rtype::network::testing::APPLIED_X;
using rtype::network::testing::APPLIED_Y;
using rtype::network::testing::ENTITIES_IN_A_LIST;
using rtype::network::testing::EXAMPLE_ENTITY_TYPE;
using rtype::network::testing::FAR_BELOW_AN_EIGHTH;
using rtype::network::testing::FIRST_DUPLICATE_X;
using rtype::network::testing::MOVED_ENTITY_ID;
using rtype::network::testing::NEW_ENTITY_ID;
using rtype::network::testing::ONE_EIGHTH;
using rtype::network::testing::OTHER_ENTITY_TYPE;
using rtype::network::testing::OTHER_LOOK;
using rtype::network::testing::SECOND_DUPLICATE_X;
using rtype::network::testing::someEntities;
using rtype::network::testing::VANISHED_ENTITY_ID;

/**
 * Given two identical lists of entities
 * When they are compared
 * Then nothing is changed and nothing is removed
 */
TEST(WorldStateDelta, DiffOfTwoIdenticalListsIsEmpty) {
  const std::vector<EntityState> entities = someEntities(ENTITIES_IN_A_LIST);

  EXPECT_EQ(diffStates(entities, entities), StateDelta{});
}

/**
 * Given a list and the same list with one entity moved by an eighth of a unit
 * When they are compared
 * Then that entity alone is changed, as it is in the later list
 */
TEST(WorldStateDelta, ListsAnEntityMovedByAnEighthAsChanged) {
  const std::vector<EntityState> base = someEntities(ENTITIES_IN_A_LIST);
  std::vector<EntityState> current = base;
  current.at(MOVED_ENTITY_ID - 1).x += ONE_EIGHTH;

  const StateDelta delta = diffStates(base, current);

  EXPECT_EQ(delta.changed,
            std::vector<EntityState>{current.at(MOVED_ENTITY_ID - 1)});
  EXPECT_TRUE(delta.removed.empty());
}

/**
 * Given a list and the same list with one entity moved by far less than an
 * eighth of a unit
 * When they are compared
 * Then nothing is changed, since a receiver reads the same position
 */
TEST(WorldStateDelta, IgnoresAMoveThatRoundsToTheSameWirePosition) {
  const std::vector<EntityState> base = someEntities(ENTITIES_IN_A_LIST);
  std::vector<EntityState> current = base;
  current.at(MOVED_ENTITY_ID - 1).x += FAR_BELOW_AN_EIGHTH;

  EXPECT_EQ(diffStates(base, current), StateDelta{});
}

/**
 * Given a list and the same list where an entity has another type at the same
 * place
 * When they are compared
 * Then that entity is changed
 */
TEST(WorldStateDelta, ListsAnEntityOfAnotherTypeAsChanged) {
  const std::vector<EntityState> base = someEntities(ENTITIES_IN_A_LIST);
  std::vector<EntityState> current = base;
  current.at(MOVED_ENTITY_ID - 1).entityType = OTHER_ENTITY_TYPE;

  EXPECT_EQ(diffStates(base, current).changed,
            std::vector<EntityState>{current.at(MOVED_ENTITY_ID - 1)});
}

/**
 * Given a list and the same list where an entity shows another look at the
 * same place
 * When they are compared
 * Then that entity alone is changed, as it is in the later list
 */
TEST(WorldStateDelta, ListsAnEntityShowingAnotherLookAsChanged) {
  const std::vector<EntityState> base = someEntities(ENTITIES_IN_A_LIST);
  std::vector<EntityState> current = base;
  current.at(MOVED_ENTITY_ID - 1).look = OTHER_LOOK;

  EXPECT_EQ(diffStates(base, current).changed,
            std::vector<EntityState>{current.at(MOVED_ENTITY_ID - 1)});
}

/**
 * Given a list and the same list with one more entity and one fewer
 * When they are compared
 * Then the new entity is changed and the missing id is removed
 */
TEST(WorldStateDelta, ListsANewEntityAsChangedAndAMissingOneAsRemoved) {
  const std::vector<EntityState> base = someEntities(ENTITIES_IN_A_LIST);
  std::vector<EntityState> current = base;
  current.erase(current.begin() + (VANISHED_ENTITY_ID - 1));
  const EntityState added{.entityId = NEW_ENTITY_ID,
                          .entityType = OTHER_ENTITY_TYPE,
                          .x = ADDED_X,
                          .y = ADDED_Y};
  current.push_back(added);

  const StateDelta delta = diffStates(base, current);

  EXPECT_EQ(delta.changed, std::vector<EntityState>{added});
  EXPECT_EQ(delta.removed, std::vector<std::uint32_t>{VANISHED_ENTITY_ID});
}

/**
 * Given an empty earlier list and a list of entities
 * When they are compared
 * Then every entity is changed and none is removed
 */
TEST(WorldStateDelta, DiffFromNothingListsEveryEntityAsChanged) {
  const std::vector<EntityState> current = someEntities(ENTITIES_IN_A_LIST);

  const StateDelta delta = diffStates({}, current);

  EXPECT_EQ(delta.changed, current);
  EXPECT_TRUE(delta.removed.empty());
}

/**
 * Given a list of entities and an empty later list
 * When they are compared
 * Then every id is removed and none is changed
 */
TEST(WorldStateDelta, DiffToNothingRemovesEveryEntity) {
  const std::vector<EntityState> base = someEntities(ENTITIES_IN_A_LIST);

  const StateDelta delta = diffStates(base, {});

  EXPECT_TRUE(delta.changed.empty());
  EXPECT_EQ(delta.removed.size(), ENTITIES_IN_A_LIST);
}

/**
 * Given an entity whose x is NaN
 * When the list is compared with one that has it
 * Then InvalidCoordinateException is thrown
 */
TEST(WorldStateDelta, RefusesACoordinateThatIsNotANumber) {
  const std::vector<EntityState> base = someEntities(ENTITIES_IN_A_LIST);
  std::vector<EntityState> current = base;
  current.front().x = std::numeric_limits<float>::quiet_NaN();

  EXPECT_THROW(static_cast<void>(diffStates(base, current)),
               InvalidCoordinateException);
}

/**
 * Given a list, and a later list with a move, an added entity and a removed
 * one
 * When the difference is applied to the earlier list
 * Then the result is the later list
 */
TEST(WorldStateDelta, ApplyingTheDiffGivesTheLaterList) {
  const std::vector<EntityState> base = someEntities(ENTITIES_IN_A_LIST);
  std::vector<EntityState> current = base;
  current.at(MOVED_ENTITY_ID - 1).y += ONE_EIGHTH;
  current.erase(current.begin() + (VANISHED_ENTITY_ID - 1));
  current.push_back(EntityState{.entityId = NEW_ENTITY_ID,
                                .entityType = EXAMPLE_ENTITY_TYPE,
                                .x = APPLIED_X,
                                .y = APPLIED_Y});

  EXPECT_EQ(applyDelta(base, diffStates(base, current)), current);
}

/**
 * Given a delta whose lists come in the reverse of the order of the ids
 * When it is applied
 * Then the result is the same as with the lists in order
 */
TEST(WorldStateDelta, ApplyingAnUnsortedDeltaGivesTheSameList) {
  const std::vector<EntityState> base = someEntities(ENTITIES_IN_A_LIST);
  std::vector<EntityState> current = base;
  current.at(MOVED_ENTITY_ID - 1).x += ONE_EIGHTH;
  current.erase(current.begin() + (VANISHED_ENTITY_ID - 1));
  StateDelta delta = diffStates(base, current);
  delta.changed.push_back(EntityState{.entityId = NEW_ENTITY_ID,
                                      .entityType = EXAMPLE_ENTITY_TYPE,
                                      .x = 0.0F,
                                      .y = 0.0F});
  const std::vector<EntityState> fromSorted = applyDelta(base, delta);
  std::ranges::reverse(delta.changed);
  std::ranges::reverse(delta.removed);

  EXPECT_EQ(applyDelta(base, delta), fromSorted);
}

/**
 * Given an empty earlier list and a delta without reference, which lists every
 * entity
 * When it is applied
 * Then the result is the listed entities
 */
TEST(WorldStateDelta, ApplyingToNothingGivesTheChangedEntities) {
  const std::vector<EntityState> entities = someEntities(ENTITIES_IN_A_LIST);

  EXPECT_EQ(applyDelta({}, StateDelta{.changed = entities, .removed = {}}),
            entities);
}

/**
 * Given a delta that lists one id twice
 * When it is applied
 * Then the id appears once, with its first entry
 */
TEST(WorldStateDelta, AnIdListedTwiceKeepsItsFirstEntry) {
  const EntityState first{.entityId = NEW_ENTITY_ID,
                          .entityType = EXAMPLE_ENTITY_TYPE,
                          .x = FIRST_DUPLICATE_X,
                          .y = FIRST_DUPLICATE_X};
  EntityState second = first;
  second.x = SECOND_DUPLICATE_X;

  const std::vector<EntityState> applied =
      applyDelta({}, StateDelta{.changed = {first, second}, .removed = {}});

  EXPECT_EQ(applied, std::vector<EntityState>{first});
}
