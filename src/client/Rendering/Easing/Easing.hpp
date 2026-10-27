#pragma once

#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns 1 − (1 − progress)², for @p progress clamped to 0 to 1: fast
 * first, then slow. The rise of the figure of a gain. */
[[nodiscard]] float easeOutQuad(float progress);

/** @returns 1 − (1 − progress)³, for @p progress clamped to 0 to 1: faster
 * first, then slower. A shockwave, a speed line, the end of the warp of the
 * sky, the flight of the ships of the title. */
[[nodiscard]] float easeOutCubic(float progress);

/** @returns The share of @p whole that @p part is, not clamped: the progress
 * an easing reads. @p whole must be positive. */
[[nodiscard]] float shareOf(engine::Duration part, engine::Duration whole);

/** @returns progress³, for @p progress clamped to 0 to 1: slow first, then
 * fast. The exit of the ships at the boost, the opening of the bands of the
 * intro. */
[[nodiscard]] float easeInCubic(float progress);

/** @returns 4 progress³ under one half and 1 − (2 − 2 progress)³ / 2 from
 * it, for @p progress clamped to 0 to 1: slow, fast, then slow again. The
 * swap of two ships of the intro. */
[[nodiscard]] float easeInOutCubic(float progress);

/** @returns 1 + 2 (progress − 1)³ + (progress − 1)², for @p progress clamped
 * to 0 to 1: past 1 near two thirds (1 + 1/27 at 2/3), then back to 1. The
 * entry of the squadron of the intro and the slam of the logo. */
[[nodiscard]] float easeOutBack(float progress);

/** @returns How far @p time is through the span of @p length from @p start,
 * clamped to 0 to 1; a span of no length gives 0 before its start and 1 from
 * it. Times in seconds. */
[[nodiscard]] float progressThrough(float time, float start, float length);

/** @returns How far @p now is from @p start to @p start + @p length, from 0
 * to 1: 0 before, 1 after; with no length, 1 from @p start on. The same as
 * progressThrough(), on the durations of the clock. */
[[nodiscard]] float progressBetween(engine::Duration now,
                                    engine::Duration start,
                                    engine::Duration length);

/** @returns @p distance times @p share, rounded to a whole pixel: how far
 * something has gone along a distance at a share of its way. */
[[nodiscard]] int pixelsOf(int distance, float share);

}  // namespace rtype::client
