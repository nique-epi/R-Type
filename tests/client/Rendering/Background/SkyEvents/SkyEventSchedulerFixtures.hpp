#pragma once

#include "SkyEventScheduler.hpp"

/** @returns How many frames of FRAME @p seconds hold, rounded to the nearest.
 */
[[nodiscard]] int framesIn(float seconds);

/** @brief Advances @p scheduler by @p seconds of real time, in frames of
 * FRAME. */
void advanceFor(rtype::client::SkyEventScheduler& scheduler, float seconds);
