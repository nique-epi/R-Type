#include <gtest/gtest.h>
#include "CollisionFiltering.hpp"
#include "CollisionFilteringTestConstants.hpp"

using rtype::engine::collisionLayerOf;
using rtype::engine::hitsLayerOf;
using rtype::engine::mayCollide;

/**
 * Given the index of the first collision layer
 * When its set is asked for
 * Then it is the lowest bit
 */
TEST(CollisionFiltering, TheFirstLayerIsTheLowestBit) {
  EXPECT_EQ(collisionLayerOf(FIRST_LAYER_INDEX), FIRST_LAYER_SET);
}

/**
 * Given the index of the last collision layer a filter tells apart
 * When its set is asked for
 * Then it is the highest of the 32 bits
 */
TEST(CollisionFiltering, TheLastLayerIsTheHighestBit) {
  EXPECT_EQ(collisionLayerOf(LAST_LAYER_INDEX), LAST_LAYER_SET);
}

/**
 * Given a shot that hits the rocks, and a rock
 * When the shot is asked whether it hits the layer of the rock
 * Then it does
 */
TEST(CollisionFiltering, AStrikerHitsALayerItNames) {
  EXPECT_TRUE(hitsLayerOf(SHOT_HITTING_ROCKS, ROCK));
}

/**
 * Given a shot that hits the rocks, and a rock that hits nothing
 * When the rock is asked whether it hits the layer of the shot
 * Then it does not: hitting goes one way
 */
TEST(CollisionFiltering, TheStruckDoesNotHitBack) {
  EXPECT_FALSE(hitsLayerOf(ROCK, SHOT_HITTING_ROCKS));
}

/**
 * Given a shot that hits the rocks, and dust
 * When the shot is asked whether it hits the layer of the dust
 * Then it does not
 */
TEST(CollisionFiltering, AStrikerMissesALayerItDoesNotName) {
  EXPECT_FALSE(hitsLayerOf(SHOT_HITTING_ROCKS, DUST));
}

/**
 * Given a shot that hits the rocks, and a rock that hits nothing
 * When the pair is tested, the shot first
 * Then the two may collide
 */
TEST(CollisionFiltering, APairMayCollideWhenTheFirstHitsTheSecond) {
  EXPECT_TRUE(mayCollide(SHOT_HITTING_ROCKS, ROCK));
}

/**
 * Given a shot that hits the rocks, and a rock that hits nothing
 * When the pair is tested, the rock first
 * Then the two may collide: one of them naming the other is enough
 */
TEST(CollisionFiltering, APairMayCollideWhenTheSecondHitsTheFirst) {
  EXPECT_TRUE(mayCollide(ROCK, SHOT_HITTING_ROCKS));
}

/**
 * Given a rock and dust, neither of which hits anything
 * When the pair is tested
 * Then the two never collide
 */
TEST(CollisionFiltering, APairNeitherOfWhichHitsTheOtherNeverCollides) {
  EXPECT_FALSE(mayCollide(ROCK, DUST));
}

/**
 * Given a shot that hits the rocks, and an entity that lies in no layer
 * When the pair is tested
 * Then the two never collide
 */
TEST(CollisionFiltering, AnEntityThatLiesNowhereIsNeverHit) {
  EXPECT_FALSE(mayCollide(SHOT_HITTING_ROCKS, NOWHERE));
}
