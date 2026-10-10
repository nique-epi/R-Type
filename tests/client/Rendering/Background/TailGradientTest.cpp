#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include "TailGradient.hpp"

using rtype::client::GradientStop;
using rtype::client::tailColor;
using rtype::client::TailGradient;

namespace {

constexpr sf::Color HEAD_COLOR(250, 250, 250);
constexpr sf::Color SECOND_COLOR(150, 150, 250);
constexpr sf::Color THIRD_COLOR(80, 80, 200);
constexpr sf::Color END_COLOR(20, 20, 90);
constexpr TailGradient GRADIENT{
    GradientStop{.end = 0.2F, .color = HEAD_COLOR},
    GradientStop{.end = 0.5F, .color = SECOND_COLOR},
    GradientStop{.end = 0.8F, .color = THIRD_COLOR},
    GradientStop{.end = 1.0F, .color = END_COLOR},
};
constexpr float AT_THE_HEAD = 0.0F;
constexpr float ON_A_STOP_END = 0.5F;
constexpr float PAST_EVERY_STOP = 1.5F;

}  // namespace

/**
 * Given a tail gradient
 * When the color at the head of the tail is asked for
 * Then it is the color of the first stop
 */
TEST(TailGradient, HeadTakesTheFirstColor) {
  EXPECT_EQ(tailColor(GRADIENT, AT_THE_HEAD), HEAD_COLOR);
}

/**
 * Given a tail gradient
 * When the color exactly at the end of a stop is asked for
 * Then it is the color of the next stop: a stop ends before its end value
 */
TEST(TailGradient, StopEndBelongsToTheNextStop) {
  EXPECT_EQ(tailColor(GRADIENT, ON_A_STOP_END), THIRD_COLOR);
}

/**
 * Given a tail gradient
 * When the color past the end of every stop is asked for
 * Then it is the color of the last stop
 */
TEST(TailGradient, PastEveryStopTakesTheLastColor) {
  EXPECT_EQ(tailColor(GRADIENT, PAST_EVERY_STOP), END_COLOR);
}
