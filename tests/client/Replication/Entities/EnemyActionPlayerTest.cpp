#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <optional>
#include "AnimationSystem.hpp"
#include "AppearanceReference.hpp"
#include "ComponentRegistry.hpp"
#include "EnemyAction.hpp"
#include "EnemyActionPlayer.hpp"
#include "EnemyActionPlayerTestConstants.hpp"
#include "Entity.hpp"
#include "EntityAppearance.hpp"
#include "EntityRegistry.hpp"
#include "NetworkIdentity.hpp"
#include "Playback.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"

using rtype::client::AnimationSystem;
using rtype::client::AppearanceReference;
using rtype::client::EnemyActionPlayer;
using rtype::client::EntityAppearance;
using rtype::client::Playback;
using rtype::client::SpriteAnimation;
using rtype::client::testing::appearanceWithAStrike;
using rtype::client::testing::DESCRIBED_ATTACK;
using rtype::client::testing::ENEMY_ID;
using rtype::client::testing::IDLE_FRAME;
using rtype::client::testing::STRIKE_ANCHOR;
using rtype::client::testing::STRIKE_FRAME_TIME;
using rtype::client::testing::UNDESCRIBED_ATTACK;
using rtype::client::testing::UNKNOWN_ENEMY_ID;
using rtype::engine::ComponentRegistry;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::game::NetworkIdentity;
using rtype::game::Sprite;
using rtype::network::EnemyAction;
using rtype::network::EnemyActionPhase;

namespace {

/** @brief An enemy that shows its idle frame, an animation system, and the
 * player of the actions announced to it. */
class EnemyScene {
 public:
  EnemyScene() {
    enemy_ = entities_.create();
    components_.add(enemy_, NetworkIdentity{ENEMY_ID});
    components_.add(enemy_, AppearanceReference{&appearance_});
    components_.add(enemy_, SpriteAnimation{.animation = idleOf(appearance_)});
    components_.add(enemy_, Sprite{.assetId = appearance_.sheet,
                                   .frame = IDLE_FRAME,
                                   .layer = rtype::game::Layer::Enemies});
  }

  void announce(std::uint32_t entityId, std::uint8_t attack,
                EnemyActionPhase phase) {
    player_.apply(
        EnemyAction{.entityId = entityId, .attack = attack, .phase = phase});
  }

  void play(rtype::engine::Duration elapsed) {
    system_.update(components_, elapsed);
  }

  [[nodiscard]] const SpriteAnimation& playing() {
    return *components_.get<SpriteAnimation>(enemy_);
  }

  [[nodiscard]] rtype::game::SpriteFrame shownFrame() {
    return components_.get<Sprite>(enemy_)->frame;
  }

 private:
  static rtype::client::Animation idleOf(const EntityAppearance& appearance) {
    return rtype::client::Animation{
        .frames = appearance.idleFrames,
        .frameDuration = appearance.idleFrameDuration,
        .playback = Playback::Loop};
  }

  EntityAppearance appearance_ = appearanceWithAStrike();
  EntityRegistry entities_;
  ComponentRegistry components_{entities_};
  Entity enemy_;
  EnemyActionPlayer player_{components_};
  AnimationSystem system_;
};

}  // namespace

/**
 * Given an enemy whose sheet describes a held strike with an anchor
 * When the server announces that strike
 * Then the enemy plays the strike from its first frame, held, with the anchor
 */
TEST(EnemyActionPlayer, PlaysTheAnimationOfTheAnnouncedPhase) {
  EnemyScene scene;

  scene.announce(ENEMY_ID, DESCRIBED_ATTACK, EnemyActionPhase::Strike);

  EXPECT_EQ(scene.playing().animation.playback, Playback::Hold);
  EXPECT_EQ(scene.playing().animation.anchor, STRIKE_ANCHOR);
  EXPECT_EQ(scene.playing().animation.frameDuration, STRIKE_FRAME_TIME);
  EXPECT_EQ(scene.playing().elapsed, rtype::engine::Duration::zero());
}

/**
 * Given an enemy that was told to strike
 * When the animation system plays the whole strike, two frames of 100 ms
 * Then the enemy shows its idle frame again
 */
TEST(EnemyActionPlayer, GoesBackToIdleOnceTheStrikeHasEnded) {
  EnemyScene scene;
  scene.announce(ENEMY_ID, DESCRIBED_ATTACK, EnemyActionPhase::Strike);

  scene.play(STRIKE_FRAME_TIME * 2);

  EXPECT_EQ(scene.shownFrame(), IDLE_FRAME);
  EXPECT_EQ(scene.playing().animation.playback, Playback::Loop);
}

/**
 * Given an enemy that was told to strike
 * When the animation system plays 100 ms
 * Then the enemy shows the second frame of the strike
 */
TEST(EnemyActionPlayer, ShowsTheFramesOfTheStrikeInTurn) {
  EnemyScene scene;
  scene.announce(ENEMY_ID, DESCRIBED_ATTACK, EnemyActionPhase::Strike);

  scene.play(STRIKE_FRAME_TIME);

  EXPECT_EQ(scene.shownFrame().left, 18);
}

/**
 * Given an enemy whose sheet describes only the strike of one attack
 * When the server announces another phase, then another attack
 * Then the enemy keeps its idle animation
 */
TEST(EnemyActionPlayer, KeepsTheIdleAnimationForAPhaseTheSheetLeavesOut) {
  EnemyScene scene;

  scene.announce(ENEMY_ID, DESCRIBED_ATTACK, EnemyActionPhase::Signal);
  scene.announce(ENEMY_ID, UNDESCRIBED_ATTACK, EnemyActionPhase::Strike);

  EXPECT_EQ(scene.playing().animation.playback, Playback::Loop);
}

/**
 * Given an enemy
 * When the server announces a strike to an entity the client does not have
 * Then the enemy keeps its idle animation
 */
TEST(EnemyActionPlayer, IgnoresAnEntityTheClientDoesNotHave) {
  EnemyScene scene;

  scene.announce(UNKNOWN_ENEMY_ID, DESCRIBED_ATTACK, EnemyActionPhase::Strike);

  EXPECT_EQ(scene.playing().animation.playback, Playback::Loop);
}
