#pragma once

#include "BeamLine.hpp"
#include "Bounds.hpp"

namespace rtype::game {

/**
 * @returns Whether @p beam and @p box overlap: no axis among the two of the
 * box and the two of the beam separates them (the separating axis theorem for
 * two rectangles). Edges that only touch are apart, as engine::overlaps()
 * holds them.
 */
[[nodiscard]] bool beamOverlaps(const BeamLine& beam,
                                const engine::Bounds& box);

}  // namespace rtype::game
