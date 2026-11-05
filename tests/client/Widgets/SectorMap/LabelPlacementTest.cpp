#include <gtest/gtest.h>
#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include "LabelCandidate.hpp"
#include "LabelPlacement.hpp"
#include "LabelSide.hpp"
#include "SectorMapPiecesTestConstants.hpp"

using rtype::client::LabelCandidate;
using rtype::client::LabelSide;
using rtype::client::placeLabels;
using rtype::client::testing::MIDDLE_STAR;
using rtype::client::testing::NAME_WIDTH;
using rtype::client::testing::OVER_THE_LEFT;
using rtype::client::testing::OVER_THE_RIGHT;
using rtype::client::testing::STAR_AT_THE_RIGHT_EDGE;
using rtype::client::testing::STAR_JUST_BELOW;

/**
 * Given a star alone in the middle of the map
 * When its name is placed
 * Then it takes its right
 */
TEST(LabelPlacement, AStarAloneWritesItsNameOnItsRight) {
  const std::vector<LabelCandidate> stars{
      {.point = MIDDLE_STAR, .nameWidth = NAME_WIDTH}};

  EXPECT_EQ(placeLabels(stars, {}), std::vector<LabelSide>{LabelSide::Right});
}

/**
 * Given a star whose name would run past the right edge of the map
 * When its name is placed
 * Then it takes its left
 */
TEST(LabelPlacement, AStarAtTheRightEdgeWritesItsNameOnItsLeft) {
  const std::vector<LabelCandidate> stars{
      {.point = STAR_AT_THE_RIGHT_EDGE, .nameWidth = NAME_WIDTH}};

  EXPECT_EQ(placeLabels(stars, {}), std::vector<LabelSide>{LabelSide::Left});
}

/**
 * Given a star 3 pixels below a star whose name took its right
 * When its name is placed
 * Then it takes its left, so the two names do not overlap
 */
TEST(LabelPlacement, ASecondNameMovesToTheLeftOfTheFirst) {
  const std::vector<LabelCandidate> stars{
      {.point = MIDDLE_STAR, .nameWidth = NAME_WIDTH},
      {.point = STAR_JUST_BELOW, .nameWidth = NAME_WIDTH}};

  EXPECT_EQ(placeLabels(stars, {}),
            (std::vector<LabelSide>{LabelSide::Right, LabelSide::Left}));
}

/**
 * Given a star whose both sides are taken by the bubble and the loupe
 * When its name is placed
 * Then it writes no name
 */
TEST(LabelPlacement, AStarWithNoFreeSideWritesNoName) {
  const std::vector<LabelCandidate> stars{
      {.point = MIDDLE_STAR, .nameWidth = NAME_WIDTH}};
  const std::vector<sf::IntRect> forbidden{OVER_THE_RIGHT, OVER_THE_LEFT};

  EXPECT_EQ(placeLabels(stars, forbidden),
            std::vector<LabelSide>{LabelSide::None});
}
