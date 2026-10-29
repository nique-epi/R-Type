#include <gtest/gtest.h>
#include <optional>
#include "DisplayMode.hpp"
#include "WindowFrame.hpp"
#include "WindowLayout.hpp"
#include "WindowLayoutTestConstants.hpp"

using rtype::client::DisplayMode;
using rtype::client::WindowFrame;
using rtype::client::WindowLayout;
using rtype::client::testing::DESKTOP_1920_BY_1080;
using rtype::client::testing::DESKTOP_TOP_LEFT;
using rtype::client::testing::LARGEST_SIZE_CENTERED_AT;
using rtype::client::testing::LARGEST_SIZE_THAT_FITS;
using rtype::client::testing::SAVED_SIZE;
using rtype::client::testing::SMALLEST_SIZE_INDEX;

/**
 * Given a windowed window on a 1920 x 1080 desktop, no size saved
 * When its frame is asked
 * Then it is 1600 x 900, the largest size that fits, centered on the desktop
 */
TEST(WindowLayout, AWindowedWindowTakesTheSelectedSizeCentered) {
  const WindowLayout layout(DESKTOP_1920_BY_1080, DisplayMode::Windowed,
                            std::nullopt);

  const WindowFrame frame = layout.frame();

  EXPECT_EQ(frame.size, LARGEST_SIZE_THAT_FITS);
  EXPECT_EQ(frame.position.x, LARGEST_SIZE_CENTERED_AT.x);
  EXPECT_EQ(frame.position.y, LARGEST_SIZE_CENTERED_AT.y);
}

/**
 * Given a borderless window on a 1920 x 1080 desktop
 * When its frame is asked
 * Then it covers the whole desktop from its top-left corner
 */
TEST(WindowLayout, ABorderlessWindowCoversTheWholeDesktop) {
  const WindowLayout layout(DESKTOP_1920_BY_1080, DisplayMode::Borderless,
                            std::nullopt);

  const WindowFrame frame = layout.frame();

  EXPECT_EQ(frame.size, DESKTOP_1920_BY_1080);
  EXPECT_EQ(frame.position.x, DESKTOP_TOP_LEFT.x);
  EXPECT_EQ(frame.position.y, DESKTOP_TOP_LEFT.y);
}

/**
 * Given a borderless window on a 1920 x 1080 desktop
 * When the size it imposes is asked
 * Then it is the size of the desktop
 */
TEST(WindowLayout, ABorderlessWindowImposesTheSizeOfTheDesktop) {
  const WindowLayout layout(DESKTOP_1920_BY_1080, DisplayMode::Borderless,
                            std::nullopt);

  EXPECT_EQ(layout.imposedSize(), DESKTOP_1920_BY_1080);
}

/**
 * Given a windowed window
 * When the size it imposes is asked
 * Then there is none: the player picks the size
 */
TEST(WindowLayout, AWindowedWindowImposesNoSize) {
  const WindowLayout layout(DESKTOP_1920_BY_1080, DisplayMode::Windowed,
                            std::nullopt);

  EXPECT_FALSE(layout.imposedSize().has_value());
}

/**
 * Given a borderless window, 1600 x 900 selected
 * When the smallest size is selected
 * Then the selection is refused, and 1600 x 900 stays selected
 */
TEST(WindowLayout, NoSizeIsSelectedWhileBorderless) {
  WindowLayout layout(DESKTOP_1920_BY_1080, DisplayMode::Borderless,
                      std::nullopt);

  const bool selected = layout.selectSize(SMALLEST_SIZE_INDEX);

  EXPECT_FALSE(selected);
  EXPECT_EQ(layout.sizes().selected(), LARGEST_SIZE_THAT_FITS);
}

/**
 * Given a windowed window at the saved 1280 x 720
 * When it is made borderless, then windowed again
 * Then it takes 1280 x 720 again
 */
TEST(WindowLayout, TheSavedSizeComesBackOnceWindowedAgain) {
  WindowLayout layout(DESKTOP_1920_BY_1080, DisplayMode::Windowed, SAVED_SIZE);

  layout.selectDisplayMode(DisplayMode::Borderless);
  layout.selectDisplayMode(DisplayMode::Windowed);

  EXPECT_EQ(layout.frame().size, SAVED_SIZE);
}

/**
 * Given a windowed window
 * When windowed is asked again
 * Then the request is refused: the window is not opened again
 */
TEST(WindowLayout, AskingForTheCurrentModeIsRefused) {
  WindowLayout layout(DESKTOP_1920_BY_1080, DisplayMode::Windowed,
                      std::nullopt);

  EXPECT_FALSE(layout.selectDisplayMode(DisplayMode::Windowed));
}
