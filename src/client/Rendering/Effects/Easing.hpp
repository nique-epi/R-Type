#pragma once

#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns 1 − (1 − progress)², for @p progress clamped to 0 to 1: fast
 * first, then slow. The rise of the figure of a gain. */
[[nodiscard]] float easeOutQuad(float progress);

/** @returns 1 − (1 − progress)³, for @p progress clamped to 0 to 1: faster
 * first, then slower. A shockwave, a speed line, the end of the warp of the
 * sky. */
[[nodiscard]] float easeOutCubic(float progress);

/** @returns The share of @p whole that @p part is, not clamped: the progress
 * an easing reads. @p whole must be positive. */
[[nodiscard]] float shareOf(engine::Duration part, engine::Duration whole);

}  // namespace rtype::client
