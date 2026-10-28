#include <gtest/gtest.h>
#include "ScreenOff.hpp"
#include "ScreenOffLook.hpp"
#include "ScreenOffTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::screenOffAt;
using rtype::client::ScreenOffLook;
using rtype::engine::Duration;

/**
 * Given a screen that has just started to turn off
 * When its look is read
 * Then the whole picture shows
 */
TEST(ScreenOff, StartsOnTheWholePicture) {
  EXPECT_EQ(
      screenOffAt(Duration::zero()),
      (ScreenOffLook{.pictureRows = WHOLE_PICTURE_ROWS, .lineHalfWidth = 0}));
}

/**
 * Given a screen at the end of its collapse
 * When its look is read
 * Then the picture is gone and the line crosses the whole picture
 */
TEST(ScreenOff, EndsItsCollapseOnAWholeLine) {
  EXPECT_EQ(screenOffAt(COLLAPSE_END),
            (ScreenOffLook{.pictureRows = 0,
                           .lineHalfWidth = WHOLE_LINE_HALF_WIDTH}));
}

/**
 * Given a screen one millisecond before it goes dark
 * When its look is read
 * Then a line of at least one pixel each side remains
 */
TEST(ScreenOff, KeepsALineUntilItGoesDark) {
  EXPECT_GE(screenOffAt(JUST_BEFORE_DARK).lineHalfWidth, 1);
}

/**
 * Given a screen that closed its line
 * When its look is read
 * Then nothing shows: the screen is dark
 */
TEST(ScreenOff, IsDarkOnceTheLineClosed) {
  EXPECT_EQ(screenOffAt(DARK), ScreenOffLook{});
}
