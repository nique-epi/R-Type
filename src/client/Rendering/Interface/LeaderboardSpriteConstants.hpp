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
 * @brief Asset id of the sheet of the leaderboard: a row of a banner in the
 * wind, then the guard of the court, then the guard mirrored, since the
 * interface canvas cannot mirror an image. Frames at their native size,
 * SPRITE_SPACING apart.
 */
constexpr const char* LEADERBOARD_SHEET_ID = "sprites/leaderboard.png";

constexpr std::size_t FLAG_FRAME_COUNT = 32;
constexpr game::SpriteFrame FLAG_FRAME{
    .left = 0, .top = 0, .width = 52, .height = 99};
constexpr std::array FLAG_FRAMES =
    framesInRow<FLAG_FRAME_COUNT>(FLAG_FRAME, SPRITE_SPACING);

/** @brief 3 ticks of the simulation: as long as IMPACT_FRAME_TIME. A loop lasts
 * 1.6 s, and the last frame leads to the first without a jump. */
constexpr engine::Duration FLAG_FRAME_TIME = std::chrono::milliseconds(50);
constexpr Animation FLAG_ANIMATION{.frames = FLAG_FRAMES,
                                   .frameDuration = FLAG_FRAME_TIME,
                                   .playback = Animation::Playback::Loop};

constexpr game::SpriteFrame GUARD_FRAME{
    .left = 0,
    .top = bottomOf(FLAG_FRAME) + SPRITE_SPACING,
    .width = 28,
    .height = 30};
constexpr std::array GUARD_FACING_LEFT_FRAMES =
    framesInRow<SPRITE_FRAME_COUNT>(GUARD_FRAME, SPRITE_SPACING);
constexpr std::array GUARD_FACING_RIGHT_FRAMES =
    framesInRow<SPRITE_FRAME_COUNT>(frameUnder(GUARD_FRAME, SPRITE_SPACING),
                                    SPRITE_SPACING);

/** @brief Each row shows its frames in turn, BYDO_FRAME_TIME each, as the Bydo
 * it is drawn from. */
constexpr Animation GUARD_FACING_LEFT_ANIMATION{
    .frames = GUARD_FACING_LEFT_FRAMES,
    .frameDuration = BYDO_FRAME_TIME,
    .playback = Animation::Playback::Loop};
constexpr Animation GUARD_FACING_RIGHT_ANIMATION{
    .frames = GUARD_FACING_RIGHT_FRAMES,
    .frameDuration = BYDO_FRAME_TIME,
    .playback = Animation::Playback::Loop};

}  // namespace rtype::client
