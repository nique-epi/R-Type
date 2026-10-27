#pragma once

#include "BookExit.hpp"
#include "BookPose.hpp"
#include "FlightLogSchedule.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @returns Where the book lies @p sinceStart after the screen opened: it
 * drops closed, opens at the openStart of @p schedule while its hinge moves
 * to the middle, then, once @p exit has a choice, closes and leaves. */
[[nodiscard]] BookPose bookPoseAt(engine::Duration sinceStart,
                                  const FlightLogSchedule& schedule,
                                  const BookExit& exit);

}  // namespace rtype::client
