#include "HudSpriteConstantsFixtures.hpp"
#include <vector>
#include "HudSpriteConstants.hpp"
#include "SpriteFrame.hpp"

using rtype::client::LEFT_GAME_FRAME;
using rtype::client::LIFE_ROW;
using rtype::client::MARKER_FRAMES;
using rtype::client::WEAK_SIGNAL_FRAME;
using rtype::game::SpriteFrame;

std::vector<SpriteFrame> everyHudFrame() {
  std::vector<SpriteFrame> frames(LIFE_ROW.begin(), LIFE_ROW.end());
  frames.insert(frames.end(), MARKER_FRAMES.begin(), MARKER_FRAMES.end());
  frames.push_back(LEFT_GAME_FRAME);
  frames.push_back(WEAK_SIGNAL_FRAME);
  return frames;
}
