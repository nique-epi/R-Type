#include "NamedEffectScene.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include "AppearanceReference.hpp"
#include "EffectPlayed.hpp"
#include "EffectPreferences.hpp"
#include "Effects/EffectTestConstants.hpp"
#include "Entity.hpp"
#include "EntityAppearance.hpp"
#include "FlashPreference.hpp"
#include "Layer.hpp"
#include "NamedEffect.hpp"
#include "NamedEffectTestConstants.hpp"
#include "NetworkIdentity.hpp"
#include "Position.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

namespace {

/** @returns An appearance of the sheet EFFECT_BODY_SHEET whose one effect is
 * SPARK. */
EntityAppearance appearanceWithASpark() {
  return EntityAppearance{
      .sheet = std::string{EFFECT_BODY_SHEET},
      .idleFrames = {},
      .actions = {},
      .enragedIdleFrames = {},
      .deathFrames = {},
      .deathFramesWithoutFlash = {},
      .effects = {NamedEffect{
          .name = std::string{SPARK},
          .frames = {SPARK_FIRST_FRAME},
          .framesWithoutFlash = {SPARK_FIRST_FRAME_WITHOUT_FLASH},
          .frameDuration = SPARK_FRAME_TIME,
          .anchor = SPARK_ANCHOR}},
      .looks = {}};
}

}  // namespace

NamedEffectScene::NamedEffectScene()
    : appearance_(appearanceWithASpark()),
      effects_(entities_, components_, EFFECT_TEST_SEED),
      player_(effects_.tools()) {
  const engine::Entity body = entities_.create();
  components_.add(body, game::NetworkIdentity{EFFECT_BODY_ID});
  components_.add(body, EFFECT_BODY_POSITION);
  components_.add(body, AppearanceReference{&appearance_});
}

void NamedEffectScene::play(std::uint32_t bodyId, std::string_view effect) {
  player_.apply(
      network::EffectPlayed{.entityId = bodyId, .effect = std::string{effect}});
}

void NamedEffectScene::preferFlashes(FlashPreference preference) {
  effects_.advance(engine::Duration::zero(),
                   EffectPreferences{.flashes = preference});
}

std::size_t NamedEffectScene::effectCount() {
  std::size_t count = 0;
  components_.forEach<game::Sprite, SpriteAnimation>(
      [&count](engine::Entity, const game::Sprite& sprite,
               const SpriteAnimation&) {
        if (sprite.layer == game::Layer::Effects) {
          ++count;
        }
      });
  return count;
}

std::optional<game::SpriteFrame> NamedEffectScene::effectFrame() {
  std::optional<game::SpriteFrame> frame;
  components_.forEach<game::Sprite, game::Position>(
      [&frame](engine::Entity, const game::Sprite& sprite,
               const game::Position& position) {
        if (sprite.layer == game::Layer::Effects &&
            sprite.assetId == EFFECT_BODY_SHEET &&
            position.x == EFFECT_BODY_POSITION.x &&
            position.y == EFFECT_BODY_POSITION.y) {
          frame = sprite.frame;
        }
      });
  return frame;
}

SpriteAnimation NamedEffectScene::effectAnimation() {
  std::optional<SpriteAnimation> found;
  components_.forEach<game::Sprite, SpriteAnimation>(
      [&found](engine::Entity, const game::Sprite& sprite,
               const SpriteAnimation& animation) {
        if (sprite.layer == game::Layer::Effects) {
          found = animation;
        }
      });
  EXPECT_TRUE(found.has_value());
  return found.value_or(SpriteAnimation{});
}

}  // namespace rtype::client::testing
