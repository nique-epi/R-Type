#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <array>
#include <cstddef>
#include <vector>
#include "Animation.hpp"
#include "EffectConstants.hpp"
#include "EffectLengths.hpp"
#include "SheetPixels.hpp"
#include "SpriteFrame.hpp"

using rtype::client::Animation;
using rtype::client::BYDO_EXPLOSION;
using rtype::client::EFFECT_SHEET_ID;
using rtype::client::EffectAnimations;
using rtype::client::ENEMY_MUZZLE_FLASH;
using rtype::client::HIT_FLASH_DURATION;
using rtype::client::IMPACT;
using rtype::client::PLAYER_MUZZLE_FLASH;
using rtype::client::SHIP_EXPLOSION;
using rtype::game::SpriteFrame;

namespace {

constexpr std::array ALL_EFFECTS{BYDO_EXPLOSION, SHIP_EXPLOSION, IMPACT,
                                 PLAYER_MUZZLE_FLASH, ENEMY_MUZZLE_FLASH};

/** @brief The white at the heart of an explosion, #fffbe8, which no version
 * without flash may show. */
constexpr sf::Color EXPLOSION_WHITE(255, 251, 232);

/** @returns Every frame of both versions of every effect. */
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

}  // namespace

/**
 * Given the effect sheet shipped in assets/
 * When every frame of every effect is laid on it
 * Then each lies inside the sheet
 */
TEST(EffectConstants, EveryEffectFrameLiesInsideTheEffectSheet) {
  const sf::Image sheet = shippedSheet(EFFECT_SHEET_ID);
  for (const SpriteFrame& frame : everyEffectFrame()) {
    SCOPED_TRACE(frame.top);
    SCOPED_TRACE(frame.left);

    EXPECT_TRUE(liesInside(frame, sheet.getSize()));
  }
}

/**
 * Given the effect sheet shipped in assets/
 * When every frame of every effect is laid on it
 * Then each shows at least one visible pixel
 */
TEST(EffectConstants, EveryEffectFrameShowsAVisiblePixel) {
  const sf::Image sheet = shippedSheet(EFFECT_SHEET_ID);
  for (const SpriteFrame& frame : everyEffectFrame()) {
    SCOPED_TRACE(frame.top);
    SCOPED_TRACE(frame.left);

    EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
  }
}

/**
 * Given the effect sheet shipped in assets/
 * When the pixels just outside every frame are read
 * Then they are all transparent, so no frame cuts its drawing
 */
TEST(EffectConstants, NoEffectFrameCutsItsDrawing) {
  const sf::Image sheet = shippedSheet(EFFECT_SHEET_ID);
  for (const SpriteFrame& frame : everyEffectFrame()) {
    SCOPED_TRACE(frame.top);
    SCOPED_TRACE(frame.left);

    EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
  }
}

/**
 * Given every frame of every effect
 * When each pair is compared
 * Then no two share a pixel of the sheet
 */
TEST(EffectConstants, NoTwoEffectFramesOverlap) {
  const std::vector<SpriteFrame> frames = everyEffectFrame();
  for (std::size_t first = 0; first < frames.size(); ++first) {
    for (std::size_t second = first + 1; second < frames.size(); ++second) {
      SCOPED_TRACE(first);
      SCOPED_TRACE(second);

      EXPECT_FALSE(overlap(frames[first], frames[second]));
    }
  }
}

/**
 * Given every effect
 * When its two versions are compared
 * Then they last as long, so reducing flashes never shortens an effect
 */
TEST(EffectConstants, BothVersionsOfAnEffectLastAsLong) {
  for (const EffectAnimations& effect : ALL_EFFECTS) {
    EXPECT_EQ(effect.standard.cycle(), effect.withoutFlash.cycle());
  }
}

/**
 * Given every effect
 * When its two versions are read
 * Then both play once, so every effect ends
 */
TEST(EffectConstants, EveryEffectPlaysOnce) {
  for (const EffectAnimations& effect : ALL_EFFECTS) {
    EXPECT_EQ(effect.standard.playback, Animation::Playback::Once);
    EXPECT_EQ(effect.withoutFlash.playback, Animation::Playback::Once);
  }
}

/**
 * Given the Bydo explosion
 * When its length is read
 * Then it lasts 420 ms, as on the animation page
 */
TEST(EffectConstants, TheBydoExplosionLastsFourHundredTwentyMilliseconds) {
  EXPECT_EQ(BYDO_EXPLOSION.standard.cycle(), BYDO_EXPLOSION_LENGTH);
}

/**
 * Given the ship explosion
 * When its length is read
 * Then it lasts 600 ms, as on the animation page
 */
TEST(EffectConstants, TheShipExplosionLastsSixHundredMilliseconds) {
  EXPECT_EQ(SHIP_EXPLOSION.standard.cycle(), SHIP_EXPLOSION_LENGTH);
}

/**
 * Given the impact of a missile
 * When its length is read
 * Then it lasts 150 ms, as on the animation page
 */
TEST(EffectConstants, AnImpactLastsOneHundredFiftyMilliseconds) {
  EXPECT_EQ(IMPACT.standard.cycle(), IMPACT_LENGTH);
}

/**
 * Given the muzzle flashes of both missiles
 * When their lengths are read
 * Then each lasts 100 ms, as on the animation page
 */
TEST(EffectConstants, AMuzzleFlashLastsOneHundredMilliseconds) {
  EXPECT_EQ(PLAYER_MUZZLE_FLASH.standard.cycle(), MUZZLE_FLASH_LENGTH);
  EXPECT_EQ(ENEMY_MUZZLE_FLASH.standard.cycle(), MUZZLE_FLASH_LENGTH);
}

/**
 * Given the flash of an entity hit
 * When its length is read
 * Then it lasts 60 ms, as on the animation page
 */
TEST(EffectConstants, AHitFlashLastsSixtyMilliseconds) {
  EXPECT_EQ(HIT_FLASH_DURATION, HIT_FLASH_LENGTH);
}

/**
 * Given the effect sheet shipped in assets/
 * When every frame of every version without flash is read
 * Then none shows the white of an explosion
 */
TEST(EffectConstants, NoVersionWithoutFlashShowsTheWhiteOfAnExplosion) {
  const sf::Image sheet = shippedSheet(EFFECT_SHEET_ID);
  for (const EffectAnimations& effect : ALL_EFFECTS) {
    for (const SpriteFrame& frame : effect.withoutFlash.frames) {
      SCOPED_TRACE(frame.top);
      SCOPED_TRACE(frame.left);

      EXPECT_FALSE(showsColor(sheet, frame, EXPLOSION_WHITE));
    }
  }
}
