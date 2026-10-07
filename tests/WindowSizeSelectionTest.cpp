#include <gtest/gtest.h>
#include <cstddef>
#include "PixelPosition.hpp"
#include "PixelSize.hpp"
#include "PlayfieldConstants.hpp"
#include "PlayfieldViewport.hpp"
#include "WindowConstants.hpp"
#include "WindowSizeSelection.hpp"

using rtype::client::fitPlayfieldInWindow;
using rtype::client::PixelPosition;
using rtype::client::PixelSize;
using rtype::client::PlayfieldViewport;
using rtype::client::WINDOW_SIZES;
using rtype::client::WindowSizeSelection;
using rtype::game::PLAYFIELD_HEIGHT;
using rtype::game::PLAYFIELD_WIDTH;

namespace {

constexpr std::size_t SMALLEST = 0;
constexpr std::size_t LARGEST = WINDOW_SIZES.size() - 1;
constexpr float WHOLE = 1.0F;

constexpr PixelSize LAPTOP_DESKTOP{.width = 1728, .height = 1117};
constexpr PixelSize AS_WIDE_AS_FULL_HD_DESKTOP{.width = 1920, .height = 1200};
constexpr PixelSize AS_HIGH_AS_FULL_HD_DESKTOP{.width = 2000, .height = 1080};
constexpr PixelSize HUGE_DESKTOP{.width = 3840, .height = 2160};
constexpr PixelSize TINY_DESKTOP{.width = 800, .height = 500};
constexpr PixelSize ROOMY_DESKTOP{.width = 2000, .height = 1100};

constexpr unsigned int WIDTH_BELOW_FULL_HD = 1600;
constexpr unsigned int HEIGHT_BELOW_FULL_HD = 900;
constexpr int HALF_FREE_WIDTH_ON_ROOMY = 40;
constexpr int HALF_FREE_HEIGHT_ON_ROOMY = 10;

}  // namespace

/**
 * Given the sizes offered to the player
 * When each is compared with the playfield
 * Then each has the proportions of the playfield
 */
TEST(WindowSizes, EverySizeHasTheProportionsOfThePlayfield) {
  for (const PixelSize size : WINDOW_SIZES) {
    EXPECT_FLOAT_EQ(static_cast<float>(size.width) * PLAYFIELD_HEIGHT,
                    static_cast<float>(size.height) * PLAYFIELD_WIDTH)
        << size.width << "x" << size.height;
  }
}

/**
 * Given the sizes offered to the player
 * When the playfield is fitted in a window of each size
 * Then the playfield covers the whole window, leaving no black bar
 */
TEST(WindowSizes, NoSizeLeavesABlackBar) {
  for (const PixelSize size : WINDOW_SIZES) {
    const PlayfieldViewport viewport =
        fitPlayfieldInWindow(size.width, size.height);

    EXPECT_FLOAT_EQ(viewport.width, WHOLE) << size.width << "x" << size.height;
    EXPECT_FLOAT_EQ(viewport.height, WHOLE) << size.width << "x" << size.height;
  }
}

/**
 * Given a desktop of 1728 by 1117 pixels
 * When the selection is created
 * Then 1600 by 900, the largest size that fits, is selected
 */
TEST(WindowSizeSelection, LargestSizeFittingTheDesktopIsSelectedAtStart) {
  const WindowSizeSelection selection(LAPTOP_DESKTOP);

  EXPECT_EQ(selection.selected().width, WIDTH_BELOW_FULL_HD);
  EXPECT_EQ(selection.selected().height, HEIGHT_BELOW_FULL_HD);
}

/**
 * Given a desktop exactly as wide as the 1920 by 1080 size, and higher
 * When the selection is created
 * Then that size is skipped and 1600 by 900 is selected
 */
TEST(WindowSizeSelection, SizeAsWideAsTheDesktopIsNotAvailable) {
  const WindowSizeSelection selection(AS_WIDE_AS_FULL_HD_DESKTOP);

  EXPECT_EQ(selection.selected().width, WIDTH_BELOW_FULL_HD);
  EXPECT_EQ(selection.selected().height, HEIGHT_BELOW_FULL_HD);
}

/**
 * Given a desktop exactly as high as the 1920 by 1080 size, and wider
 * When the selection is created
 * Then that size is skipped and 1600 by 900 is selected
 */
TEST(WindowSizeSelection, SizeAsHighAsTheDesktopIsNotAvailable) {
  const WindowSizeSelection selection(AS_HIGH_AS_FULL_HD_DESKTOP);

  EXPECT_EQ(selection.selected().width, WIDTH_BELOW_FULL_HD);
  EXPECT_EQ(selection.selected().height, HEIGHT_BELOW_FULL_HD);
}

/**
 * Given a desktop smaller than every size
 * When the selection is created
 * Then the smallest size is selected, although it is not available
 */
TEST(WindowSizeSelection, SmallestSizeIsSelectedWhenNoneFits) {
  const WindowSizeSelection selection(TINY_DESKTOP);

  EXPECT_EQ(selection.selectedIndex(), SMALLEST);
  EXPECT_FALSE(selection.isAvailable(SMALLEST));
}

/**
 * Given a desktop larger than every size
 * When the smallest size is selected
 * Then it becomes the selected size
 */
TEST(WindowSizeSelection, SelectingAnAvailableSizeMakesItTheSelectedOne) {
  WindowSizeSelection selection(HUGE_DESKTOP);
  ASSERT_EQ(selection.selectedIndex(), LARGEST);

  const bool selected = selection.select(SMALLEST);

  EXPECT_TRUE(selected);
  EXPECT_EQ(selection.selectedIndex(), SMALLEST);
}

/**
 * Given a desktop too small for the largest size
 * When the largest size is selected
 * Then the selection is refused and does not change
 */
TEST(WindowSizeSelection, SelectingAnUnavailableSizeChangesNothing) {
  WindowSizeSelection selection(LAPTOP_DESKTOP);
  const std::size_t before = selection.selectedIndex();

  const bool selected = selection.select(LARGEST);

  EXPECT_FALSE(selected);
  EXPECT_EQ(selection.selectedIndex(), before);
}

/**
 * Given a desktop larger than every size
 * When an index past the end of the list is selected
 * Then the selection is refused and does not change
 */
TEST(WindowSizeSelection, IndexOutsideTheListIsRefused) {
  WindowSizeSelection selection(HUGE_DESKTOP);
  const std::size_t before = selection.selectedIndex();

  const bool selected = selection.select(WINDOW_SIZES.size());

  EXPECT_FALSE(selected);
  EXPECT_FALSE(selection.isAvailable(WINDOW_SIZES.size()));
  EXPECT_EQ(selection.selectedIndex(), before);
}

/**
 * Given a desktop of 2000 by 1100 pixels, where 1920 by 1080 is selected
 * When the centered position is asked
 * Then the window leaves half of the free space on each side: 40 and 10 pixels
 */
TEST(WindowSizeSelection, SelectedSizeIsCenteredOnTheDesktop) {
  const WindowSizeSelection selection(ROOMY_DESKTOP);

  const PixelPosition position = selection.centeredPosition();

  EXPECT_EQ(position.x, HALF_FREE_WIDTH_ON_ROOMY);
  EXPECT_EQ(position.y, HALF_FREE_HEIGHT_ON_ROOMY);
}

/**
 * Given a desktop smaller than the selected size in both directions
 * When the centered position is asked
 * Then the window starts at the corner of the desktop
 */
TEST(WindowSizeSelection, WindowLargerThanTheDesktopStartsAtItsCorner) {
  const WindowSizeSelection selection(TINY_DESKTOP);

  const PixelPosition position = selection.centeredPosition();

  EXPECT_EQ(position.x, 0);
  EXPECT_EQ(position.y, 0);
}
