#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include "PixelCircle.hpp"
#include "PixelCircleTestConstants.hpp"

using rtype::client::circleOf;
using rtype::client::testing::BOTTOM_END;
using rtype::client::testing::FIVE_PIXELS;
using rtype::client::testing::HALF_A_PIXEL;
using rtype::client::testing::LEFT_END;
using rtype::client::testing::RIGHT_END;
using rtype::client::testing::TOP_END;
using ::testing::IsSupersetOf;

/**
 * Given a circle of five pixels
 * When its pixels are listed
 * Then it reaches its four extremities
 */
TEST(PixelCircle, ACircleReachesItsFourExtremities) {
  EXPECT_THAT(circleOf(FIVE_PIXELS),
              IsSupersetOf({RIGHT_END, BOTTOM_END, LEFT_END, TOP_END}));
}

/**
 * Given a circle of five pixels
 * When its pixels are listed
 * Then each lies within half a pixel of the true circle
 */
TEST(PixelCircle, EveryPixelLiesOnTheCircle) {
  for (const sf::Vector2i pixel : circleOf(FIVE_PIXELS)) {
    EXPECT_LE(std::abs(sf::Vector2f(pixel).length() -
                       static_cast<float>(FIVE_PIXELS)),
              HALF_A_PIXEL)
        << pixel.x << ", " << pixel.y;
  }
}
