#include "EffectConstantsFixtures.hpp"
#include <SFML/Graphics/Image.hpp>
#include <algorithm>
#include <cstddef>
#include <vector>
#include "EffectAnimations.hpp"
#include "EffectConstantsTestConstants.hpp"
#include "SheetPixels.hpp"
#include "SpriteFrame.hpp"

using rtype::client::EffectAnimations;
using rtype::game::SpriteFrame;

Change changeOf(const sf::Image& sheet, const EffectAnimations& effect) {
  Change change;
  for (std::size_t i = 0; i < effect.standard.frames.size(); ++i) {
    const int pixels = changedPixelCount(sheet, effect.standard.frames[i],
                                         effect.withoutFlash.frames[i]);
    if (pixels > 0) {
      change.pixels = std::max(change.pixels, pixels);
      change.frames = static_cast<int>(i) + 1;
    }
  }
  return change;
}

std::vector<SpriteFrame> everyEffectFrame() {
  std::vector<SpriteFrame> frames;
  for (const EffectAnimations& effect : ALL_EFFECTS) {
    frames.insert(frames.end(), effect.standard.frames.begin(),
                  effect.standard.frames.end());
    frames.insert(frames.end(), effect.withoutFlash.frames.begin(),
                  effect.withoutFlash.frames.end());
  }
  return frames;
}
