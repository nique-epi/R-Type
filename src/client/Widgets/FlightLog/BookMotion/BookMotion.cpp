#include "BookMotion.hpp"
#include <algorithm>
#include "BookExit.hpp"
#include "BookPose.hpp"
#include "Easing.hpp"
#include "EndChoice.hpp"
#include "FlightLogLayoutConstants.hpp"
#include "FlightLogSchedule.hpp"
#include "FlightLogTimingConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns How far the book has left once it closed, from 0 to 1. */
float leavingOf(const BookExit& exit) {
  return easeInCubic(
      progressBetween(exit.age, EXIT_CLOSE_TIME, EXIT_MOVE_TIME));
}

}  // namespace

BookPose bookPoseAt(engine::Duration sinceStart,
                    const FlightLogSchedule& schedule, const BookExit& exit) {
  float open = easeInOutCubic(
      progressBetween(sinceStart, schedule.openStart, OPEN_TIME));
  int top = BOOK_TOP - pixelsOf(DROP_HEIGHT,
                                1.0F - easeOutBack(progressBetween(
                                           sinceStart, DROP_START, DROP_TIME)));
  int slide = 0;
  if (exit.choice.has_value()) {
    open = std::min(open, 1.0F - easeInOutCubic(progressBetween(
                                     exit.age, {}, EXIT_CLOSE_TIME)));
    if (*exit.choice == EndChoice::Quit) {
      top += pixelsOf(EXIT_FALL, leavingOf(exit));
    } else {
      slide = pixelsOf(EXIT_SLIDE, leavingOf(exit));
    }
  }
  return BookPose{
      .hinge = CLOSED_HINGE + pixelsOf(OPEN_HINGE - CLOSED_HINGE, open) + slide,
      .top = top,
      .open = open};
}

}  // namespace rtype::client
