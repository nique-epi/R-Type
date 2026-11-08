#include <gtest/gtest.h>
#include <vector>
#include "CollisionScene.hpp"
#include "CurrentAction.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "HurtZoneBeamTestConstants.hpp"
#include "HurtZoneSystemFixtures.hpp"
#include "ShipContact.hpp"

using rtype::engine::Entity;
using rtype::game::CurrentAction;
using rtype::game::EntityType;
using rtype::game::ShipContact;
using rtype::game::testing::BEAM_SIGNAL;
using rtype::game::testing::BEAM_STRIKE;
using rtype::game::testing::contactsOf;
using rtype::game::testing::EARLY_IN_THE_SWEEP;
using rtype::game::testing::enemyWithBeam;
using rtype::game::testing::HALFWAY_THROUGH_THE_SWEEP;
using rtype::game::testing::SHIP_ABOVE_THE_BEAM_Y;
using rtype::game::testing::SHIP_HALFWAY_DOWN_THE_SWEEP_X;
using rtype::game::testing::SHIP_HALFWAY_DOWN_THE_SWEEP_Y;
using rtype::game::testing::SHIP_ON_THE_BEAM_X;
using rtype::game::testing::SHIP_PAST_THE_BEAM_X;
using rtype::game::testing::SHIP_TOUCHING_THE_END_X;
using rtype::game::testing::STRAIGHT_BEAM;
using rtype::game::testing::SWEEP_STRIKE;
using rtype::game::testing::SWEEPING_BEAM;

/**
 * Given an enemy in the strike of its straight beam and a ship on the beam
 * When the hurt zone system updates and the bus dispatches
 * Then one ShipContact names the ship and the enemy
 */
TEST(HurtZoneSystem, AStraightBeamHurtsAShipOnItsLine) {
  CollisionScene scene;
  const Entity enemy =
      enemyWithBeam(scene, STRAIGHT_BEAM, CurrentAction{.key = BEAM_STRIKE});
  const Entity ship = scene.spawn(EntityType::Player, SHIP_ON_THE_BEAM_X, 0.0F);

  const std::vector<ShipContact> received = contactsOf(scene);

  ASSERT_EQ(received.size(), 1U);
  EXPECT_EQ(received.front().player, ship);
  EXPECT_EQ(received.front().enemy, enemy);
}

/**
 * Given an enemy in the strike of its straight beam and a ship above the beam
 * When the hurt zone system updates and the bus dispatches
 * Then no ShipContact is delivered
 */
TEST(HurtZoneSystem, AStraightBeamSparesAShipAboveIt) {
  CollisionScene scene;
  enemyWithBeam(scene, STRAIGHT_BEAM, CurrentAction{.key = BEAM_STRIKE});
  scene.spawn(EntityType::Player, SHIP_ON_THE_BEAM_X, SHIP_ABOVE_THE_BEAM_Y);

  EXPECT_TRUE(contactsOf(scene).empty());
}

/**
 * Given an enemy in the signal of the attack of its beam and a ship on the
 * beam
 * When the hurt zone system updates and the bus dispatches
 * Then no ShipContact is delivered: the beam hurts in its strike only
 */
TEST(HurtZoneSystem, ABeamHurtsOnlyInItsAction) {
  CollisionScene scene;
  enemyWithBeam(scene, STRAIGHT_BEAM, CurrentAction{.key = BEAM_SIGNAL});
  scene.spawn(EntityType::Player, SHIP_ON_THE_BEAM_X, 0.0F);

  EXPECT_TRUE(contactsOf(scene).empty());
}

/**
 * Given an enemy 100 ms into a beam that sweeps down over a second, and a
 * ship on the line the beam takes halfway through
 * When the hurt zone system updates and the bus dispatches
 * Then no ShipContact is delivered: the beam has not reached the ship yet
 */
TEST(HurtZoneSystem, ASweepingBeamSparesAShipItHasNotReachedYet) {
  CollisionScene scene;
  enemyWithBeam(
      scene, SWEEPING_BEAM,
      CurrentAction{.key = SWEEP_STRIKE, .since = EARLY_IN_THE_SWEEP});
  scene.spawn(EntityType::Player, SHIP_HALFWAY_DOWN_THE_SWEEP_X,
              SHIP_HALFWAY_DOWN_THE_SWEEP_Y);

  EXPECT_TRUE(contactsOf(scene).empty());
}

/**
 * Given an enemy halfway through a beam that sweeps down over a second, and a
 * ship 45 degrees down and left of it
 * When the hurt zone system updates and the bus dispatches
 * Then the ship is hurt: the beam has turned halfway, onto it
 */
TEST(HurtZoneSystem, ASweepingBeamHurtsAShipHalfwayThroughItsSweep) {
  CollisionScene scene;
  enemyWithBeam(
      scene, SWEEPING_BEAM,
      CurrentAction{.key = SWEEP_STRIKE, .since = HALFWAY_THROUGH_THE_SWEEP});
  scene.spawn(EntityType::Player, SHIP_HALFWAY_DOWN_THE_SWEEP_X,
              SHIP_HALFWAY_DOWN_THE_SWEEP_Y);

  EXPECT_EQ(contactsOf(scene).size(), 1U);
}

/**
 * Given an enemy in the strike of its straight beam and a ship on its line
 * past its end
 * When the hurt zone system updates and the bus dispatches
 * Then no ShipContact is delivered
 */
TEST(HurtZoneSystem, ABeamSparesAShipPastItsLength) {
  CollisionScene scene;
  enemyWithBeam(scene, STRAIGHT_BEAM, CurrentAction{.key = BEAM_STRIKE});
  scene.spawn(EntityType::Player, SHIP_PAST_THE_BEAM_X, 0.0F);

  EXPECT_TRUE(contactsOf(scene).empty());
}

/**
 * Given an enemy in the strike of its straight beam and a ship whose edge
 * only touches the end of the beam
 * When the hurt zone system updates and the bus dispatches
 * Then no ShipContact is delivered: sharing an edge is not overlapping
 */
TEST(HurtZoneSystem, ABeamThatOnlyTouchesAShipSparesIt) {
  CollisionScene scene;
  enemyWithBeam(scene, STRAIGHT_BEAM, CurrentAction{.key = BEAM_STRIKE});
  scene.spawn(EntityType::Player, SHIP_TOUCHING_THE_END_X, 0.0F);

  EXPECT_TRUE(contactsOf(scene).empty());
}
