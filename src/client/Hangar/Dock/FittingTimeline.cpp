#include "FittingTimeline.hpp"
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include "ArmPose.hpp"
#include "Easing.hpp"
#include "FittingRoute.hpp"
#include "FittingRun.hpp"
#include "FittingTimelineConstants.hpp"
#include "ShipLook.hpp"
#include "ShipLooks.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The eased share of the phase from @p start to @p end at
 * @p progress. */
float eased(float progress, float start, float end) {
  return easeInOutCubic(progressThrough(progress, start, end - start));
}

/** @returns The pixel @p share of the way from @p from to @p to. */
sf::Vector2i between(sf::Vector2i from, sf::Vector2i to, float share) {
  return from + sf::Vector2i(pixelsOf(to.x - from.x, share),
                             pixelsOf(to.y - from.y, share));
}

/** @returns What the arm does on its way out: it lets go of the hull,
 * reaches off screen and closes on the item there. */
ArmPose outboundPoseAt(float progress, const FittingRoute& route) {
  if (progress < FITTING_RELEASE_END) {
    return {.target = route.grip,
            .open = GRIPPER_OPEN *
                    progressThrough(progress, 0.0F, FITTING_RELEASE_END)};
  }
  if (progress < FITTING_REACH_OUT_END) {
    return {.target = between(
                route.grip, route.away,
                eased(progress, FITTING_RELEASE_END, FITTING_REACH_OUT_END)),
            .open = GRIPPER_OPEN};
  }
  const bool grabbed = progress >= FITTING_GRAB_AT;
  return {.target = route.away,
          .open = grabbed ? 0.0F : GRIPPER_OPEN,
          .carrying = grabbed};
}

/** @returns What the arm does with the item: brings it above its mount, sets
 * it down, lets go of it. */
ArmPose placingPoseAt(float progress, const FittingRoute& route) {
  const sf::Vector2i hover = route.place + sf::Vector2i(0, route.approach);
  if (progress < FITTING_HOVER_END) {
    return {.target =
                between(route.away, hover,
                        eased(progress, FITTING_LEAVE_END, FITTING_HOVER_END)),
            .carrying = true};
  }
  if (progress < FITTING_PLACE_AT) {
    return {
        .target = between(hover, route.place,
                          eased(progress, FITTING_HOVER_END, FITTING_PLACE_AT)),
        .carrying = true};
  }
  return {.target = route.place,
          .open = GRIPPER_OPEN *
                  progressThrough(progress, FITTING_PLACE_AT,
                                  FITTING_LET_GO_END - FITTING_PLACE_AT)};
}

/** @returns What the arm does on its way back: it goes back to the hull and
 * closes on it. */
ArmPose returningPoseAt(float progress, const FittingRoute& route) {
  if (progress < FITTING_BACK_END) {
    return {.target =
                between(route.place, route.grip,
                        eased(progress, FITTING_LET_GO_END, FITTING_BACK_END)),
            .open = GRIPPER_OPEN};
  }
  return {
      .target = route.grip,
      .open = GRIPPER_OPEN * (1.0F - progressThrough(progress, FITTING_BACK_END,
                                                     1.0F - FITTING_BACK_END))};
}

}  // namespace

float fittingProgressOf(const FittingRun& run, engine::Duration now) {
  return std::clamp(shareOf(now - run.started, FITTING_DURATION), 0.0F, 1.0F);
}

ArmPose fittingPoseAt(float progress, const FittingRoute& route) {
  if (progress < FITTING_LEAVE_END) {
    return outboundPoseAt(progress, route);
  }
  if (progress < FITTING_LET_GO_END) {
    return placingPoseAt(progress, route);
  }
  return returningPoseAt(progress, route);
}

ShipLook lookShownAt(float progress, const FittingRun& run) {
  if (progress >= FITTING_PLACE_AT) {
    return wearing(run.before, run.item);
  }
  if (progress >= FITTING_HOVER_END) {
    return without(wearing(run.before, run.item), run.item);
  }
  return run.before;
}

float sparksShareAt(float progress, float start) {
  return (progress - start) / SPARKS_LENGTH;
}

}  // namespace rtype::client
