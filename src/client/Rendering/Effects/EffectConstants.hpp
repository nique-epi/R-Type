#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <chrono>
#include <cstddef>
#include "Animation.hpp"
#include "EntitySpriteConstants.hpp"
#include "SheetRow.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Asset id of the sheet of the effects: explosions, impacts and muzzle
 * flashes. Like the entity sheet, its pixels are at native size, each effect
 * holds one row, and frames and rows are SPRITE_SPACING pixels apart.
 */
constexpr const char* EFFECT_SHEET_ID = "sprites/effects.png";

/**
 * @brief The two versions of an effect: the standard one, and the one played
 * when the player turned REDUCE FLASHES on, which shows no white burst. Both
 * have as many frames, each as long.
 */
struct EffectAnimations {
  Animation standard;
  Animation withoutFlash;
};

/** @brief Sizes of the frames of the effects, in pixels of the sheet. */
constexpr int BYDO_EXPLOSION_SIZE = 48;
constexpr int SHIP_EXPLOSION_WIDTH = 40;
constexpr int SHIP_EXPLOSION_HEIGHT = 36;
constexpr int IMPACT_SIZE = 10;
constexpr int MUZZLE_FLASH_SIZE = 7;

/**
 * @brief The first frame of each row of the effect sheet, from the top: each
 * row starts SPRITE_SPACING pixels under the previous one, and the version
 * without flash of an effect lies right under its standard version, with
 * frames of the same size.
 */
constexpr game::SpriteFrame BYDO_EXPLOSION_FRAME{.left = 0,
                                                 .top = 0,
                                                 .width = BYDO_EXPLOSION_SIZE,
                                                 .height = BYDO_EXPLOSION_SIZE};
constexpr game::SpriteFrame BYDO_EXPLOSION_WITHOUT_FLASH_FRAME =
    frameUnder(BYDO_EXPLOSION_FRAME, SPRITE_SPACING);
constexpr game::SpriteFrame SHIP_EXPLOSION_FRAME{
    .left = 0,
    .top = bottomOf(BYDO_EXPLOSION_WITHOUT_FLASH_FRAME) + SPRITE_SPACING,
    .width = SHIP_EXPLOSION_WIDTH,
    .height = SHIP_EXPLOSION_HEIGHT};
constexpr game::SpriteFrame SHIP_EXPLOSION_WITHOUT_FLASH_FRAME =
    frameUnder(SHIP_EXPLOSION_FRAME, SPRITE_SPACING);
constexpr game::SpriteFrame IMPACT_FRAME{
    .left = 0,
    .top = bottomOf(SHIP_EXPLOSION_WITHOUT_FLASH_FRAME) + SPRITE_SPACING,
    .width = IMPACT_SIZE,
    .height = IMPACT_SIZE};
constexpr game::SpriteFrame IMPACT_WITHOUT_FLASH_FRAME =
    frameUnder(IMPACT_FRAME, SPRITE_SPACING);
constexpr game::SpriteFrame PLAYER_MUZZLE_FLASH_FRAME{
    .left = 0,
    .top = bottomOf(IMPACT_WITHOUT_FLASH_FRAME) + SPRITE_SPACING,
    .width = MUZZLE_FLASH_SIZE,
    .height = MUZZLE_FLASH_SIZE};
constexpr game::SpriteFrame PLAYER_MUZZLE_FLASH_WITHOUT_FLASH_FRAME =
    frameUnder(PLAYER_MUZZLE_FLASH_FRAME, SPRITE_SPACING);
constexpr game::SpriteFrame ENEMY_MUZZLE_FLASH_FRAME{
    .left = 0,
    .top = bottomOf(PLAYER_MUZZLE_FLASH_WITHOUT_FLASH_FRAME) + SPRITE_SPACING,
    .width = MUZZLE_FLASH_SIZE,
    .height = MUZZLE_FLASH_SIZE};
constexpr game::SpriteFrame ENEMY_MUZZLE_FLASH_WITHOUT_FLASH_FRAME =
    frameUnder(ENEMY_MUZZLE_FLASH_FRAME, SPRITE_SPACING);

/** @brief Number of frames of each effect. */
constexpr std::size_t BYDO_EXPLOSION_FRAME_COUNT = 6;
constexpr std::size_t SHIP_EXPLOSION_FRAME_COUNT = 10;
constexpr std::size_t IMPACT_FRAME_COUNT = 3;
constexpr std::size_t MUZZLE_FLASH_FRAME_COUNT = 2;

/**
 * @brief How long each frame of an effect shows. The ship explosion lays its
 * three fireballs two frames, so 120 ms, apart.
 */
constexpr engine::Duration BYDO_EXPLOSION_FRAME_TIME =
    std::chrono::milliseconds(70);
constexpr engine::Duration SHIP_EXPLOSION_FRAME_TIME =
    std::chrono::milliseconds(60);
constexpr engine::Duration IMPACT_FRAME_TIME = std::chrono::milliseconds(50);
constexpr engine::Duration MUZZLE_FLASH_FRAME_TIME =
    std::chrono::milliseconds(50);

/** @brief How long an entity hit stays flashed, or shaded. */
constexpr engine::Duration HIT_FLASH_DURATION = std::chrono::milliseconds(60);

/** @brief The frames of each row of the effect sheet. */
constexpr std::array BYDO_EXPLOSION_FRAMES =
    framesInRow<BYDO_EXPLOSION_FRAME_COUNT>(BYDO_EXPLOSION_FRAME,
                                            SPRITE_SPACING);
constexpr std::array BYDO_EXPLOSION_WITHOUT_FLASH_FRAMES =
    framesInRow<BYDO_EXPLOSION_FRAME_COUNT>(BYDO_EXPLOSION_WITHOUT_FLASH_FRAME,
                                            SPRITE_SPACING);
constexpr std::array SHIP_EXPLOSION_FRAMES =
    framesInRow<SHIP_EXPLOSION_FRAME_COUNT>(SHIP_EXPLOSION_FRAME,
                                            SPRITE_SPACING);
constexpr std::array SHIP_EXPLOSION_WITHOUT_FLASH_FRAMES =
    framesInRow<SHIP_EXPLOSION_FRAME_COUNT>(SHIP_EXPLOSION_WITHOUT_FLASH_FRAME,
                                            SPRITE_SPACING);
constexpr std::array IMPACT_FRAMES =
    framesInRow<IMPACT_FRAME_COUNT>(IMPACT_FRAME, SPRITE_SPACING);
constexpr std::array IMPACT_WITHOUT_FLASH_FRAMES =
    framesInRow<IMPACT_FRAME_COUNT>(IMPACT_WITHOUT_FLASH_FRAME, SPRITE_SPACING);
constexpr std::array PLAYER_MUZZLE_FLASH_FRAMES =
    framesInRow<MUZZLE_FLASH_FRAME_COUNT>(PLAYER_MUZZLE_FLASH_FRAME,
                                          SPRITE_SPACING);
constexpr std::array PLAYER_MUZZLE_FLASH_WITHOUT_FLASH_FRAMES =
    framesInRow<MUZZLE_FLASH_FRAME_COUNT>(
        PLAYER_MUZZLE_FLASH_WITHOUT_FLASH_FRAME, SPRITE_SPACING);
constexpr std::array ENEMY_MUZZLE_FLASH_FRAMES =
    framesInRow<MUZZLE_FLASH_FRAME_COUNT>(ENEMY_MUZZLE_FLASH_FRAME,
                                          SPRITE_SPACING);
constexpr std::array ENEMY_MUZZLE_FLASH_WITHOUT_FLASH_FRAMES =
    framesInRow<MUZZLE_FLASH_FRAME_COUNT>(
        ENEMY_MUZZLE_FLASH_WITHOUT_FLASH_FRAME, SPRITE_SPACING);

/**
 * @brief The effects, each played once: the burst of a Bydo, the three
 * fireballs of a ship, the impact of a missile, and the muzzle flash of a
 * missile of a player or of an enemy.
 */
constexpr EffectAnimations BYDO_EXPLOSION{
    .standard = {.frames = BYDO_EXPLOSION_FRAMES,
                 .frameDuration = BYDO_EXPLOSION_FRAME_TIME,
                 .playback = Animation::Playback::Once},
    .withoutFlash = {.frames = BYDO_EXPLOSION_WITHOUT_FLASH_FRAMES,
                     .frameDuration = BYDO_EXPLOSION_FRAME_TIME,
                     .playback = Animation::Playback::Once}};
constexpr EffectAnimations SHIP_EXPLOSION{
    .standard = {.frames = SHIP_EXPLOSION_FRAMES,
                 .frameDuration = SHIP_EXPLOSION_FRAME_TIME,
                 .playback = Animation::Playback::Once},
    .withoutFlash = {.frames = SHIP_EXPLOSION_WITHOUT_FLASH_FRAMES,
                     .frameDuration = SHIP_EXPLOSION_FRAME_TIME,
                     .playback = Animation::Playback::Once}};
constexpr EffectAnimations IMPACT{
    .standard = {.frames = IMPACT_FRAMES,
                 .frameDuration = IMPACT_FRAME_TIME,
                 .playback = Animation::Playback::Once},
    .withoutFlash = {.frames = IMPACT_WITHOUT_FLASH_FRAMES,
                     .frameDuration = IMPACT_FRAME_TIME,
                     .playback = Animation::Playback::Once}};
constexpr EffectAnimations PLAYER_MUZZLE_FLASH{
    .standard = {.frames = PLAYER_MUZZLE_FLASH_FRAMES,
                 .frameDuration = MUZZLE_FLASH_FRAME_TIME,
                 .playback = Animation::Playback::Once},
    .withoutFlash = {.frames = PLAYER_MUZZLE_FLASH_WITHOUT_FLASH_FRAMES,
                     .frameDuration = MUZZLE_FLASH_FRAME_TIME,
                     .playback = Animation::Playback::Once}};
constexpr EffectAnimations ENEMY_MUZZLE_FLASH{
    .standard = {.frames = ENEMY_MUZZLE_FLASH_FRAMES,
                 .frameDuration = MUZZLE_FLASH_FRAME_TIME,
                 .playback = Animation::Playback::Once},
    .withoutFlash = {.frames = ENEMY_MUZZLE_FLASH_WITHOUT_FLASH_FRAMES,
                     .frameDuration = MUZZLE_FLASH_FRAME_TIME,
                     .playback = Animation::Playback::Once}};

/**
 * @brief How much longer the first frame of an explosion stays, which freezes
 * the victim: 3 frames for a Bydo destroyed, 5 for the fifth kill of a chain,
 * 6 for a ship, whose picture stays white meanwhile.
 */
constexpr engine::Duration KILL_HOLD = std::chrono::milliseconds(50);
constexpr engine::Duration BIG_CATCH_HOLD = std::chrono::milliseconds(83);
constexpr engine::Duration SHIP_DOWN_HOLD = std::chrono::milliseconds(100);

/**
 * @brief A frozen victim is drawn VIBRATION_OFFSET pixels of the canvas away,
 * on the other side every VIBRATION_STEP, one frame, over the first half of
 * its freeze.
 */
constexpr sf::Vector2i VIBRATION_OFFSET(1, 0);
constexpr engine::Duration VIBRATION_STEP = engine::SIMULATION_TICK_DURATION;

/**
 * @brief A Bydo hit is drawn KNOCKBACK_OFFSET pixels of the canvas further
 * along the shot for 3 frames, a ship hit as far the other way; a ship that
 * fires is drawn RECOIL_OFFSET pixels away, backwards, for 2 frames.
 */
constexpr sf::Vector2i KNOCKBACK_OFFSET(1, 0);
constexpr engine::Duration KNOCKBACK_DURATION = std::chrono::milliseconds(50);
constexpr sf::Vector2i RECOIL_OFFSET(-1, 0);
constexpr engine::Duration RECOIL_DURATION = std::chrono::milliseconds(33);

/**
 * @brief The figure of a gain rises POPUP_RISE_HEIGHT pixels of the canvas
 * over POPUP_RISE, stays POPUP_HOLD, blinks POPUP_BLINK_COUNT times,
 * POPUP_BLINK_STEP per state, and goes after POPUP_LIFE.
 */
constexpr engine::Duration POPUP_RISE = std::chrono::milliseconds(400);
constexpr engine::Duration POPUP_HOLD = std::chrono::milliseconds(300);
constexpr engine::Duration POPUP_BLINK_STEP = std::chrono::milliseconds(67);
constexpr int POPUP_BLINK_COUNT = 2;
constexpr int POPUP_RISE_HEIGHT = 8;
constexpr engine::Duration POPUP_LIFE =
    POPUP_RISE + POPUP_HOLD + (2 * POPUP_BLINK_COUNT * POPUP_BLINK_STEP);

/** @brief Written before the points of a gain. */
constexpr const char* GAIN_PREFIX = "+";

/**
 * @brief A ship arriving stays hidden on the first half of
 * MATERIALIZE_DURATION, 16 frames, then shows row by row; its speed lines
 * run as long, on the rows of the canvas SPEED_LINE_ROWS away from its
 * center.
 */
constexpr engine::Duration MATERIALIZE_DURATION =
    std::chrono::milliseconds(267);
constexpr std::array SPEED_LINE_ROWS{-6, -2, 2, 6};

/**
 * @brief Most pixels of the canvas that may show FLASH_COLOR at once: 2.8 %
 * of its 129 600 pixels, the share of a full screen that WCAG 2.3.1 takes for
 * a quarter of a field of view of 10 degrees.
 */
constexpr int FLASH_AREA_MAXIMUM = 3600;

/**
 * @brief Pixels each effect turns FLASH_COLOR, counted in the sheets by the
 * tests: the most a frame of the effect changes between its row and its row
 * without flash, over the frames that change any (the first of a Bydo
 * explosion and of a muzzle flash, the first SHIP_EXPLOSION_FLASH_FRAMES of a
 * ship explosion, the first IMPACT_FLASH_FRAMES of an impact), and the
 * largest frame of a Bydo and of a ship drawn as a silhouette.
 */
constexpr int BYDO_EXPLOSION_FLASH_AREA = 275;
constexpr int SHIP_EXPLOSION_FLASH_AREA = 23;
constexpr int SHIP_EXPLOSION_FLASH_FRAMES = 8;
constexpr int IMPACT_FLASH_AREA = 6;
constexpr int IMPACT_FLASH_FRAMES = 2;
constexpr int MUZZLE_FLASH_AREA = 12;
constexpr int BYDO_SILHOUETTE_AREA = 367;
constexpr int SHIP_SILHOUETTE_AREA = 254;

/**
 * @brief Two kills less than KILL_CHAIN_WINDOW apart, by any pilot, are of
 * the same chain; every BIG_CATCH_EVERY-th kill of a chain is a big catch.
 */
constexpr engine::Duration KILL_CHAIN_WINDOW = std::chrono::milliseconds(1000);
constexpr int BIG_CATCH_EVERY = 5;

/** @brief A kill and a gain of score noted less than GAIN_MATCH_WINDOW
 * apart, three world states, make the figure of the gain. */
constexpr engine::Duration GAIN_MATCH_WINDOW = std::chrono::milliseconds(150);

}  // namespace rtype::client
