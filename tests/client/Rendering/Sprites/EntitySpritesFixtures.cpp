#include "EntitySpritesFixtures.hpp"
#include <optional>
#include <vector>
#include "AllEntityTypes.hpp"
#include "EntitySprites.hpp"
#include "EntityType.hpp"
#include "Sprite.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "SpriteTestConstants.hpp"

using rtype::client::animationOf;
using rtype::client::SpriteAnimation;
using rtype::client::spriteOf;
using rtype::game::EntityType;
using rtype::game::Sprite;
using rtype::game::SpriteFrame;

SpriteFrame frameOf(EntityType type) {
  const std::optional<Sprite> sprite = spriteOf(type);
  return sprite.has_value() ? sprite->frame : NO_FRAME;
}

std::vector<SpriteFrame> everyAnimationFrame() {
  std::vector<SpriteFrame> frames;
  for (const EntityType type : ALL_ENTITY_TYPES) {
    const std::optional<SpriteAnimation> animation = animationOf(type, 0);
    if (animation.has_value()) {
      frames.insert(frames.end(), animation->animation.frames.begin(),
                    animation->animation.frames.end());
    }
  }
  return frames;
}

SpriteFrame startFrameOf(const SpriteAnimation& animation) {
  return animation.animation.frameAt(animation.elapsed).value_or(NO_FRAME);
}
