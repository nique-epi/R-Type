#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "EntityAnnouncements.hpp"
#include "EntityIdConstants.hpp"
#include "EntityReplicatorFixture.hpp"
#include "EntityReplicatorTestConstants.hpp"
#include "EntityType.hpp"
#include "WorldState.hpp"
#include "WorldStateFilterTestConstants.hpp"

using rtype::client::testing::APPLIED_TICK;
using rtype::client::testing::BYDO_ID;
using rtype::client::testing::BYDO_POSITION;
using rtype::client::testing::drawnAs;
using rtype::client::testing::ENEMY_MISSILE_ID;
using rtype::client::testing::ENEMY_MISSILE_POSITION;
using rtype::client::testing::EntityReplicatorFixture;
using rtype::client::testing::FIRST_POSITION;
using rtype::client::testing::killed;
using rtype::client::testing::listed;
using rtype::client::testing::OLDER_TICK;
using rtype::client::testing::PLAYER_MISSILE_ID;
using rtype::client::testing::PLAYER_MISSILE_POSITION;
using rtype::client::testing::SECOND_POSITION;
using rtype::client::testing::SHIP_ID;
using rtype::client::testing::SHIP_POSITION;
using rtype::client::testing::spawned;
using rtype::client::testing::stateListing;
using rtype::client::testing::UNANNOUNCED_ID;
using rtype::client::testing::WIRE_TYPE_AFTER_THE_LAST;
using rtype::client::testing::WIRE_TYPE_BEFORE_THE_FIRST;
using rtype::game::EntityType;
using rtype::identifiers::NO_ENTITY_ID;
using rtype::network::WireEntityType;
using ::testing::ElementsAre;
using ::testing::UnorderedElementsAre;

/**
 * Given a replicator that has applied nothing
 * When an EntitySpawned announces a Bydo
 * Then one entity is drawn, at the announced position, with the sprite of an
 * enemy
 */
TEST_F(EntityReplicatorFixture, DrawsASpawnedEntityWhereTheServerPutsIt) {
  replicator().apply(spawned(BYDO_ID, WireEntityType::Bydo, FIRST_POSITION));

  EXPECT_THAT(drawnEntities(),
              ElementsAre(drawnAs(EntityType::Enemy, FIRST_POSITION)));
}

/**
 * Given a replicator that has applied nothing
 * When one entity of each type of the protocol is announced
 * Then each is drawn with the sprite of its game type, as the entity type
 * table of the protocol page pairs them: PlayerShip a Player, PlayerMissile a
 * PlayerMissile, Bydo an Enemy, EnemyMissile an EnemyMissile
 */
TEST_F(EntityReplicatorFixture, GivesEachTypeOfTheProtocolItsSprite) {
  replicator().apply(
      spawned(SHIP_ID, WireEntityType::PlayerShip, SHIP_POSITION));
  replicator().apply(spawned(PLAYER_MISSILE_ID, WireEntityType::PlayerMissile,
                             PLAYER_MISSILE_POSITION));
  replicator().apply(spawned(BYDO_ID, WireEntityType::Bydo, BYDO_POSITION));
  replicator().apply(spawned(ENEMY_MISSILE_ID, WireEntityType::EnemyMissile,
                             ENEMY_MISSILE_POSITION));

  EXPECT_THAT(drawnEntities(),
              UnorderedElementsAre(
                  drawnAs(EntityType::Player, SHIP_POSITION),
                  drawnAs(EntityType::PlayerMissile, PLAYER_MISSILE_POSITION),
                  drawnAs(EntityType::Enemy, BYDO_POSITION),
                  drawnAs(EntityType::EnemyMissile, ENEMY_MISSILE_POSITION)));
}

/**
 * Given a ship the client already has
 * When an EntitySpawned announces it again, elsewhere
 * Then nothing changes: one ship, where it first appeared
 */
TEST_F(EntityReplicatorFixture, IgnoresASpawnNamingAnEntityItHas) {
  replicator().apply(
      spawned(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION));

  replicator().apply(
      spawned(SHIP_ID, WireEntityType::PlayerShip, SECOND_POSITION));

  EXPECT_THAT(drawnEntities(),
              ElementsAre(drawnAs(EntityType::Player, FIRST_POSITION)));
}

/**
 * Given a replicator that has applied nothing, as after a lost EntitySpawned
 * When a WorldState lists a ship
 * Then the ship is created, drawn at the listed position
 */
TEST_F(EntityReplicatorFixture, CreatesAnEntityAStateListsAndItDoesNotHave) {
  replicator().apply(stateListing(
      APPLIED_TICK,
      {listed(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION)}));

  EXPECT_THAT(drawnEntities(),
              ElementsAre(drawnAs(EntityType::Player, FIRST_POSITION)));
}

/**
 * Given a ship the client has
 * When a WorldState lists it elsewhere
 * Then the same ship is drawn at the new position, and no second one appears
 */
TEST_F(EntityReplicatorFixture, MovesAnEntityItHasWhereAStatePutsIt) {
  replicator().apply(
      spawned(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION));

  replicator().apply(stateListing(
      APPLIED_TICK,
      {listed(SHIP_ID, WireEntityType::PlayerShip, SECOND_POSITION)}));

  EXPECT_THAT(drawnEntities(),
              ElementsAre(drawnAs(EntityType::Player, SECOND_POSITION)));
}

/**
 * Given a ship moved by a WorldState
 * When an older WorldState arrives late, listing the ship where it was before
 * Then the ship stays where the newer state put it
 */
TEST_F(EntityReplicatorFixture, DoesNotMoveAnEntityBackForAnOlderState) {
  replicator().apply(stateListing(
      APPLIED_TICK,
      {listed(SHIP_ID, WireEntityType::PlayerShip, SECOND_POSITION)}));

  replicator().apply(stateListing(
      OLDER_TICK,
      {listed(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION)}));

  EXPECT_THAT(drawnEntities(),
              ElementsAre(drawnAs(EntityType::Player, SECOND_POSITION)));
}

/**
 * Given a WorldState listing a ship has been applied
 * When an older WorldState arrives late, listing a Bydo
 * Then no Bydo is created: an old state creates nothing either
 */
TEST_F(EntityReplicatorFixture, CreatesNothingForAnOlderState) {
  replicator().apply(stateListing(
      APPLIED_TICK,
      {listed(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION)}));

  replicator().apply(stateListing(
      OLDER_TICK, {listed(BYDO_ID, WireEntityType::Bydo, SECOND_POSITION)}));

  EXPECT_THAT(drawnEntities(),
              ElementsAre(drawnAs(EntityType::Player, FIRST_POSITION)));
}

/**
 * Given a WorldState too big for one datagram, sent as two with the same tick
 * When both parts are applied, the ship in the first and the Bydo in the
 * second
 * Then both entities are drawn
 */
TEST_F(EntityReplicatorFixture, AppliesEveryPartOfAStateSentInSeveralParts) {
  replicator().apply(stateListing(
      APPLIED_TICK,
      {listed(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION)}));

  replicator().apply(stateListing(
      APPLIED_TICK, {listed(BYDO_ID, WireEntityType::Bydo, SECOND_POSITION)}));

  EXPECT_THAT(
      drawnEntities(),
      UnorderedElementsAre(drawnAs(EntityType::Player, FIRST_POSITION),
                           drawnAs(EntityType::Enemy, SECOND_POSITION)));
}

/**
 * Given a replicator that has applied nothing
 * When a WorldState lists the same ship twice, at two positions
 * Then one ship is drawn, at the second position
 */
TEST_F(EntityReplicatorFixture, CreatesAnEntityListedTwiceInOneStateOnce) {
  replicator().apply(stateListing(
      APPLIED_TICK,
      {listed(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION),
       listed(SHIP_ID, WireEntityType::PlayerShip, SECOND_POSITION)}));

  EXPECT_THAT(drawnEntities(),
              ElementsAre(drawnAs(EntityType::Player, SECOND_POSITION)));
}

/**
 * Given a ship the client has
 * When an EntityDestroyed names it
 * Then the entity no longer exists in the registry
 */
TEST_F(EntityReplicatorFixture, DestroysTheEntityAnEntityDestroyedNames) {
  replicator().apply(
      spawned(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION));

  replicator().apply(killed(SHIP_ID));

  EXPECT_EQ(entityCount(), 0U);
}

/**
 * Given a ship the client has, the first entity of the registry
 * When an EntityDestroyed names an entity no message announced
 * Then the ship is still drawn: an unknown id never falls back on another
 * entity
 */
TEST_F(EntityReplicatorFixture,
       IgnoresADestructionNamingAnEntityItDoesNotHave) {
  replicator().apply(
      spawned(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION));

  replicator().apply(killed(UNANNOUNCED_ID));

  EXPECT_THAT(drawnEntities(),
              ElementsAre(drawnAs(EntityType::Player, FIRST_POSITION)));
}

/**
 * Given a ship destroyed by the first copy of its EntityDestroyed
 * When a WorldState sent before the destruction arrives late, listing it
 * Then the ship is created again, as the protocol page asks for an entity the
 * client does not have; the next copy of the EntityDestroyed removes it
 */
TEST_F(EntityReplicatorFixture, RecreatesADestroyedEntityALateStateLists) {
  replicator().apply(
      spawned(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION));
  replicator().apply(killed(SHIP_ID));

  replicator().apply(stateListing(
      APPLIED_TICK,
      {listed(SHIP_ID, WireEntityType::PlayerShip, SECOND_POSITION)}));

  EXPECT_THAT(drawnEntities(),
              ElementsAre(drawnAs(EntityType::Player, SECOND_POSITION)));
}

/**
 * Given a replicator that has applied nothing
 * When an EntitySpawned names the entity id no entity has
 * Then no entity is created
 */
TEST_F(EntityReplicatorFixture, CreatesNothingForTheReservedEntityId) {
  replicator().apply(
      spawned(NO_ENTITY_ID, WireEntityType::PlayerShip, FIRST_POSITION));

  EXPECT_EQ(entityCount(), 0U);
}

/**
 * Given a replicator that has applied nothing
 * When EntitySpawned messages carry the values just outside the entity types
 * of the protocol
 * Then no entity is created
 */
TEST_F(EntityReplicatorFixture, CreatesNothingForATypeOutsideTheProtocol) {
  replicator().apply(
      spawned(SHIP_ID, WIRE_TYPE_BEFORE_THE_FIRST, FIRST_POSITION));
  replicator().apply(
      spawned(BYDO_ID, WIRE_TYPE_AFTER_THE_LAST, FIRST_POSITION));

  EXPECT_EQ(entityCount(), 0U);
}
