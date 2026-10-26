#pragma once

#include <span>
#include <vector>
#include "IntroCast.hpp"
#include "IntroCue.hpp"
#include "IntroFreeze.hpp"

namespace rtype::client {

/**
 * @returns What the intro plays, sorted by moment of the scene: the growl of
 * the Bydo in the dark, a flyby at each close-up, the shot of every missile
 * of the cast, the explosion and the shake of the prey and of each Bydo of
 * the helix, the whoosh of the squadron and of the reinforcements, the heavy
 * impact of the first kill, the charge, the beams and their shake, the big
 * explosion, its aftershock, the boost, and the impact of the logo.
 *
 * @param freezes The freezes the scene goes through, none with REDUCE
 * FLASHES: the impact of the logo is a moment of the music, turned into a
 * moment of the scene through them.
 */
[[nodiscard]] std::vector<IntroCue> introCues(
    const IntroCast& cast, std::span<const IntroFreeze> freezes);

}  // namespace rtype::client
