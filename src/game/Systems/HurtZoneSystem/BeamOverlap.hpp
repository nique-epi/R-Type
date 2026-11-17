#pragma once

#include "BeamLine.hpp"
#include "Bounds.hpp"
#include "ContactShape.hpp"
#include "PlacedMask.hpp"

namespace rtype::game {

/**
 * @returns Whether @p beam and @p box overlap: no axis among the two of the
 * box and the two of the beam separates them (the separating axis theorem for
 * two rectangles). Edges that only touch are apart, as engine::overlaps()
 * holds them.
 */
[[nodiscard]] bool beamOverlaps(const BeamLine& beam,
                                const engine::Bounds& box);

/** @returns Whether @p beam covers a drawn pixel of @p placed, each pixel
 * tested as the square of the grid it covers (cellBoundsOf()), as
 * beamOverlaps() tests a box. */
[[nodiscard]] bool beamTouchesMask(const BeamLine& beam,
                                   const engine::PlacedMask& placed);

/**
 * @returns Whether @p beam touches @p shape: it overlaps its box, then, when
 * the shape has a mask, one of its drawn pixels.
 *
 * The box is tested first, so a beam far from the body reads no pixel: at
 * most one test per pixel of the mask, for a beam that already crosses the
 * box.
 */
[[nodiscard]] bool beamTouchesShape(const BeamLine& beam,
                                    const ContactShape& shape);

}  // namespace rtype::game
