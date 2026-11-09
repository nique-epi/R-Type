#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <cstddef>
#include <optional>
#include <vector>
#include "AllEntityTypes.hpp"
#include "AppearanceReference.hpp"
#include "CollisionBox.hpp"
#include "EffectConstants.hpp"
#include "Entity.hpp"
#include "EntityAnnouncements.hpp"
#include "EntityIdConstants.hpp"
#include "EntityReplicatorFixture.hpp"
#include "EntityReplicatorFixtures.hpp"
#include "EntityReplicatorTestConstants.hpp"
#include "EntityType.hpp"
#include "EntityTypeConstants.hpp"
#include "FixedEntityTypeSourceConstants.hpp"
#include "LastListedTicksTestConstants.hpp"
#include "NetworkIdentity.hpp"
#include "PilotTestConstants.hpp"
#include "SoundConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateFilterTestConstants.hpp"

using rtype::client::AppearanceReference;
using rtype::client::BIG_EXPLOSION_SOUND_ID;
using rtype::client::BYDO_EXPLOSION_FRAME;
using rtype::client::HIT_SOUND_ID;
using rtype::client::PLAYER_HIT_SOUND_ID;
using rtype::client::SHIP_EXPLOSION_FRAME;
using rtype::client::SMALL_EXPLOSION_SOUND_ID;
using rtype::client::testing::AFTER_REMOVAL_TICK;
using rtype::client::testing::AFTER_THE_LAST_PLAYER;
using rtype::client::testing::AN_EVENT_NUMBER;
using rtype::client::testing::animatedEntityCount;
using rtype::client::testing::announcedEntityCount;
using rtype::client::testing::ANOTHER_PLAYER;
using rtype::client::testing::APPLIED_TICK;
using rtype::client::testing::BEFORE_THE_FIRST_PLAYER;
using rtype::client::testing::boxOfThe;
using rtype::client::testing::BYDO_BOX;
using rtype::client::testing::BYDO_ID;
using rtype::client::testing::BYDO_POSITION;
using rtype::client::testing::damaged;
using rtype::client::testing::drawnAs;
using rtype::client::testing::drawnEffects;
using rtype::client::testing::drawnEntities;
using rtype::client::testing::DRIFTER_TYPE_NUMBER;
using rtype::client::testing::effectAt;
using rtype::client::testing::ENEMY_MISSILE_ID;
using rtype::client::testing::ENEMY_MISSILE_POSITION;
using rtype::client::testing::ENEMY_MISSILE_TYPE_NUMBER;
using rtype::client::testing::EntityReplicatorFixture;
using rtype::client::testing::FIRST_LISTING_TICK;
using rtype::client::testing::FIRST_POSITION;
using rtype::client::testing::frozenPictureCount;
using rtype::client::testing::HALFWAY_BETWEEN_THE_POSITIONS;
using rtype::client::testing::historyPositionAt;
using rtype::client::testing::isArriving;
using rtype::client::testing::isFlashed;
using rtype::client::testing::isInvulnerable;
using rtype::client::testing::killed;
using rtype::client::testing::LAST_KEPT_TICK;
using rtype::client::testing::listed;
using rtype::client::testing::LISTING_TICK;
using rtype::client::testing::LOOK_ITEM_ID;
using rtype::client::testing::NEWER_TICK;
using rtype::client::testing::NO_HISTORY_POSITION;
using rtype::client::testing::NUMBER_OF_NO_TYPE;
using rtype::client::testing::OLDER_TICK;
using rtype::client::testing::OWN_PLAYER;
using rtype::client::testing::ownMissileSpawned;
using rtype::client::testing::pilotColorOfThe;
using rtype::client::testing::PLAYER_MISSILE_ID;
using rtype::client::testing::PLAYER_MISSILE_POSITION;
using rtype::client::testing::PLAYER_MISSILE_TYPE_NUMBER;
using rtype::client::testing::playerFlying;
using rtype::client::testing::playerHit;
using rtype::client::testing::REMOVAL_TICK;
using rtype::client::testing::removed;
using rtype::client::testing::SECOND_BYDO_ID;
using rtype::client::testing::SECOND_LISTING_TICK;
using rtype::client::testing::SECOND_PLAYER_NUMBER;
using rtype::client::testing::SECOND_POSITION;
using rtype::client::testing::secondPilotFlying;
using rtype::client::testing::SHIP_ID;
using rtype::client::testing::SHIP_POSITION;
using rtype::client::testing::SHIP_TYPE_NUMBER;
using rtype::client::testing::spawned;
using rtype::client::testing::speedLineColors;
using rtype::client::testing::stateListing;
using rtype::client::testing::stateListingPlayers;
using rtype::client::testing::TICK_BETWEEN_THE_LISTINGS;
using rtype::client::testing::UNANNOUNCED_ID;
using rtype::client::testing::WEAVER_TYPE_NUMBER;
using rtype::engine::Entity;
using rtype::game::CollisionBox;
using rtype::game::EntityType;
using rtype::game::NetworkIdentity;
using rtype::identifiers::NO_ENTITY_ID;
using rtype::identifiers::NO_ENTITY_TYPE;
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
  replicator().apply(spawned(BYDO_ID, DRIFTER_TYPE_NUMBER, FIRST_POSITION));

  EXPECT_THAT(drawnEntities(components()),
              ElementsAre(drawnAs(EntityType::Enemy, BYDO_ID, FIRST_POSITION)));
}

/**
 * Given a replicator that has applied nothing
 * When an EntitySpawned announces a Bydo
 * Then the entity carries the network id of the server and the appearance of
 * its type, which EnemyActionPlayer finds it by
 */
TEST_F(EntityReplicatorFixture,
       KeepsTheIdAndTheAppearanceOfTheEntityItCreates) {
  replicator().apply(spawned(BYDO_ID, DRIFTER_TYPE_NUMBER, FIRST_POSITION));

  std::size_t found = 0;
  components().forEach<NetworkIdentity, AppearanceReference>(
      [&found](Entity /*entity*/, const NetworkIdentity& identity,
               const AppearanceReference& look) {
        found += (identity.networkId == BYDO_ID && look.appearance != nullptr)
                     ? 1U
                     : 0U;
      });
  EXPECT_EQ(found, 1U);
}

/**
 * Given a replicator that has applied nothing
 * When one entity of each side is announced
 * Then the ship and the Bydo are drawn with the look of the side the table
 * gives their type, and the missiles are not drawn yet: they await the tick
 * the client shows
 */
TEST_F(EntityReplicatorFixture, DrawsAShipAndABydoAndHoldsTheMissilesBack) {
  announceOneOfEachType();

  EXPECT_THAT(drawnEntities(components()),
              UnorderedElementsAre(
                  drawnAs(EntityType::Player, SHIP_ID, SHIP_POSITION),
                  drawnAs(EntityType::Enemy, BYDO_ID, BYDO_POSITION),
                  drawnAs(EntityType::LookItem, LOOK_ITEM_ID, BYDO_POSITION)));
}

/**
 * Given a replicator that has applied nothing
 * When a Bydo is announced
 * Then it carries the box the table gives its type, 96 by 96
 */
TEST_F(EntityReplicatorFixture, GivesAnEntityTheBoxOfItsType) {
  replicator().apply(spawned(BYDO_ID, DRIFTER_TYPE_NUMBER, BYDO_POSITION));

  const std::optional<CollisionBox> box =
      boxOfThe(components(), EntityType::Enemy);
  ASSERT_TRUE(box.has_value());
  EXPECT_FLOAT_EQ(box.value_or(CollisionBox{}).width, BYDO_BOX.width);
  EXPECT_FLOAT_EQ(box.value_or(CollisionBox{}).height, BYDO_BOX.height);
}

/**
 * Given a ship the client already has
 * When an EntitySpawned announces it again, elsewhere
 * Then nothing changes: one ship, where it first appeared
 */
TEST_F(EntityReplicatorFixture, IgnoresASpawnNamingAnEntityItHas) {
  replicator().apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION));

  replicator().apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, SECOND_POSITION));

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
      APPLIED_TICK, {listed(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION)}));

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
  replicator().apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION));

  replicator().apply(stateListing(
      APPLIED_TICK, {listed(SHIP_ID, SHIP_TYPE_NUMBER, SECOND_POSITION)}));

  EXPECT_THAT(
      drawnEntities(components()),
      ElementsAre(drawnAs(EntityType::Player, SHIP_ID, SECOND_POSITION)));
}

/**
 * Given a ship the client has, listed by a state of tick 10 at the first
 * position and by a state of tick 12 at the second
 * When the history of the ship is asked for tick 11, between the two
 * Then it puts the ship halfway between the two positions: each state
 * recorded its own tick
 */
TEST_F(EntityReplicatorFixture, RecordsThePositionOfAListedEntityAtItsTick) {
  replicator().apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION));
  replicator().apply(stateListing(
      FIRST_LISTING_TICK, {listed(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION)}));
  replicator().apply(
      stateListing(SECOND_LISTING_TICK,
                   {listed(SHIP_ID, SHIP_TYPE_NUMBER, SECOND_POSITION)}));

  const auto position =
      historyPositionAt(components(), TICK_BETWEEN_THE_LISTINGS);

  ASSERT_TRUE(position.has_value());
  EXPECT_FLOAT_EQ(position.value_or(NO_HISTORY_POSITION).x,
                  HALFWAY_BETWEEN_THE_POSITIONS.x);
  EXPECT_FLOAT_EQ(position.value_or(NO_HISTORY_POSITION).y,
                  HALFWAY_BETWEEN_THE_POSITIONS.y);
}

/**
 * Given a client that has no Bydo
 * When a WorldState lists one at the first position
 * Then the Bydo created has that position as the first sample of its history
 */
TEST_F(EntityReplicatorFixture, RecordsTheFirstSampleOfAnEntityAStateCreates) {
  replicator().apply(
      stateListing(FIRST_LISTING_TICK,
                   {listed(BYDO_ID, DRIFTER_TYPE_NUMBER, FIRST_POSITION)}));

  const auto position =
      historyPositionAt(components(), TICK_BETWEEN_THE_LISTINGS);

  ASSERT_TRUE(position.has_value());
  EXPECT_FLOAT_EQ(position.value_or(NO_HISTORY_POSITION).x, FIRST_POSITION.x);
  EXPECT_FLOAT_EQ(position.value_or(NO_HISTORY_POSITION).y, FIRST_POSITION.y);
}

/**
 * Given a ship moved by a WorldState
 * When an older WorldState arrives late, listing the ship where it was before
 * Then the ship stays where the newer state put it
 */
TEST_F(EntityReplicatorFixture, DoesNotMoveAnEntityBackForAnOlderState) {
  replicator().apply(stateListing(
      APPLIED_TICK, {listed(SHIP_ID, SHIP_TYPE_NUMBER, SECOND_POSITION)}));

  replicator().apply(stateListing(
      OLDER_TICK, {listed(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION)}));

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
      APPLIED_TICK, {listed(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION)}));

  replicator().apply(stateListing(
      OLDER_TICK, {listed(BYDO_ID, DRIFTER_TYPE_NUMBER, SECOND_POSITION)}));

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
      APPLIED_TICK, {listed(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION)}));

  replicator().apply(stateListing(
      APPLIED_TICK, {listed(BYDO_ID, DRIFTER_TYPE_NUMBER, SECOND_POSITION)}));

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
      APPLIED_TICK, {listed(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION),
                     listed(SHIP_ID, SHIP_TYPE_NUMBER, SECOND_POSITION)}));

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
  replicator().apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION));

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
  replicator().apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION));

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
  replicator().apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION));
  replicator().apply(killed(SHIP_ID));

  replicator().apply(stateListing(
      APPLIED_TICK, {listed(SHIP_ID, SHIP_TYPE_NUMBER, SECOND_POSITION)}));

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
  replicator().apply(spawned(NO_ENTITY_ID, SHIP_TYPE_NUMBER, FIRST_POSITION));

  EXPECT_EQ(entityCount(), 0U);
}

/**
 * Given a replicator that has applied nothing
 * When EntitySpawned messages carry NO_ENTITY_TYPE and a number past the
 * table of the server
 * Then no entity is created
 */
TEST_F(EntityReplicatorFixture, CreatesNothingForANumberOfNoType) {
  replicator().apply(spawned(SHIP_ID, NO_ENTITY_TYPE, FIRST_POSITION));
  replicator().apply(spawned(BYDO_ID, NUMBER_OF_NO_TYPE, FIRST_POSITION));

  EXPECT_EQ(entityCount(), 0U);
}

/**
 * Given a replicator that has applied nothing
 * When a drifter and a weaver, two sheets of the enemy side, are announced
 * Then both are drawn as enemies: a type takes the side the table gives it
 */
TEST_F(EntityReplicatorFixture, EveryTypeOfASideIsDrawnAsThatSide) {
  replicator().apply(spawned(BYDO_ID, DRIFTER_TYPE_NUMBER, FIRST_POSITION));
  replicator().apply(
      spawned(SECOND_BYDO_ID, WEAVER_TYPE_NUMBER, SECOND_POSITION));

  EXPECT_THAT(drawnEntities(components()),
              UnorderedElementsAre(
                  drawnAs(EntityType::Enemy, BYDO_ID, FIRST_POSITION),
                  drawnAs(EntityType::Enemy, SECOND_BYDO_ID, SECOND_POSITION)));
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
  replicator().apply(spawned(BYDO_ID, DRIFTER_TYPE_NUMBER, FIRST_POSITION));
  replicator().apply(stateListing(
      APPLIED_TICK, {listed(BYDO_ID, DRIFTER_TYPE_NUMBER, SECOND_POSITION)}));

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
  replicator().apply(spawned(BYDO_ID, DRIFTER_TYPE_NUMBER, FIRST_POSITION));

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
  replicator().apply(spawned(BYDO_ID, DRIFTER_TYPE_NUMBER, FIRST_POSITION));
  replicator().apply(killed(BYDO_ID));

  replicator().apply(killed(BYDO_ID));

  EXPECT_THAT(drawnEffects(components()),
              ElementsAre(effectAt(BYDO_EXPLOSION_FRAME, FIRST_POSITION)));
}

/**
 * Given a replicator that has applied nothing
 * When an EntitySpawned announces a player missile
 * Then no muzzle flash starts yet: it starts once the tick the client shows
 * reaches the missile
 */
TEST_F(EntityReplicatorFixture, AMissileItCreatesWaitsToShowItsMuzzleFlash) {
  replicator().apply(
      spawned(PLAYER_MISSILE_ID, PLAYER_MISSILE_TYPE_NUMBER, FIRST_POSITION));

  EXPECT_THAT(drawnEffects(components()), IsEmpty());
}

/**
 * Given a replicator that has applied nothing, as after a lost EntitySpawned
 * When a WorldState lists an enemy missile
 * Then no muzzle flash starts yet either
 */
TEST_F(EntityReplicatorFixture, AMissileAStateCreatesWaitsToShowItsFlashToo) {
  replicator().apply(stateListing(
      APPLIED_TICK,
      {listed(ENEMY_MISSILE_ID, ENEMY_MISSILE_TYPE_NUMBER, FIRST_POSITION)}));

  EXPECT_THAT(drawnEffects(components()), IsEmpty());
}

/**
 * Given a player missile the client has
 * When a later copy of its EntitySpawned arrives
 * Then the client still holds one missile, not two
 */
TEST_F(EntityReplicatorFixture, ALaterCopyOfASpawnCreatesNoSecondMissile) {
  replicator().apply(
      spawned(PLAYER_MISSILE_ID, PLAYER_MISSILE_TYPE_NUMBER, FIRST_POSITION));

  replicator().apply(
      spawned(PLAYER_MISSILE_ID, PLAYER_MISSILE_TYPE_NUMBER, FIRST_POSITION));

  EXPECT_EQ(announcedEntityCount(components()), 1U);
}

/**
 * Given a replicator that has applied nothing
 * When a ship and a Bydo are announced
 * Then no effect starts: a ship and a Bydo appear without one
 */
TEST_F(EntityReplicatorFixture, AShipAndABydoAppearWithoutEffect) {
  replicator().apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, SHIP_POSITION));
  replicator().apply(spawned(BYDO_ID, DRIFTER_TYPE_NUMBER, BYDO_POSITION));

  EXPECT_THAT(drawnEffects(components()), IsEmpty());
}

/**
 * Given a ship the client has
 * When a WorldState lists player 2 with that ship
 * Then the ship is marked with the second pilot color
 */
TEST_F(EntityReplicatorFixture, MarksTheShipOfEachPlayerWithItsPilotColor) {
  replicator().apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION));

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
  replicator().apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION));
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
  replicator().apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION));
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
  replicator().apply(spawned(BYDO_ID, DRIFTER_TYPE_NUMBER, FIRST_POSITION));

  replicator().apply(damaged(AN_EVENT_NUMBER, BYDO_ID));

  EXPECT_TRUE(isFlashed(components(), EntityType::Enemy));
}

/**
 * Given a ship the client has, its player listed
 * When a PlayerHit names that player
 * Then nothing flashes: a ship goes down through its EntityDestroyed
 */
TEST_F(EntityReplicatorFixture, APlayerHitPlaysNothing) {
  replicator().apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION));
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
  replicator().apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION));

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
  replicator().apply(WorldState{
      .tick = APPLIED_TICK,
      .players = {secondPilotFlying(false)},
      .entities = {listed(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION)}});

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
  replicator().apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, FIRST_POSITION));

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
  replicator().apply(spawned(BYDO_ID, DRIFTER_TYPE_NUMBER, FIRST_POSITION));

  replicator().apply(stateListingPlayers(
      APPLIED_TICK, {playerFlying(SECOND_PLAYER_NUMBER, BYDO_ID)}));

  EXPECT_FALSE(pilotColorOfThe(components(), EntityType::Enemy).has_value());
}

/**
 * Given a replicator that has applied nothing
 * When an EntitySpawned announces a missile of a player
 * Then its shot does not play yet: it plays once the tick the client shows
 * reaches the missile
 */
TEST_F(EntityReplicatorFixture, AMissileWaitsToPlayItsShot) {
  replicator().apply(spawned(PLAYER_MISSILE_ID, PLAYER_MISSILE_TYPE_NUMBER,
                             PLAYER_MISSILE_POSITION));

  EXPECT_THAT(sounds().played(), IsEmpty());
}

/**
 * Given a replicator that has applied nothing
 * When a ship and a Bydo are announced
 * Then no sound plays
 */
TEST_F(EntityReplicatorFixture, AShipAndABydoAppearWithoutASound) {
  replicator().apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, SHIP_POSITION));
  replicator().apply(spawned(BYDO_ID, DRIFTER_TYPE_NUMBER, BYDO_POSITION));

  EXPECT_THAT(sounds().played(), IsEmpty());
}

/**
 * Given a WorldState listing a missile of an enemy whose EntitySpawned was
 * lost
 * When it is applied
 * Then its shot does not play yet either
 */
TEST_F(EntityReplicatorFixture, AMissileAWorldStateCreatesWaitsToPlayItsShot) {
  replicator().apply(stateListing(
      APPLIED_TICK, {listed(ENEMY_MISSILE_ID, ENEMY_MISSILE_TYPE_NUMBER,
                            ENEMY_MISSILE_POSITION)}));

  EXPECT_THAT(sounds().played(), IsEmpty());
}

/**
 * Given a missile of an enemy the client holds back, its arrival awaited
 * When an EntityDestroyed says it was killed
 * Then no impact and no sound play: the player never saw it
 */
TEST_F(EntityReplicatorFixture, AMissileKilledBeforeItsArrivalLeavesNothing) {
  replicator().apply(spawned(ENEMY_MISSILE_ID, ENEMY_MISSILE_TYPE_NUMBER,
                             ENEMY_MISSILE_POSITION));

  replicator().apply(killed(ENEMY_MISSILE_ID));

  EXPECT_THAT(drawnEffects(components()), IsEmpty());
}

/**
 * Given a Bydo the client shows
 * When an EntityDestroyed says it was killed
 * Then the small explosion plays
 */
TEST_F(EntityReplicatorFixture, AKilledBydoPlaysTheSmallExplosion) {
  replicator().apply(spawned(BYDO_ID, DRIFTER_TYPE_NUMBER, BYDO_POSITION));

  replicator().apply(killed(BYDO_ID));

  EXPECT_THAT(sounds().played(), ElementsAre(SMALL_EXPLOSION_SOUND_ID));
}

/**
 * Given a ship the client shows
 * When an EntityDestroyed says it was killed
 * Then the big explosion plays
 */
TEST_F(EntityReplicatorFixture, AKilledShipPlaysTheBigExplosion) {
  replicator().apply(spawned(SHIP_ID, SHIP_TYPE_NUMBER, SHIP_POSITION));

  replicator().apply(killed(SHIP_ID));

  EXPECT_THAT(sounds().played(), ElementsAre(BIG_EXPLOSION_SOUND_ID));
}

/**
 * Given a Bydo the client shows
 * When an EntityDestroyed says it was removed
 * Then no sound plays
 */
TEST_F(EntityReplicatorFixture, ARemovedBydoPlaysNoSound) {
  replicator().apply(spawned(BYDO_ID, DRIFTER_TYPE_NUMBER, BYDO_POSITION));

  replicator().apply(removed(BYDO_ID));

  EXPECT_THAT(sounds().played(), IsEmpty());
}

/**
 * Given a Bydo the client shows
 * When an EntityDamaged names it
 * Then the hit plays
 */
TEST_F(EntityReplicatorFixture, ADamagedEntityPlaysTheHit) {
  replicator().apply(spawned(BYDO_ID, DRIFTER_TYPE_NUMBER, BYDO_POSITION));

  replicator().apply(damaged(AN_EVENT_NUMBER, BYDO_ID));

  EXPECT_THAT(sounds().played(), ElementsAre(HIT_SOUND_ID));
}

/**
 * Given a replicator that shows no entity
 * When an EntityDamaged names an entity it does not have
 * Then no sound plays, as no hit shows
 */
TEST_F(EntityReplicatorFixture, ADamagedEntityTheClientDoesNotHaveIsSilent) {
  replicator().apply(damaged(AN_EVENT_NUMBER, UNANNOUNCED_ID));

  EXPECT_THAT(sounds().played(), IsEmpty());
}

/**
 * Given a replicator in a game
 * When a PlayerHit names the second player
 * Then the hit of a player plays
 */
TEST_F(EntityReplicatorFixture, AHitPlayerPlaysTheHitOfAPlayer) {
  replicator().apply(playerHit(AN_EVENT_NUMBER, SECOND_PLAYER_NUMBER));

  EXPECT_THAT(sounds().played(), ElementsAre(PLAYER_HIT_SOUND_ID));
}

/**
 * Given a Bydo listed by a state of LISTING_TICK, whose EntityDestroyed copies
 * were all lost, and a state of REMOVAL_TICK, 900 ms later, that lists nothing
 * When a state of a newer tick closes that tick
 * Then the Bydo is no longer drawn
 */
TEST_F(EntityReplicatorFixture,
       RemovesAnEntityUnlistedFor900MillisecondsAtAClosedTick) {
  replicator().apply(stateListing(
      LISTING_TICK, {listed(BYDO_ID, DRIFTER_TYPE_NUMBER, FIRST_POSITION)}));
  replicator().apply(stateListing(REMOVAL_TICK, {}));

  replicator().apply(stateListing(AFTER_REMOVAL_TICK, {}));

  EXPECT_THAT(drawnEntities(components()), IsEmpty());
}

/**
 * Given a Bydo listed by a state of LISTING_TICK, then by a state of
 * REMOVAL_TICK further on
 * When a state of a newer tick leaves it out
 * Then the Bydo is still drawn, where the state of REMOVAL_TICK put it
 */
TEST_F(EntityReplicatorFixture, KeepsAnEntityTheNewestStateLists) {
  replicator().apply(stateListing(
      LISTING_TICK, {listed(BYDO_ID, DRIFTER_TYPE_NUMBER, FIRST_POSITION)}));
  replicator().apply(stateListing(
      REMOVAL_TICK, {listed(BYDO_ID, DRIFTER_TYPE_NUMBER, SECOND_POSITION)}));

  replicator().apply(stateListing(AFTER_REMOVAL_TICK, {}));

  EXPECT_THAT(
      drawnEntities(components()),
      ElementsAre(drawnAs(EntityType::Enemy, BYDO_ID, SECOND_POSITION)));
}

/**
 * Given a player missile listed by a state of LISTING_TICK, and a state of
 * LAST_KEPT_TICK that leaves it out
 * When the state of REMOVAL_TICK comes in two parts, the first leaving the
 * missile out and the second listing it
 * Then the missile keeps its history from LISTING_TICK: the first part does
 * not remove the missile for the second to create it again
 */
TEST_F(EntityReplicatorFixture, KeepsAnEntityALaterPartOfTheSameStateLists) {
  replicator().apply(stateListing(
      LISTING_TICK,
      {listed(PLAYER_MISSILE_ID, PLAYER_MISSILE_TYPE_NUMBER, FIRST_POSITION)}));
  replicator().apply(stateListing(LAST_KEPT_TICK, {}));

  replicator().apply(stateListing(REMOVAL_TICK, {}));
  replicator().apply(stateListing(
      REMOVAL_TICK, {listed(PLAYER_MISSILE_ID, PLAYER_MISSILE_TYPE_NUMBER,
                            SECOND_POSITION)}));

  const auto atTheFirstListing =
      historyPositionAt(components(), static_cast<double>(LISTING_TICK));
  ASSERT_TRUE(atTheFirstListing.has_value());
  EXPECT_FLOAT_EQ(atTheFirstListing.value_or(NO_HISTORY_POSITION).x,
                  FIRST_POSITION.x);
}

/**
 * Given a Bydo listed by a state of LISTING_TICK, and a state of REMOVAL_TICK
 * that leaves it out
 * When a state of a newer tick closes that tick
 * Then no effect starts: the client cannot tell whether it was killed
 */
TEST_F(EntityReplicatorFixture, AnEntityRemovedUnlistedLeavesNoEffect) {
  replicator().apply(stateListing(
      LISTING_TICK, {listed(BYDO_ID, DRIFTER_TYPE_NUMBER, FIRST_POSITION)}));
  replicator().apply(stateListing(REMOVAL_TICK, {}));

  replicator().apply(stateListing(AFTER_REMOVAL_TICK, {}));

  EXPECT_THAT(drawnEffects(components()), IsEmpty());
}

/**
 * Given a state of LISTING_TICK, then a Bydo an EntitySpawned announced and no
 * state listed, and a state of REMOVAL_TICK that leaves it out
 * When a state of a newer tick closes that tick
 * Then the Bydo is no longer drawn
 */
TEST_F(EntityReplicatorFixture, RemovesASpawnedEntityNoStateListed) {
  replicator().apply(stateListing(LISTING_TICK, {}));
  replicator().apply(spawned(BYDO_ID, DRIFTER_TYPE_NUMBER, FIRST_POSITION));
  replicator().apply(stateListing(REMOVAL_TICK, {}));

  replicator().apply(stateListing(AFTER_REMOVAL_TICK, {}));

  EXPECT_THAT(drawnEntities(components()), IsEmpty());
}

/**
 * Given a Bydo removed once REMOVAL_TICK, whose state left it out, was closed
 * When a newer state lists it
 * Then the Bydo is drawn again, where that state puts it
 */
TEST_F(EntityReplicatorFixture,
       CreatesAgainAnEntityRemovedUnlistedThatAStateLists) {
  replicator().apply(stateListing(
      LISTING_TICK, {listed(BYDO_ID, DRIFTER_TYPE_NUMBER, FIRST_POSITION)}));
  replicator().apply(stateListing(REMOVAL_TICK, {}));
  replicator().apply(stateListing(AFTER_REMOVAL_TICK, {}));

  replicator().apply(
      stateListing(AFTER_REMOVAL_TICK + 1,
                   {listed(BYDO_ID, DRIFTER_TYPE_NUMBER, SECOND_POSITION)}));

  EXPECT_THAT(
      drawnEntities(components()),
      ElementsAre(drawnAs(EntityType::Enemy, BYDO_ID, SECOND_POSITION)));
}

/**
 * Given a replicator whose listener predicts the missiles of its player
 * When an EntitySpawned announces a missile that player fired
 * Then the replicator draws nothing of it and hands it to the listener
 */
TEST_F(EntityReplicatorFixture, LeavesAMissileOfItsPlayerToThePrediction) {
  ownMissiles().predict(true);

  replicator().apply(ownMissileSpawned(OWN_PLAYER));

  EXPECT_EQ(announcedEntityCount(components()), 0U);
  EXPECT_EQ(ownMissiles().spawned().size(), 1U);
}

/**
 * Given a replicator whose listener predicts the missiles of its player
 * When an EntitySpawned announces a missile another player fired
 * Then the replicator creates it as any missile
 */
TEST_F(EntityReplicatorFixture, CreatesAMissileOfAnotherPlayer) {
  ownMissiles().predict(true);

  replicator().apply(ownMissileSpawned(ANOTHER_PLAYER));

  EXPECT_EQ(announcedEntityCount(components()), 1U);
  EXPECT_TRUE(ownMissiles().spawned().empty());
}

/**
 * Given a replicator whose listener does not predict yet
 * When an EntitySpawned announces a missile its player fired
 * Then the replicator creates it as any missile
 */
TEST_F(EntityReplicatorFixture, CreatesItsOwnMissileWithoutPrediction) {
  replicator().apply(ownMissileSpawned(OWN_PLAYER));

  EXPECT_EQ(announcedEntityCount(components()), 1U);
}

/**
 * Given a missile of its player the replicator left to the prediction
 * When a WorldState lists it, then an EntityDestroyed removes it
 * Then nothing is drawn of it, and the listener is told it was removed
 */
TEST_F(EntityReplicatorFixture, HandsTheRemovalOfAClaimedMissileOn) {
  ownMissiles().predict(true);
  replicator().apply(ownMissileSpawned(OWN_PLAYER));

  replicator().apply(stateListing(
      APPLIED_TICK, {listed(PLAYER_MISSILE_ID, PLAYER_MISSILE_TYPE_NUMBER,
                            PLAYER_MISSILE_POSITION)}));
  replicator().apply(removed(PLAYER_MISSILE_ID));

  EXPECT_EQ(announcedEntityCount(components()), 0U);
  EXPECT_EQ(ownMissiles().destroyed().size(), 1U);
}
