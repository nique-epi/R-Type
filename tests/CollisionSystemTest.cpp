#include <gtest/gtest.h>
#include <vector>
#include "Collision.hpp"
#include "CollisionBox.hpp"
#include "CollisionScene.hpp"
#include "CollisionSystem.hpp"
#include "CollisionSystemFixtures.hpp"
#include "CollisionTestConstants.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"

using rtype::engine::Collision;
using rtype::engine::Entity;
using rtype::game::CollisionBox;
using rtype::game::CollisionSystem;
using rtype::game::EntityType;
using rtype::game::testing::updateAndCollect;

/**
 * Given two entities whose collision boxes overlap
 * When the collision system updates and the bus dispatches
 * Then exactly one Collision names that pair
 */
TEST(CollisionSystem, ReportsAnOverlappingPairOnce) {
  CollisionScene scene;
  const Entity first = scene.spawn(EntityType::Player, 0.0F, 0.0F);
  const Entity second =
      scene.spawn(EntityType::Enemy, CENTER_DISTANCE_OVERLAPPING, 0.0F);
  CollisionSystem system(scene.events);

  const std::vector<Collision> received = updateAndCollect(scene, system);

  ASSERT_EQ(received.size(), 1U);
  EXPECT_TRUE(isPair(received.front(), first, second));
}

/**
 * Given two entities whose collision boxes are apart
 * When the collision system updates and the bus dispatches
 * Then no Collision is delivered
 */
TEST(CollisionSystem, ReportsNothingForEntitiesApart) {
  CollisionScene scene;
  scene.spawn(EntityType::Player, 0.0F, 0.0F);
  scene.spawn(EntityType::Enemy, CENTER_DISTANCE_APART, 0.0F);
  CollisionSystem system(scene.events);

  EXPECT_TRUE(updateAndCollect(scene, system).empty());
}

/**
 * Given two entities whose collision boxes only touch along an edge
 * When the collision system updates and the bus dispatches
 * Then no Collision is delivered
 */
TEST(CollisionSystem, ReportsNothingForBoxesOnlyTouching) {
  CollisionScene scene;
  scene.spawn(EntityType::Player, 0.0F, 0.0F);
  scene.spawn(EntityType::Enemy, CENTER_DISTANCE_TOUCHING, 0.0F);
  CollisionSystem system(scene.events);

  EXPECT_TRUE(updateAndCollect(scene, system).empty());
}

/**
 * Given three entities, two of which overlap and one far away
 * When the collision system updates and the bus dispatches
 * Then the only Collision names the two that overlap
 */
TEST(CollisionSystem, ReportsOnlyThePairThatOverlaps) {
  CollisionScene scene;
  const Entity first = scene.spawn(EntityType::Player, 0.0F, 0.0F);
  const Entity second =
      scene.spawn(EntityType::Enemy, CENTER_DISTANCE_OVERLAPPING, 0.0F);
  scene.spawn(EntityType::Enemy, FAR_DISTANCE, 0.0F);
  CollisionSystem system(scene.events);

  const std::vector<Collision> received = updateAndCollect(scene, system);

  ASSERT_EQ(received.size(), 1U);
  EXPECT_TRUE(isPair(received.front(), first, second));
}

/**
 * Given three entities that all overlap each other
 * When the collision system updates and the bus dispatches
 * Then three Collisions are delivered, one per pair
 */
TEST(CollisionSystem, ReportsEachPairOfThreeOverlappingEntitiesOnce) {
  CollisionScene scene;
  const Entity leftmost = scene.spawn(EntityType::Player, 0.0F, 0.0F);
  const Entity middle = scene.spawn(EntityType::Enemy, 1.0F, 0.0F);
  const Entity rightmost = scene.spawn(EntityType::Enemy, 2.0F, 0.0F);
  CollisionSystem system(scene.events);

  const std::vector<Collision> received = updateAndCollect(scene, system);

  ASSERT_EQ(received.size(), 3U);
  int leftmostMiddle = 0;
  int leftmostRightmost = 0;
  int middleRightmost = 0;
  for (const Collision& collision : received) {
    leftmostMiddle += isPair(collision, leftmost, middle) ? 1 : 0;
    leftmostRightmost += isPair(collision, leftmost, rightmost) ? 1 : 0;
    middleRightmost += isPair(collision, middle, rightmost) ? 1 : 0;
  }
  EXPECT_EQ(leftmostMiddle, 1);
  EXPECT_EQ(leftmostRightmost, 1);
  EXPECT_EQ(middleRightmost, 1);
}

/**
 * Given an entity with a position and no collision box standing on another
 * When the collision system updates and the bus dispatches
 * Then no Collision is delivered
 */
TEST(CollisionSystem, IgnoresAnEntityWithoutACollisionBox) {
  CollisionScene scene;
  scene.spawn(EntityType::Player, 0.0F, 0.0F);
  scene.spawnWithoutCollisionBox(EntityType::Enemy, 0.0F, 0.0F);
  CollisionSystem system(scene.events);

  EXPECT_TRUE(updateAndCollect(scene, system).empty());
}

/**
 * Given a registry with no entity
 * When the collision system updates and the bus dispatches
 * Then no Collision is delivered
 */
TEST(CollisionSystem, ReportsNothingWithoutEntities) {
  CollisionScene scene;
  CollisionSystem system(scene.events);

  EXPECT_TRUE(updateAndCollect(scene, system).empty());
}

/**
 * Given two overlapping entities and a system that already updated once
 * When it updates a second time
 * Then the pair is reported again, once
 */
TEST(CollisionSystem, ReportsTheSamePairAgainOnTheNextUpdate) {
  CollisionScene scene;
  scene.spawn(EntityType::Player, 0.0F, 0.0F);
  scene.spawn(EntityType::Enemy, CENTER_DISTANCE_OVERLAPPING, 0.0F);
  CollisionSystem system(scene.events);
  updateAndCollect(scene, system);

  EXPECT_EQ(updateAndCollect(scene, system).size(), 1U);
}

/**
 * Given a square whose box is centered three sides to the right of its
 * Position, and a square on the center of that box
 * When the collision system updates and the bus dispatches
 * Then the pair is reported: a box collides where it is laid
 */
TEST(CollisionSystem, AMovedHitboxCollidesWhereItIsMoved) {
  CollisionScene scene;
  const Entity moved =
      scene.spawnWithoutCollisionBox(EntityType::Enemy, 0.0F, 0.0F);
  scene.components.add(moved, CollisionBox{.width = SQUARE_SIDE,
                                           .height = SQUARE_SIDE,
                                           .offsetX = MOVED_BOX_OFFSET_X});
  const Entity other =
      scene.spawn(EntityType::PlayerMissile, MOVED_BOX_OFFSET_X, 0.0F);
  CollisionSystem system(scene.events);

  const std::vector<Collision> received = updateAndCollect(scene, system);

  ASSERT_EQ(received.size(), 1U);
  EXPECT_TRUE(isPair(received.front(), moved, other));
}

/**
 * Given a square whose box is centered three sides to the right of its
 * Position, and a square on that Position
 * When the collision system updates and the bus dispatches
 * Then nothing is reported: the Position itself does not collide
 */
TEST(CollisionSystem, AMovedHitboxIsApartFromItsPosition) {
  CollisionScene scene;
  const Entity moved =
      scene.spawnWithoutCollisionBox(EntityType::Enemy, 0.0F, 0.0F);
  scene.components.add(moved, CollisionBox{.width = SQUARE_SIDE,
                                           .height = SQUARE_SIDE,
                                           .offsetX = MOVED_BOX_OFFSET_X});
  scene.spawn(EntityType::PlayerMissile, 0.0F, 0.0F);
  CollisionSystem system(scene.events);

  EXPECT_TRUE(updateAndCollect(scene, system).empty());
}

/**
 * Given two entities whose boxes, each wider than two cells of the collision
 * grid, overlap and share several cells
 * When the collision system updates and the bus dispatches
 * Then exactly one Collision names that pair
 */
TEST(CollisionSystem, ReportsAPairSharingSeveralCellsOnce) {
  CollisionScene scene;
  const Entity first = scene.spawnWithoutCollisionBox(
      EntityType::Player, WIDE_BOX_CENTER_X, WIDE_BOX_CENTER_Y);
  scene.components.add(
      first, CollisionBox{.width = WIDE_BOX_SIDE, .height = WIDE_BOX_SIDE});
  const Entity second = scene.spawnWithoutCollisionBox(
      EntityType::Enemy, OTHER_WIDE_BOX_CENTER_X, WIDE_BOX_CENTER_Y);
  scene.components.add(
      second, CollisionBox{.width = WIDE_BOX_SIDE, .height = WIDE_BOX_SIDE});
  CollisionSystem system(scene.events);

  const std::vector<Collision> received = updateAndCollect(scene, system);

  ASSERT_EQ(received.size(), 1U);
  EXPECT_TRUE(isPair(received.front(), first, second));
}

/**
 * Given two entities beyond the left and the top edges of the playfield whose
 * boxes overlap
 * When the collision system updates and the bus dispatches
 * Then exactly one Collision names that pair
 */
TEST(CollisionSystem, ReportsAnOverlappingPairBeyondThePlayfield) {
  CollisionScene scene;
  const Entity first = scene.spawn(EntityType::Player, BEYOND_THE_PLAYFIELD,
                                   BEYOND_THE_PLAYFIELD);
  const Entity second = scene.spawn(
      EntityType::Enemy, BEYOND_THE_PLAYFIELD + CENTER_DISTANCE_OVERLAPPING,
      BEYOND_THE_PLAYFIELD);
  CollisionSystem system(scene.events);

  const std::vector<Collision> received = updateAndCollect(scene, system);

  ASSERT_EQ(received.size(), 1U);
  EXPECT_TRUE(isPair(received.front(), first, second));
}
