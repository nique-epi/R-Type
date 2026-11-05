#pragma once

#include "EntityAppearance.hpp"

namespace rtype::client::testing {

/** @returns The appearance of the enemy: one idle frame, and the strike of
 * DESCRIBED_ATTACK in two frames, held, with an anchor, then back to idle. */
[[nodiscard]] EntityAppearance appearanceWithAStrike();

}  // namespace rtype::client::testing
