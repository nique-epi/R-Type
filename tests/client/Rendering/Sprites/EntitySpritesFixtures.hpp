#pragma once

#include "EntityAppearance.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteFrame.hpp"

/** @returns The appearance the tests draw, built from TEST_SHEET_ID,
 * TEST_IDLE_FRAMES and TEST_FRAME_TIME; it lives for the whole run, so the
 * animations that view its frames never dangle. */
[[nodiscard]] const rtype::client::EntityAppearance& testAppearance();

/** @returns The frame an animation shows where it starts. */
rtype::game::SpriteFrame startFrameOf(
    const rtype::client::SpriteAnimation& animation);
