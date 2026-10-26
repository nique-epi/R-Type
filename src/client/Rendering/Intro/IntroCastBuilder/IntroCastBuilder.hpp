#pragma once

#include <cstdint>
#include "IntroCast.hpp"

namespace rtype::client {

/**
 * @returns What the intro shows that is fixed when it starts, the same for
 * every run of one seed: the shot of the Bydo in the dark; the missile, the
 * burst and the shake of the prey; for each Bydo of the helix, the curved
 * missile of the ship closest in height, its burst and its ring; the two
 * shots of the helix at the swap; the bursts of the newcomers the beams hit;
 * and the big burst and the big ring of the ring.
 */
[[nodiscard]] IntroCast buildIntroCast(std::uint32_t seed);

}  // namespace rtype::client
