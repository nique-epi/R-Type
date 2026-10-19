#include "EntitySprites.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <optional>
#include "Animation.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntityType.hpp"
#include "Layer.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The animation of an entity type, whose first frame is the sprite
 * of the type, or nullptr for a value outside EntityType. */
const Animation* entityAnimationOf(game::EntityType type) {
  switch (type) {
    case game::EntityType::Player:
      return &PLAYER_SHIP_ANIMATION;
    case game::EntityType::Enemy:
      return &BYDO_ANIMATION;
    case game::EntityType::PlayerMissile:
      return &PLAYER_MISSILE_ANIMATION;
    case game::EntityType::EnemyMissile:
      return &ENEMY_MISSILE_ANIMATION;
  }
  return nullptr;
}

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

std::optional<game::Sprite> spriteOf(game::EntityType type) {
  const Animation* animation = entityAnimationOf(type);
  if (animation == nullptr) {
    return std::nullopt;
  }
  return game::Sprite{.assetId = ENTITY_SHEET_ID,
                      .frame = animation->frames.front(),
                      .layer = game::Layer::Entities};
}

std::optional<SpriteAnimation> animationOf(game::EntityType type,
                                           std::uint32_t networkId) {
  const Animation* animation = entityAnimationOf(type);
  if (animation == nullptr) {
    return std::nullopt;
  }
  return startedAt(*animation, networkId);
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
