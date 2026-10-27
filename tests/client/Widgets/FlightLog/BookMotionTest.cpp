#include <gtest/gtest.h>
#include "BookExit.hpp"
#include "BookMotion.hpp"
#include "BookMotionTestConstants.hpp"
#include "BookPose.hpp"
#include "EndChoice.hpp"
#include "FlightLogFixtures.hpp"
#include "FlightLogSchedule.hpp"
#include "FlightLogTestConstants.hpp"
#include "FlightLogTimingConstants.hpp"
#include "TimeTestConstants.hpp"

using rtype::client::BookExit;
using rtype::client::BookPose;
using rtype::client::bookPoseAt;
using rtype::client::EndChoice;
using rtype::client::EXIT_CLOSE_TIME;
using rtype::client::EXIT_TIME;
using rtype::client::FlightLogSchedule;
using rtype::client::testing::BOOK_HEIGHT;
using rtype::client::testing::CLOSED_LEFT;
using rtype::client::testing::MIDDLE_COLUMN;
using rtype::client::testing::PICTURE_HEIGHT;
using rtype::client::testing::PICTURE_WIDTH;
using rtype::client::testing::THREE_PILOTS;

/**
 * Given the screen just opened
 * When the book is placed
 * Then it lies wholly above the picture
 */
TEST(BookMotion, TheBookStartsAboveThePicture) {
  const BookPose pose = bookPoseAt({}, designScheduleOf(THREE_PILOTS), {});
  EXPECT_LE(pose.top + BOOK_HEIGHT, 0);
}

/**
 * Given the moment just before the cover turns
 * When the book is placed
 * Then it lies closed in the middle of the picture
 */
TEST(BookMotion, TheClosedBookLiesInTheMiddle) {
  const FlightLogSchedule schedule = designScheduleOf(THREE_PILOTS);
  const BookPose pose =
      bookPoseAt(schedule.openStart - ONE_NANOSECOND, schedule, {});
  EXPECT_EQ(pose.open, 0.0F);
  EXPECT_EQ(pose.hinge, CLOSED_LEFT);
}

/**
 * Given the moment the book is open
 * When it is placed
 * Then its hinge is the middle of the picture
 */
TEST(BookMotion, TheOpenBookIsCentered) {
  const FlightLogSchedule schedule = designScheduleOf(THREE_PILOTS);
  const BookPose pose = bookPoseAt(schedule.opened, schedule, {});
  EXPECT_EQ(pose.open, 1.0F);
  EXPECT_EQ(pose.hinge, MIDDLE_COLUMN);
}

/**
 * Given a choice made, the book closing for 0.45 s
 * When it is placed once it closed
 * Then it is closed again
 */
TEST(BookMotion, TheBookClosesBeforeItLeaves) {
  const FlightLogSchedule schedule = designScheduleOf(THREE_PILOTS);
  const BookPose pose =
      bookPoseAt(schedule.ready, schedule,
                 BookExit{.choice = EndChoice::Replay, .age = EXIT_CLOSE_TIME});
  EXPECT_EQ(pose.open, 0.0F);
}

/**
 * Given QUIT chosen
 * When the book has left
 * Then it lies below the picture
 */
TEST(BookMotion, AfterQuitTheBookFallsOffTheBottom) {
  const FlightLogSchedule schedule = designScheduleOf(THREE_PILOTS);
  const BookPose pose =
      bookPoseAt(schedule.ready, schedule,
                 BookExit{.choice = EndChoice::Quit, .age = EXIT_TIME});
  EXPECT_GE(pose.top, PICTURE_HEIGHT);
}

/**
 * Given PLAY AGAIN chosen
 * When the book has left
 * Then it lies right of the picture
 */
TEST(BookMotion, AfterPlayAgainTheBookSlidesOffTheRight) {
  const FlightLogSchedule schedule = designScheduleOf(THREE_PILOTS);
  const BookPose pose =
      bookPoseAt(schedule.ready, schedule,
                 BookExit{.choice = EndChoice::Replay, .age = EXIT_TIME});
  EXPECT_GE(pose.hinge, PICTURE_WIDTH);
}
