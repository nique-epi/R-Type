#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include "LayerDrawingTestConstants.hpp"
#include "StripFrame.hpp"
#include "StripFrameRect.hpp"

using rtype::client::StripFrame;
using rtype::client::stripFrameRect;

/**
 * Given an image of 6 by 3 pixels that holds two frames side by side
 * When the rectangle of its second frame is asked for
 * Then it is its right half, 3 by 3 pixels
 */
TEST(StripFrameRect, AFrameIsItsShareOfTheWidthOfTheImage) {
  const StripFrame twinkling{.file = NEAR_STAR_IMAGE,
                             .frames = FRAMES_OF_THE_NEAR_STAR,
                             .frame = SECOND_FRAME};

  EXPECT_EQ(stripFrameRect(SIZE_OF_THE_NEAR_STAR_IMAGE, twinkling),
            RIGHT_HALF_OF_THE_IMAGE);
}

/**
 * Given a strip that says it holds no frame
 * When the rectangle of a frame is asked for
 * Then it is empty, and nothing is divided by zero
 */
TEST(StripFrameRect, AStripOfNoFrameHasNoRectangle) {
  const StripFrame none{.file = NEAR_STAR_IMAGE, .frames = NO_FRAME};

  EXPECT_EQ(stripFrameRect(SIZE_OF_THE_NEAR_STAR_IMAGE, none), sf::FloatRect{});
}
