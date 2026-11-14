#include "EntitySprites.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <optional>
#include "AnimatedSprite.hpp"
#include "Animation.hpp"
#include "EntityAppearance.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntityType.hpp"
#include "Layer.hpp"
#include "NamedLook.hpp"
#include "Playback.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

Animation idleAnimationOf(const EntityAppearance& appearance) {
  return Animation{.frames = appearance.idleFrames,
                   .frameDuration = appearance.idleFrameDuration,
                   .playback = Playback::Loop};
}

Animation enragedIdleAnimationOf(const EntityAppearance& appearance) {
  if (appearance.enragedIdleFrames.empty()) {
    return idleAnimationOf(appearance);
  }
  return Animation{.frames = appearance.enragedIdleFrames,
                   .frameDuration = appearance.enragedIdleFrameDuration,
                   .playback = Playback::Loop};
}

std::optional<Animation> lookAnimationOf(const EntityAppearance& appearance,
                                         std::uint8_t look) {
  if (look == 0 || look > appearance.looks.size()) {
    return std::nullopt;
  }
  const NamedLook& shown = appearance.looks[look - 1U];
  return Animation{.frames = shown.frames,
                   .frameDuration = shown.frameDuration,
                   .playback = Playback::Loop};
}

namespace {

/** @returns The animation with its elapsed time set to the offset of a network
 * id: the fractional part of networkId times LOOP_START_SPREAD, as a share of
 * one cycle. */
SpriteAnimation startedAt(const Animation& animation, std::uint32_t networkId) {
  const double shareOfCycle =
      std::fmod(static_cast<double>(networkId) * LOOP_START_SPREAD, 1.0);
  return SpriteAnimation{
      .animation = animation,
      .elapsed = std::chrono::duration_cast<engine::Duration>(
          animation.cycle() * shareOfCycle)};
}

}  // namespace

game::Layer layerOf(game::EntityType side) {
  switch (side) {
    case game::EntityType::Player:
    case game::EntityType::PlayerMissile:
      return game::Layer::Players;
    case game::EntityType::EnemyMissile:
      return game::Layer::EnemyMissiles;
    case game::EntityType::Enemy:
    case game::EntityType::LookItem:
    case game::EntityType::Obstacle:
      break;
  }
  return game::Layer::Enemies;
}

AnimatedSprite animatedSpriteOf(const EntityAppearance& appearance,
                                game::EntityType side,
                                std::uint32_t networkId) {
  const SpriteAnimation started =
      startedAt(idleAnimationOf(appearance), networkId);
  return AnimatedSprite{
      .sprite = game::Sprite{.assetId = appearance.sheet,
                             .frame = started.animation.frameAt(started.elapsed)
                                          .value_or(game::SpriteFrame{}),
                             .layer = layerOf(side)},
      .animation = started};
}

sf::IntRect rectangleOf(game::SpriteFrame frame) {
  return {{frame.left, frame.top}, {frame.width, frame.height}};
}

game::SpriteFrame inPlayerColorRow(game::SpriteFrame frame, int color) {
  frame.top = PLAYER_SHIP_FRAME.top +
              (color * (PLAYER_SHIP_FRAME.height + SPRITE_SPACING));
  return frame;
}

}  // namespace rtype::client
