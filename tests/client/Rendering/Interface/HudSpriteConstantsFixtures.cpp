#include "HudSpriteConstantsFixtures.hpp"
#include <vector>
#include "HudSpriteConstants.hpp"
#include "SpriteFrame.hpp"

using rtype::client::LIFE_ROW;
using rtype::client::MARKER_FRAMES;
using rtype::game::SpriteFrame;

std::vector<SpriteFrame> everyHudFrame() {
  std::vector<SpriteFrame> frames(LIFE_ROW.begin(), LIFE_ROW.end());
  frames.insert(frames.end(), MARKER_FRAMES.begin(), MARKER_FRAMES.end());
  return frames;
}
