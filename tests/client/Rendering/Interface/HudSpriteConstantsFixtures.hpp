#pragma once

#include <vector>
#include "SpriteFrame.hpp"

/** @returns Every frame of the HUD sheet: the lives, the life lost, the
 * markers. */
[[nodiscard]] std::vector<rtype::game::SpriteFrame> everyHudFrame();
