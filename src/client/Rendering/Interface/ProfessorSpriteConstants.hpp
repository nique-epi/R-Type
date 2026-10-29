#pragma once

#include <array>
#include "Animation.hpp"
#include "EntitySpriteConstants.hpp"
#include "SheetRow.hpp"
#include "SpriteFrame.hpp"

namespace rtype::client {

/**
 * @brief Asset id of the sheet of Professor Bydo, the guide of the help: the
 * Bydo of the entity sheet under a mortarboard, round glasses over its eye.
 *
 * Its two frames face left in the first row, as the entity sheet draws the
 * Bydo, and the same two mirrored in the second row, since the interface
 * canvas cannot mirror an image. Like the entity sheet, its frames are at
 * their native size, SPRITE_SPACING apart.
 */
constexpr const char* PROFESSOR_SHEET_ID = "sprites/professor.png";

constexpr game::SpriteFrame PROFESSOR_FRAME{
    .left = 0, .top = 0, .width = 24, .height = 28};

constexpr std::array PROFESSOR_FACING_LEFT_FRAMES =
    framesInRow<SPRITE_FRAME_COUNT>(PROFESSOR_FRAME, SPRITE_SPACING);
constexpr std::array PROFESSOR_FACING_RIGHT_FRAMES =
    framesInRow<SPRITE_FRAME_COUNT>(frameUnder(PROFESSOR_FRAME, SPRITE_SPACING),
                                    SPRITE_SPACING);

/** @brief Each row shows its frames in turn, BYDO_FRAME_TIME each, as the
 * Bydo it is drawn from. */
constexpr Animation PROFESSOR_FACING_LEFT_ANIMATION{
    .frames = PROFESSOR_FACING_LEFT_FRAMES,
    .frameDuration = BYDO_FRAME_TIME,
    .playback = Animation::Playback::Loop};
constexpr Animation PROFESSOR_FACING_RIGHT_ANIMATION{
    .frames = PROFESSOR_FACING_RIGHT_FRAMES,
    .frameDuration = BYDO_FRAME_TIME,
    .playback = Animation::Playback::Loop};

}  // namespace rtype::client
