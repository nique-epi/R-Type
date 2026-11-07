#include "EnemyScene.hpp"
#include <cstdint>
#include <utility>
#include "Animation.hpp"
#include "AppearanceReference.hpp"
#include "EnemyAction.hpp"
#include "EnemyActionPlayerFixtures.hpp"
#include "EnemyActionPlayerTestConstants.hpp"
#include "EntityAppearance.hpp"
#include "Layer.hpp"
#include "NetworkIdentity.hpp"
#include "Playback.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

namespace {

/** @returns The idle animation of @p appearance, looped. */
Animation idleOf(const EntityAppearance& appearance) {
  return Animation{.frames = appearance.idleFrames,
                   .frameDuration = appearance.idleFrameDuration,
                   .playback = Playback::Loop};
}

}  // namespace

EnemyScene::EnemyScene() : EnemyScene(appearanceWithAStrike()) {}

EnemyScene::EnemyScene(EntityAppearance appearance)
    : appearance_(std::move(appearance)) {
  enemy_ = entities_.create();
  components_.add(enemy_, game::NetworkIdentity{ENEMY_ID});
  components_.add(enemy_, AppearanceReference{&appearance_});
  components_.add(enemy_, SpriteAnimation{.animation = idleOf(appearance_)});
  components_.add(enemy_, game::Sprite{.assetId = appearance_.sheet,
                                       .frame = IDLE_FRAME,
                                       .layer = game::Layer::Enemies});
}

void EnemyScene::announce(std::uint32_t entityId, std::uint8_t attack,
                          network::EnemyActionPhase phase) {
  player_.apply(network::EnemyAction{
      .entityId = entityId, .attack = attack, .phase = phase});
}

void EnemyScene::play(engine::Duration elapsed) {
  system_.update(components_, elapsed);
}

const SpriteAnimation& EnemyScene::playing() {
  return *components_.get<SpriteAnimation>(enemy_);
}

game::SpriteFrame EnemyScene::shownFrame() {
  return components_.get<game::Sprite>(enemy_)->frame;
}

}  // namespace rtype::client::testing
