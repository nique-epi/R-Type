#pragma once

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

}  // namespace rtype::client
