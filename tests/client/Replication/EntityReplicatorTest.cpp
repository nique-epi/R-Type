#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <vector>
#include "AllEntityTypes.hpp"
#include "EffectConstants.hpp"
#include "EntityAnnouncements.hpp"
#include "EntityIdConstants.hpp"
#include "EntityReplicatorFixture.hpp"
#include "EntityReplicatorFixtures.hpp"
#include "EntityReplicatorTestConstants.hpp"
#include "EntityType.hpp"
#include "PilotTestConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateFilterTestConstants.hpp"

using rtype::client::BYDO_EXPLOSION_FRAME;
using rtype::client::ENEMY_MUZZLE_FLASH_FRAME;
using rtype::client::PLAYER_MUZZLE_FLASH_FRAME;
using rtype::client::SHIP_EXPLOSION_FRAME;
using rtype::client::testing::AFTER_THE_LAST_PLAYER;
using rtype::client::testing::AN_EVENT_NUMBER;
using rtype::client::testing::animatedEntityCount;
using rtype::client::testing::announcedEntityCount;
using rtype::client::testing::APPLIED_TICK;
using rtype::client::testing::BEFORE_THE_FIRST_PLAYER;
using rtype::client::testing::BYDO_ID;
using rtype::client::testing::BYDO_POSITION;
using rtype::client::testing::damaged;
using rtype::client::testing::drawnAs;
using rtype::client::testing::drawnEffects;
using rtype::client::testing::drawnEntities;
using rtype::client::testing::effectAt;
using rtype::client::testing::ENEMY_MISSILE_ID;
using rtype::client::testing::ENEMY_MISSILE_POSITION;
using rtype::client::testing::EntityReplicatorFixture;
using rtype::client::testing::FIRST_POSITION;
using rtype::client::testing::frozenPictureCount;
using rtype::client::testing::isArriving;
using rtype::client::testing::isFlashed;
using rtype::client::testing::isInvulnerable;
using rtype::client::testing::killed;
using rtype::client::testing::listed;
using rtype::client::testing::NEWER_TICK;
using rtype::client::testing::OLDER_TICK;
using rtype::client::testing::pilotColorOfThe;
using rtype::client::testing::PLAYER_MISSILE_ID;
using rtype::client::testing::PLAYER_MISSILE_POSITION;
using rtype::client::testing::playerFlying;
using rtype::client::testing::playerHit;
using rtype::client::testing::removed;
using rtype::client::testing::SECOND_PLAYER_NUMBER;
using rtype::client::testing::SECOND_POSITION;
using rtype::client::testing::secondPilotFlying;
using rtype::client::testing::SHIP_ID;
using rtype::client::testing::SHIP_POSITION;
using rtype::client::testing::spawned;
using rtype::client::testing::speedLineColors;
using rtype::client::testing::stateListing;
using rtype::client::testing::stateListingPlayers;
using rtype::client::testing::UNANNOUNCED_ID;
using rtype::client::testing::WIRE_TYPE_AFTER_THE_LAST;
using rtype::client::testing::WIRE_TYPE_BEFORE_THE_FIRST;
using rtype::game::EntityType;
using rtype::identifiers::NO_ENTITY_ID;
using rtype::network::WireEntityType;
using rtype::network::WorldState;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::UnorderedElementsAre;

/**
 * Given a replicator that has applied nothing
 * When an EntitySpawned announces a Bydo
 * Then one entity is drawn, at the announced position, with the sprite of an
 * enemy
 */
TEST_F(EntityReplicatorFixture, DrawsASpawnedEntityWhereTheServerPutsIt) {
  replicator().apply(spawned(BYDO_ID, WireEntityType::Bydo, FIRST_POSITION));

  EXPECT_THAT(drawnEntities(components()),
              ElementsAre(drawnAs(EntityType::Enemy, BYDO_ID, FIRST_POSITION)));
}

/**
 * Given a replicator that has applied nothing
 * When one entity of each type of the protocol is announced
 * Then each is drawn with the sprite of its game type, as the entity type
 * table of the protocol page pairs them: PlayerShip a Player, PlayerMissile a
 * PlayerMissile, Bydo an Enemy, EnemyMissile an EnemyMissile
 */
TEST_F(EntityReplicatorFixture, GivesEachTypeOfTheProtocolItsSprite) {
  announceOneOfEachType();

  EXPECT_THAT(
      drawnEntities(components()),
      UnorderedElementsAre(drawnAs(EntityType::Player, SHIP_ID, SHIP_POSITION),
                           drawnAs(EntityType::PlayerMissile, PLAYER_MISSILE_ID,
                                   PLAYER_MISSILE_POSITION),
                           drawnAs(EntityType::Enemy, BYDO_ID, BYDO_POSITION),
                           drawnAs(EntityType::EnemyMissile, ENEMY_MISSILE_ID,
                                   ENEMY_MISSILE_POSITION)));
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

  EXPECT_THAT(
      drawnEntities(components()),
      ElementsAre(drawnAs(EntityType::Player, SHIP_ID, FIRST_POSITION)));
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

  EXPECT_THAT(
      drawnEntities(components()),
      ElementsAre(drawnAs(EntityType::Player, SHIP_ID, FIRST_POSITION)));
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

  EXPECT_THAT(
      drawnEntities(components()),
      ElementsAre(drawnAs(EntityType::Player, SHIP_ID, SECOND_POSITION)));
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

  EXPECT_THAT(
      drawnEntities(components()),
      ElementsAre(drawnAs(EntityType::Player, SHIP_ID, SECOND_POSITION)));
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

  EXPECT_THAT(
      drawnEntities(components()),
      ElementsAre(drawnAs(EntityType::Player, SHIP_ID, FIRST_POSITION)));
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

  EXPECT_THAT(drawnEntities(components()),
              UnorderedElementsAre(
                  drawnAs(EntityType::Player, SHIP_ID, FIRST_POSITION),
                  drawnAs(EntityType::Enemy, BYDO_ID, SECOND_POSITION)));
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

  EXPECT_THAT(
      drawnEntities(components()),
      ElementsAre(drawnAs(EntityType::Player, SHIP_ID, SECOND_POSITION)));
}

/**
 * Given a ship the client has
 * When an EntityDestroyed names it, removed
 * Then the entity no longer exists in the registry
 */
TEST_F(EntityReplicatorFixture, DestroysTheEntityAnEntityDestroyedNames) {
  replicator().apply(
      spawned(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION));

  replicator().apply(removed(SHIP_ID));

  EXPECT_EQ(announcedEntityCount(components()), 0U);
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

  EXPECT_THAT(
      drawnEntities(components()),
      ElementsAre(drawnAs(EntityType::Player, SHIP_ID, FIRST_POSITION)));
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

  EXPECT_THAT(
      drawnEntities(components()),
      ElementsAre(drawnAs(EntityType::Player, SHIP_ID, SECOND_POSITION)));
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

/**
 * Given a replicator that has applied nothing
 * When one entity of each type of the protocol is announced
 * Then every one of them plays an animation
 */
TEST_F(EntityReplicatorFixture, AnimatesEveryEntityItCreates) {
  announceOneOfEachType();

  EXPECT_EQ(animatedEntityCount(components()), ALL_ENTITY_TYPES.size());
}

/**
 * Given a Bydo the client has, moved by a WorldState
 * When an EntityDestroyed says it was killed
 * Then the explosion of a Bydo starts where the Bydo stood last
 */
TEST_F(EntityReplicatorFixture, AKilledEntityLeavesItsExplosionWhereItStood) {
  replicator().apply(spawned(BYDO_ID, WireEntityType::Bydo, FIRST_POSITION));
  replicator().apply(stateListing(
      APPLIED_TICK, {listed(BYDO_ID, WireEntityType::Bydo, SECOND_POSITION)}));

  replicator().apply(killed(BYDO_ID));

  EXPECT_THAT(drawnEffects(components()),
              ElementsAre(effectAt(BYDO_EXPLOSION_FRAME, SECOND_POSITION)));
}

/**
 * Given a Bydo the client has
 * When an EntityDestroyed says it was removed, as past the left edge
 * Then no effect starts: a removal shows nothing
 */
TEST_F(EntityReplicatorFixture, ARemovedEntityLeavesNoEffect) {
  replicator().apply(spawned(BYDO_ID, WireEntityType::Bydo, FIRST_POSITION));

  replicator().apply(removed(BYDO_ID));

  EXPECT_THAT(drawnEffects(components()), IsEmpty());
}

/**
 * Given a Bydo destroyed by the first copy of an EntityDestroyed saying it was
 * killed
 * When the second copy arrives
 * Then one explosion plays, not two
 */
TEST_F(EntityReplicatorFixture, ALaterCopyOfAKillPlaysNoSecondExplosion) {
  replicator().apply(spawned(BYDO_ID, WireEntityType::Bydo, FIRST_POSITION));
  replicator().apply(killed(BYDO_ID));

  replicator().apply(killed(BYDO_ID));

  EXPECT_THAT(drawnEffects(components()),
              ElementsAre(effectAt(BYDO_EXPLOSION_FRAME, FIRST_POSITION)));
}

/**
 * Given a replicator that has applied nothing
 * When an EntitySpawned announces a player missile
 * Then the muzzle flash of a player missile starts where it appears
 */
TEST_F(EntityReplicatorFixture, AMissileItCreatesShowsItsMuzzleFlash) {
  replicator().apply(spawned(PLAYER_MISSILE_ID, WireEntityType::PlayerMissile,
                             FIRST_POSITION));

  EXPECT_THAT(drawnEffects(components()),
              ElementsAre(effectAt(PLAYER_MUZZLE_FLASH_FRAME, FIRST_POSITION)));
}

/**
 * Given a replicator that has applied nothing, as after a lost EntitySpawned
 * When a WorldState lists an enemy missile
 * Then the muzzle flash of an enemy missile starts where it is created, as the
 * protocol page asks
 */
TEST_F(EntityReplicatorFixture, AMissileAStateCreatesShowsItsMuzzleFlashToo) {
  replicator().apply(stateListing(
      APPLIED_TICK, {listed(ENEMY_MISSILE_ID, WireEntityType::EnemyMissile,
                            FIRST_POSITION)}));

  EXPECT_THAT(drawnEffects(components()),
              ElementsAre(effectAt(ENEMY_MUZZLE_FLASH_FRAME, FIRST_POSITION)));
}

/**
 * Given a player missile the client has
 * When a later copy of its EntitySpawned arrives
 * Then one muzzle flash plays, not two
 */
TEST_F(EntityReplicatorFixture, ALaterCopyOfASpawnPlaysNoSecondFlash) {
  replicator().apply(spawned(PLAYER_MISSILE_ID, WireEntityType::PlayerMissile,
                             FIRST_POSITION));

  replicator().apply(spawned(PLAYER_MISSILE_ID, WireEntityType::PlayerMissile,
                             FIRST_POSITION));

  EXPECT_THAT(drawnEffects(components()),
              ElementsAre(effectAt(PLAYER_MUZZLE_FLASH_FRAME, FIRST_POSITION)));
}

/**
 * Given a replicator that has applied nothing
 * When a ship and a Bydo are announced
 * Then no effect starts: a ship and a Bydo appear without one
 */
TEST_F(EntityReplicatorFixture, AShipAndABydoAppearWithoutEffect) {
  replicator().apply(
      spawned(SHIP_ID, WireEntityType::PlayerShip, SHIP_POSITION));
  replicator().apply(spawned(BYDO_ID, WireEntityType::Bydo, BYDO_POSITION));

  EXPECT_THAT(drawnEffects(components()), IsEmpty());
}

/**
 * Given a ship the client has
 * When a WorldState lists player 2 with that ship
 * Then the ship is marked with the second pilot color
 */
TEST_F(EntityReplicatorFixture, MarksTheShipOfEachPlayerWithItsPilotColor) {
  replicator().apply(
      spawned(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION));

  replicator().apply(
      stateListingPlayers(APPLIED_TICK, {secondPilotFlying(false)}));

  EXPECT_EQ(pilotColorOfThe(components(), EntityType::Player),
            SECOND_PILOT_COLOR);
}

/**
 * Given a ship a state lists as invulnerable
 * When a newer state lists it as vulnerable
 * Then it shows it cannot be hit, then no longer
 */
TEST_F(EntityReplicatorFixture, FollowsTheInvulnerabilityTheServerSends) {
  replicator().apply(
      spawned(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION));
  replicator().apply(
      stateListingPlayers(APPLIED_TICK, {secondPilotFlying(true)}));
  const bool invulnerableFirst =
      isInvulnerable(components(), EntityType::Player);

  replicator().apply(
      stateListingPlayers(NEWER_TICK, {secondPilotFlying(false)}));

  EXPECT_TRUE(invulnerableFirst);
  EXPECT_FALSE(isInvulnerable(components(), EntityType::Player));
}

/**
 * Given a ship a state lists as vulnerable
 * When an older state arrives late, listing it invulnerable
 * Then it stays vulnerable: an older state changes nothing
 */
TEST_F(EntityReplicatorFixture, AnOlderStateDoesNotChangeInvulnerability) {
  replicator().apply(
      spawned(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION));
  replicator().apply(
      stateListingPlayers(APPLIED_TICK, {secondPilotFlying(false)}));

  replicator().apply(
      stateListingPlayers(OLDER_TICK, {secondPilotFlying(true)}));

  EXPECT_FALSE(isInvulnerable(components(), EntityType::Player));
}

/**
 * Given a Bydo the client has
 * When an EntityDamaged names it
 * Then it flashes
 */
TEST_F(EntityReplicatorFixture, FlashesTheEntityAnEntityDamagedNames) {
  replicator().apply(spawned(BYDO_ID, WireEntityType::Bydo, FIRST_POSITION));

  replicator().apply(damaged(AN_EVENT_NUMBER, BYDO_ID));

  EXPECT_TRUE(isFlashed(components(), EntityType::Enemy));
}

/**
 * Given a ship the client has, its player listed
 * When a PlayerHit names that player
 * Then nothing flashes: a ship goes down through its EntityDestroyed
 */
TEST_F(EntityReplicatorFixture, APlayerHitPlaysNothing) {
  replicator().apply(
      spawned(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION));
  replicator().apply(
      stateListingPlayers(APPLIED_TICK, {secondPilotFlying(false)}));

  replicator().apply(playerHit(AN_EVENT_NUMBER, SECOND_PLAYER_NUMBER));

  EXPECT_FALSE(isFlashed(components(), EntityType::Player));
}

/**
 * Given a ship the client has
 * When an EntityDestroyed says it was killed
 * Then its fireballs start and its white picture stays: the whole of T4
 */
TEST_F(EntityReplicatorFixture, AKilledShipGoesDownWithItsWholeTier) {
  replicator().apply(
      spawned(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION));

  replicator().apply(killed(SHIP_ID));

  EXPECT_THAT(drawnEffects(components()),
              ElementsAre(effectAt(SHIP_EXPLOSION_FRAME, FIRST_POSITION)));
  EXPECT_EQ(frozenPictureCount(components()), 1U);
}

/**
 * Given a replicator that has applied nothing
 * When a WorldState creates a ship and lists its player
 * Then the ship arrives, behind speed lines in the light of its pilot
 */
TEST_F(EntityReplicatorFixture, AShipAStateCreatesArrivesInItsPilotColor) {
  replicator().apply(
      WorldState{.tick = APPLIED_TICK,
                 .players = {secondPilotFlying(false)},
                 .entities = {listed(SHIP_ID, WireEntityType::PlayerShip,
                                     FIRST_POSITION)}});

  EXPECT_TRUE(isArriving(components(), EntityType::Player));
  EXPECT_THAT(speedLineColors(components()),
              ::testing::Contains(PINK_LIGHT_OF_THE_SECOND_PILOT));
}

/**
 * Given a ship the client has
 * When a WorldState lists the numbers just outside the players with it
 * Then the ship gets no pilot color
 */
TEST_F(EntityReplicatorFixture, IgnoresAPlayerNumberOutsideThePlayers) {
  replicator().apply(
      spawned(SHIP_ID, WireEntityType::PlayerShip, FIRST_POSITION));

  replicator().apply(stateListingPlayers(
      APPLIED_TICK, {playerFlying(BEFORE_THE_FIRST_PLAYER, SHIP_ID),
                     playerFlying(AFTER_THE_LAST_PLAYER, SHIP_ID)}));

  EXPECT_FALSE(pilotColorOfThe(components(), EntityType::Player).has_value());
}

/**
 * Given a Bydo the client has
 * When a WorldState names it as the ship of a player
 * Then it gets no pilot color: only a ship is a pilot's
 */
TEST_F(EntityReplicatorFixture, IgnoresAShipIdThatNamesNoShip) {
  replicator().apply(spawned(BYDO_ID, WireEntityType::Bydo, FIRST_POSITION));

  replicator().apply(stateListingPlayers(
      APPLIED_TICK, {playerFlying(SECOND_PLAYER_NUMBER, BYDO_ID)}));

  EXPECT_FALSE(pilotColorOfThe(components(), EntityType::Enemy).has_value());
}
