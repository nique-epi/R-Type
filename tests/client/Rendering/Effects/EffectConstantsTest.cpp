#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <algorithm>
#include <array>
#include <cstddef>
#include <span>
#include <vector>
#include "Animation.hpp"
#include "EffectConstants.hpp"
#include "EffectLengths.hpp"
#include "EntitySpriteConstants.hpp"
#include "SheetPixels.hpp"
#include "SpriteFrame.hpp"

using rtype::client::Animation;
using rtype::client::BYDO_EXPLOSION;
using rtype::client::BYDO_EXPLOSION_FLASH_AREA;
using rtype::client::BYDO_FRAMES;
using rtype::client::BYDO_SILHOUETTE_AREA;
using rtype::client::EFFECT_SHEET_ID;
using rtype::client::EffectAnimations;
using rtype::client::ENEMY_MUZZLE_FLASH;
using rtype::client::ENTITY_SHEET_ID;
using rtype::client::HIT_FLASH_DURATION;
using rtype::client::IMPACT;
using rtype::client::IMPACT_FLASH_AREA;
using rtype::client::IMPACT_FLASH_FRAMES;
using rtype::client::MUZZLE_FLASH_AREA;
using rtype::client::PLAYER_MUZZLE_FLASH;
using rtype::client::PLAYER_SHIP_FRAMES;
using rtype::client::SHIP_EXPLOSION;
using rtype::client::SHIP_EXPLOSION_FLASH_AREA;
using rtype::client::SHIP_EXPLOSION_FLASH_FRAMES;
using rtype::client::SHIP_SILHOUETTE_AREA;
using rtype::game::SpriteFrame;

namespace {

constexpr std::array ALL_EFFECTS{BYDO_EXPLOSION, SHIP_EXPLOSION, IMPACT,
                                 PLAYER_MUZZLE_FLASH, ENEMY_MUZZLE_FLASH};

/** @brief The white at the heart of an explosion, #fffbe8, which no version
 * without flash may show. */
constexpr sf::Color EXPLOSION_WHITE(255, 251, 232);

/** @brief The areas measured in the sheets as shipped: the most pixels a
 * frame of each effect changes between its row and its row without flash,
 * the frames that change any, and the largest silhouette of a Bydo and of a
 * ship. */
constexpr int MEASURED_BYDO_EXPLOSION_FLASH = 275;
constexpr int MEASURED_BYDO_EXPLOSION_FLASH_FRAMES = 1;
constexpr int MEASURED_SHIP_EXPLOSION_FLASH = 23;
constexpr int MEASURED_SHIP_EXPLOSION_FLASH_FRAMES = 8;
constexpr int MEASURED_IMPACT_FLASH = 6;
constexpr int MEASURED_IMPACT_FLASH_FRAMES = 2;
constexpr int MEASURED_MUZZLE_FLASH = 12;
constexpr int MEASURED_MUZZLE_FLASH_FRAMES = 1;
constexpr int MEASURED_BYDO_SILHOUETTE = 367;
constexpr int MEASURED_SHIP_SILHOUETTE = 254;

/** @brief What the version without flash of an effect changes: the most
 * pixels one frame changes, and how many frames, from the first, change
 * any. */
struct Change {
  int pixels{};
  int frames{};

  bool operator==(const Change&) const = default;
};

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

int largestVisibleArea(const sf::Image& sheet,
                       std::span<const SpriteFrame> frames) {
  int largest = 0;
  for (const SpriteFrame& frame : frames) {
    largest = std::max(largest, visiblePixelCount(sheet, frame));
  }
  return largest;
}

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

/**
 * Given the effect sheet as shipped
 * When the pixels each frame of an effect changes between its row and its
 * row without flash are counted
 * Then the most any frame changes, and the frames that change any, are those
 * measured
 */
TEST(EffectConstants, TheFlashOfEachEffectIsTheOneMeasuredInTheSheet) {
  const sf::Image sheet = shippedSheet(EFFECT_SHEET_ID);

  EXPECT_EQ(changeOf(sheet, BYDO_EXPLOSION),
            (Change{.pixels = MEASURED_BYDO_EXPLOSION_FLASH,
                    .frames = MEASURED_BYDO_EXPLOSION_FLASH_FRAMES}));
  EXPECT_EQ(changeOf(sheet, SHIP_EXPLOSION),
            (Change{.pixels = MEASURED_SHIP_EXPLOSION_FLASH,
                    .frames = MEASURED_SHIP_EXPLOSION_FLASH_FRAMES}));
  EXPECT_EQ(changeOf(sheet, IMPACT),
            (Change{.pixels = MEASURED_IMPACT_FLASH,
                    .frames = MEASURED_IMPACT_FLASH_FRAMES}));
  EXPECT_EQ(changeOf(sheet, PLAYER_MUZZLE_FLASH),
            (Change{.pixels = MEASURED_MUZZLE_FLASH,
                    .frames = MEASURED_MUZZLE_FLASH_FRAMES}));
}

/**
 * Given the entity sheet as shipped
 * When the visible pixels of every frame of a Bydo and of a ship are counted
 * Then the largest are those measured
 */
TEST(EffectConstants, TheSilhouettesAreTheOnesMeasuredInTheSheet) {
  const sf::Image sheet = shippedSheet(ENTITY_SHEET_ID);

  EXPECT_EQ(largestVisibleArea(sheet, BYDO_FRAMES), MEASURED_BYDO_SILHOUETTE);
  EXPECT_EQ(largestVisibleArea(sheet, PLAYER_SHIP_FRAMES),
            MEASURED_SHIP_SILHOUETTE);
}

/**
 * Given the areas measured in the sheets
 * When the flash budget reads the area of each effect
 * Then it lends exactly what each effect flashes, for as many frames
 */
TEST(EffectConstants, TheFlashBudgetLendsTheAreasMeasured) {
  EXPECT_EQ(BYDO_EXPLOSION_FLASH_AREA, MEASURED_BYDO_EXPLOSION_FLASH);
  EXPECT_EQ(SHIP_EXPLOSION_FLASH_AREA, MEASURED_SHIP_EXPLOSION_FLASH);
  EXPECT_EQ(SHIP_EXPLOSION_FLASH_FRAMES, MEASURED_SHIP_EXPLOSION_FLASH_FRAMES);
  EXPECT_EQ(IMPACT_FLASH_AREA, MEASURED_IMPACT_FLASH);
  EXPECT_EQ(IMPACT_FLASH_FRAMES, MEASURED_IMPACT_FLASH_FRAMES);
  EXPECT_EQ(MUZZLE_FLASH_AREA, MEASURED_MUZZLE_FLASH);
  EXPECT_EQ(BYDO_SILHOUETTE_AREA, MEASURED_BYDO_SILHOUETTE);
  EXPECT_EQ(SHIP_SILHOUETTE_AREA, MEASURED_SHIP_SILHOUETTE);
}
