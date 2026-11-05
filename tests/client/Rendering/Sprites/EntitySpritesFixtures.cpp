#include "EntitySpritesFixtures.hpp"
#include <string>
#include "EntityAppearance.hpp"
#include "EntitySpritesTestConstants.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"
#include "SpriteTestConstants.hpp"

using rtype::client::EntityAppearance;
using rtype::client::SpriteAnimation;
using rtype::game::SpriteFrame;

const EntityAppearance& testAppearance() {
  static const EntityAppearance appearance{
      .sheet = std::string{TEST_SHEET_ID},
      .idleFrames = {TEST_IDLE_FRAMES.begin(), TEST_IDLE_FRAMES.end()},
      .idleFrameDuration = TEST_FRAME_TIME,
      .actions = {}};
  return appearance;
}

SpriteFrame startFrameOf(const SpriteAnimation& animation) {
  return animation.animation.frameAt(animation.elapsed).value_or(NO_FRAME);
}
