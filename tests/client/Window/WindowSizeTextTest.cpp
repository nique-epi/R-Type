#include <gtest/gtest.h>
#include "PixelSize.hpp"
#include "WindowSizeText.hpp"

using rtype::client::PixelSize;
using rtype::client::windowSizeText;

/**
 * Given the size 1920 x 1080
 * When it is written
 * Then it reads as on the picture the team chose
 */
TEST(WindowSizeTextTest, ASizeReadsWidthTimesHeight) {
  EXPECT_EQ(windowSizeText(PixelSize{.width = 1920, .height = 1080}),
            "1920 X 1080");
}
