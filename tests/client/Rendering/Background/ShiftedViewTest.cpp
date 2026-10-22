#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include "ShiftedView.hpp"

using rtype::client::shiftedBy;

namespace {

constexpr sf::FloatRect CANVAS_AREA({0.0F, 0.0F}, {480.0F, 270.0F});
constexpr sf::Vector2i SHAKE(2, -1);
constexpr sf::Vector2f A_PIXEL(100.0F, 50.0F);

}  // namespace

/**
 * Given the view of a canvas, and a shake of 2 pixels right and 1 up
 * When a pixel is drawn through the shifted view
 * Then it lands where the pixel 2 to its right and 1 above lands through the
 * view
 */
TEST(ShiftedView, MovesWhatIsDrawnByTheShake) {
  const sf::View canvasView(CANVAS_AREA);
  const sf::View shifted = shiftedBy(canvasView, SHAKE);

  const sf::Vector2f landed = shifted.getTransform().transformPoint(A_PIXEL);
  const sf::Vector2f expected =
      canvasView.getTransform().transformPoint(A_PIXEL + sf::Vector2f(SHAKE));

  EXPECT_FLOAT_EQ(landed.x, expected.x);
  EXPECT_FLOAT_EQ(landed.y, expected.y);
}
