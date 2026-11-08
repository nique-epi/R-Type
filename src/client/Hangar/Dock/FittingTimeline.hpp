#pragma once

#include "ArmPose.hpp"
#include "FittingRoute.hpp"
#include "FittingRun.hpp"
#include "ShipLook.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns How far @p run is at @p now, on the clock of the screen: 0 to 1,
 * 1 from FITTING_DURATION on. */
[[nodiscard]] float fittingProgressOf(const FittingRun& run,
                                      engine::Duration now);

/** @returns What the fitting arm does at @p progress on @p route. */
[[nodiscard]] ArmPose fittingPoseAt(float progress, const FittingRoute& route);

/** @returns What the ship shows at @p progress of @p run: what it wore, then
 * the slot of the item empty from FITTING_HOVER_END, then the item from
 * FITTING_PLACE_AT. */
[[nodiscard]] ShipLook lookShownAt(float progress, const FittingRun& run);

/** @returns How far the sparks of an event at @p start are at @p progress:
 * under 0 before, 0 to 1 while they show, over 1 after. */
[[nodiscard]] float sparksShareAt(float progress, float start);

}  // namespace rtype::client
