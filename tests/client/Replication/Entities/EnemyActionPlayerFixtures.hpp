#pragma once

#include "EntityAppearance.hpp"

namespace rtype::client::testing {

/** @returns The appearance of the enemy: one idle frame, and the strike of
 * DESCRIBED_ATTACK in two frames, held, with an anchor, then back to idle. */
[[nodiscard]] EntityAppearance appearanceWithAStrike();

/** @returns appearanceWithAStrike() with an enraged idle frame, and a rage:
 * the attack RAGE_ATTACK in the phase Enraged, two frames held, then back to
 * idle. */
[[nodiscard]] EntityAppearance appearanceWithARage();

}  // namespace rtype::client::testing
