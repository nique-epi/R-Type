#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include "BossArrival.hpp"
#include "BossHealthChanged.hpp"
#include "BossStatus.hpp"
#include "EffectPlayed.hpp"
#include "EffectStarted.hpp"
#include "EnemyAction.hpp"
#include "EnemyActionStarted.hpp"
#include "EnemyPhase.hpp"
#include "EntityDestroyed.hpp"
#include "EntityDestruction.hpp"
#include "EntitySpawn.hpp"
#include "EntitySpawned.hpp"
#include "EntityTypeNumberTestConstants.hpp"
#include "EventMessage.hpp"
#include "EventMessages.hpp"
#include "GameEventTestConstants.hpp"
#include "LevelChange.hpp"
#include "LevelStatus.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "NetworkException.hpp"
#include "PlayerDeparture.hpp"
#include "PlayerLeft.hpp"

namespace rtype::server::testing {

/**
 * Given the events that need no number
 * When a monster spawn is encoded
 * Then the message is an EntitySpawned with the monster's id, type and
 * position, sent once
 */
TEST(EventMessagesTest, EncodesASpawnSentOnce) {
  const EventMessage message =
      eventMessageOf(game::EntitySpawn{.networkId = MONSTER_NETWORK_ID,
                                       .typeNumber = DRIFTER_TYPE_NUMBER,
                                       .x = MONSTER_X,
                                       .y = MONSTER_Y});

  EXPECT_EQ(message.message.type, network::MessageType::EntitySpawned);
  EXPECT_FALSE(message.guaranteed);
  EXPECT_EQ(
      network::decodeMessage<network::EntitySpawned>(message.message.payload),
      (network::EntitySpawned{.entityId = MONSTER_NETWORK_ID,
                              .entityType = DRIFTER_TYPE_NUMBER,
                              .x = MONSTER_X,
                              .y = MONSTER_Y}));
}

/**
 * Given the events that need no number
 * When an enemy entering the strike of attack 3 is encoded
 * Then the message is a guaranteed EnemyAction with its id, attack and phase
 */
TEST(EventMessagesTest, EncodesAnEnemyActionGuaranteed) {
  const EventMessage message = eventMessageOf(
      game::EnemyActionStarted{.networkId = MONSTER_NETWORK_ID,
                               .attack = SOME_ATTACK_NUMBER,
                               .phase = game::EnemyPhase::Strike});

  EXPECT_EQ(message.message.type, network::MessageType::EnemyAction);
  EXPECT_TRUE(message.guaranteed);
  EXPECT_EQ(
      network::decodeMessage<network::EnemyAction>(message.message.payload),
      (network::EnemyAction{.entityId = MONSTER_NETWORK_ID,
                            .attack = SOME_ATTACK_NUMBER,
                            .phase = network::EnemyActionPhase::Strike}));
}

/**
 * Given the events that need no number
 * When a body playing the effect spark is encoded
 * Then the message is a guaranteed EffectPlayed with its id and the name
 */
TEST(EventMessagesTest, EncodesANamedEffectGuaranteed) {
  const EventMessage message = eventMessageOf(game::EffectStarted{
      .networkId = MONSTER_NETWORK_ID, .effect = std::string{SPARK_EFFECT}});

  EXPECT_EQ(message.message.type, network::MessageType::EffectPlayed);
  EXPECT_TRUE(message.guaranteed);
  EXPECT_EQ(
      network::decodeMessage<network::EffectPlayed>(message.message.payload),
      (network::EffectPlayed{.entityId = MONSTER_NETWORK_ID,
                             .effect = std::string{SPARK_EFFECT}}));
}

/**
 * Given the events that need no number
 * When an enemy enters each of the four phases
 * Then the messages carry Signal, Strike, Recover and Enraged, in that order
 */
TEST(EventMessagesTest, EncodesEachPhaseAsTheProtocolNamesIt) {
  const auto phaseOf = [](game::EnemyPhase phase) {
    const EventMessage message =
        eventMessageOf(game::EnemyActionStarted{.networkId = MONSTER_NETWORK_ID,
                                                .attack = SOME_ATTACK_NUMBER,
                                                .phase = phase});
    return network::decodeMessage<network::EnemyAction>(message.message.payload)
        .phase;
  };

  EXPECT_EQ(phaseOf(game::EnemyPhase::Signal),
            network::EnemyActionPhase::Signal);
  EXPECT_EQ(phaseOf(game::EnemyPhase::Strike),
            network::EnemyActionPhase::Strike);
  EXPECT_EQ(phaseOf(game::EnemyPhase::Recover),
            network::EnemyActionPhase::Recover);
  EXPECT_EQ(phaseOf(game::EnemyPhase::Enraged),
            network::EnemyActionPhase::Enraged);
}

/**
 * Given the events that need no number
 * When an enemy action names the entity 0
 * Then encoding is refused with an InvalidFieldValueException
 */
TEST(EventMessagesTest, RefusesAnEnemyActionOfNoEntity) {
  EXPECT_THROW((void)eventMessageOf(
                   game::EnemyActionStarted{.networkId = UNASSIGNED_NETWORK_ID,
                                            .attack = SOME_ATTACK_NUMBER,
                                            .phase = game::EnemyPhase::Signal}),
               network::InvalidFieldValueException);
}

/**
 * Given the events that need no number
 * When a kill and a removal are encoded
 * Then the messages are EntityDestroyed with the reason Killed and the reason
 * Removed, each guaranteed
 */
TEST(EventMessagesTest, EncodesAKillAndARemovalWithTheirReason) {
  const EventMessage kill = eventMessageOf(
      game::EntityDestruction{.networkId = MONSTER_NETWORK_ID,
                              .cause = game::DestructionCause::Killed});
  const EventMessage removal = eventMessageOf(
      game::EntityDestruction{.networkId = MONSTER_NETWORK_ID,
                              .cause = game::DestructionCause::Removed});

  EXPECT_EQ(kill.message.type, network::MessageType::EntityDestroyed);
  EXPECT_TRUE(kill.guaranteed);
  EXPECT_TRUE(removal.guaranteed);
  EXPECT_EQ(
      network::decodeMessage<network::EntityDestroyed>(kill.message.payload),
      (network::EntityDestroyed{.entityId = MONSTER_NETWORK_ID,
                                .reason = network::DestructionReason::Killed}));
  EXPECT_EQ(
      network::decodeMessage<network::EntityDestroyed>(removal.message.payload),
      (network::EntityDestroyed{
          .entityId = MONSTER_NETWORK_ID,
          .reason = network::DestructionReason::Removed}));
}

/**
 * Given the events that need no number
 * When a player that left and a player that timed out are encoded
 * Then the messages are PlayerLeft with the number of the player and the
 * reason Left and the reason TimedOut, each guaranteed
 */
TEST(EventMessagesTest, EncodesADepartureWithItsReason) {
  const EventMessage left = eventMessageOf(
      game::PlayerDeparture{.playerNumber = SECOND_PLAYER_NUMBER,
                            .cause = game::DepartureCause::Left});
  const EventMessage timedOut = eventMessageOf(
      game::PlayerDeparture{.playerNumber = SECOND_PLAYER_NUMBER,
                            .cause = game::DepartureCause::TimedOut});

  EXPECT_EQ(left.message.type, network::MessageType::PlayerLeft);
  EXPECT_TRUE(left.guaranteed);
  EXPECT_TRUE(timedOut.guaranteed);
  EXPECT_EQ(network::decodeMessage<network::PlayerLeft>(left.message.payload),
            (network::PlayerLeft{.playerNumber = SECOND_PLAYER_NUMBER,
                                 .reason = network::DepartureReason::Left}));
  EXPECT_EQ(
      network::decodeMessage<network::PlayerLeft>(timedOut.message.payload),
      (network::PlayerLeft{.playerNumber = SECOND_PLAYER_NUMBER,
                           .reason = network::DepartureReason::TimedOut}));
}

/**
 * Given the events that need no number
 * When a player the host expelled is encoded
 * Then the message is a guaranteed PlayerLeft with the reason Expelled
 */
TEST(EventMessagesTest, EncodesAnExpulsionWithTheReasonExpelled) {
  const EventMessage expelled = eventMessageOf(
      game::PlayerDeparture{.playerNumber = SECOND_PLAYER_NUMBER,
                            .cause = game::DepartureCause::Expelled});

  EXPECT_TRUE(expelled.guaranteed);
  EXPECT_EQ(
      network::decodeMessage<network::PlayerLeft>(expelled.message.payload),
      (network::PlayerLeft{.playerNumber = SECOND_PLAYER_NUMBER,
                           .reason = network::DepartureReason::Expelled}));
}

/**
 * Given the arrival of a boss
 * When it is encoded
 * Then it is a guaranteed BossStatus that says the boss arrives, naming no
 * entity yet
 */
TEST(EventMessagesTest, EncodesTheArrivalOfTheBoss) {
  const EventMessage message = eventMessageOf(
      game::BossArrival{.typeNumber = BOSS_TYPE_NUMBER,
                        .warningMilliseconds = BOSS_WARNING_MILLISECONDS});

  EXPECT_EQ(message.message.type, network::MessageType::BossStatus);
  EXPECT_TRUE(message.guaranteed);
  EXPECT_EQ(
      network::decodeMessage<network::BossStatus>(message.message.payload),
      (network::BossStatus{.stage = network::BossStage::Arriving}));
}

/**
 * Given a calm boss hit down to 200 of its 240 points
 * When its health is encoded
 * Then it is a guaranteed BossStatus of a boss fighting with that health
 */
TEST(EventMessagesTest, EncodesAHitBossAsFighting) {
  const EventMessage message =
      eventMessageOf(game::BossHealthChanged{.networkId = BOSS_NETWORK_ID,
                                             .points = BOSS_POINTS_LEFT,
                                             .full = BOSS_FULL_POINTS,
                                             .enraged = false});

  EXPECT_TRUE(message.guaranteed);
  EXPECT_EQ(
      network::decodeMessage<network::BossStatus>(message.message.payload),
      (network::BossStatus{.bossEntityId = BOSS_NETWORK_ID,
                           .health = BOSS_POINTS_LEFT,
                           .fullHealth = BOSS_FULL_POINTS,
                           .stage = network::BossStage::Fighting}));
}

/**
 * Given an enraged boss with 200 of its 240 points
 * When its health is encoded
 * Then the BossStatus says the boss is enraged
 */
TEST(EventMessagesTest, EncodesAnEnragedBossAsEnraged) {
  const EventMessage message =
      eventMessageOf(game::BossHealthChanged{.networkId = BOSS_NETWORK_ID,
                                             .points = BOSS_POINTS_LEFT,
                                             .full = BOSS_FULL_POINTS,
                                             .enraged = true});

  EXPECT_EQ(network::decodeMessage<network::BossStatus>(message.message.payload)
                .stage,
            network::BossStage::Enraged);
}

/**
 * Given an enraged boss with no point left
 * When its health is encoded
 * Then the BossStatus says the boss is defeated
 */
TEST(EventMessagesTest, EncodesADeadBossAsDefeated) {
  const EventMessage message =
      eventMessageOf(game::BossHealthChanged{.networkId = BOSS_NETWORK_ID,
                                             .points = 0,
                                             .full = BOSS_FULL_POINTS,
                                             .enraged = true});

  EXPECT_EQ(network::decodeMessage<network::BossStatus>(message.message.payload)
                .stage,
            network::BossStage::Defeated);
}

/**
 * Given the biome wreck, scrolled 240 units, its scroll paused
 * When the change of the level is encoded
 * Then it is a guaranteed LevelStatus of that biome and that scroll
 */
TEST(EventMessagesTest, EncodesAChangeOfTheLevel) {
  const EventMessage message = eventMessageOf(
      game::LevelChange{.biome = std::string{LEVEL_BIOME},
                        .scrollPosition = LEVEL_SCROLL_POSITION,
                        .scrollSpeed = PAUSED_LEVEL_SCROLL_SPEED});

  EXPECT_EQ(message.message.type, network::MessageType::LevelStatus);
  EXPECT_TRUE(message.guaranteed);
  EXPECT_EQ(
      network::decodeMessage<network::LevelStatus>(message.message.payload),
      (network::LevelStatus{.biome = std::string{LEVEL_BIOME},
                            .scrollPosition = LEVEL_SCROLL_POSITION,
                            .scrollSpeed = PAUSED_LEVEL_SCROLL_SPEED}));
}

}  // namespace rtype::server::testing
