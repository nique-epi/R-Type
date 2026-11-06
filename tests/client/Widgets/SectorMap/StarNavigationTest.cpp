#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <vector>
#include "MapDirection.hpp"
#include "SectorMapPiecesTestConstants.hpp"
#include "StarNavigation.hpp"

using rtype::client::MapDirection;
using rtype::client::neighbourOf;
using rtype::client::starUnder;
using rtype::client::testing::AT_THE_EDGE_OF_THE_PICK;
using rtype::client::testing::FAR_ON_THE_RIGHT;
using rtype::client::testing::JUST_PAST_THE_PICK;
using rtype::client::testing::MIDDLE_STAR;
using rtype::client::testing::NEAR_ON_THE_RIGHT;
using rtype::client::testing::NO_SUCH_STAR;
using rtype::client::testing::STEEPLY_UP_RIGHT;

/**
 * Given a star with two others on its right, one nearer
 * When the right arrow moves from it
 * Then it reaches the nearer one
 */
TEST(StarNavigation, TheRightArrowReachesTheNearestStarOnTheRight) {
  const std::vector<sf::Vector2i> stars{MIDDLE_STAR, FAR_ON_THE_RIGHT,
                                        NEAR_ON_THE_RIGHT};

  EXPECT_EQ(neighbourOf(stars, 0, MapDirection::Right),
            std::optional<std::size_t>{2});
}

/**
 * Given a star whose only neighbour lies up and right, 60 degrees off the axis
 * When the right arrow moves from it
 * Then it reaches nothing: the neighbour lies more up than right
 */
TEST(StarNavigation, AStarMoreThanFortyFiveDegreesAsideIsNotReached) {
  const std::vector<sf::Vector2i> stars{MIDDLE_STAR, STEEPLY_UP_RIGHT};

  EXPECT_EQ(neighbourOf(stars, 0, MapDirection::Right), std::nullopt);
}

/**
 * Given the same two stars
 * When the up arrow moves from the first
 * Then it reaches the second
 */
TEST(StarNavigation, TheUpArrowReachesAStarAbove) {
  const std::vector<sf::Vector2i> stars{MIDDLE_STAR, STEEPLY_UP_RIGHT};

  EXPECT_EQ(neighbourOf(stars, 0, MapDirection::Up),
            std::optional<std::size_t>{1});
}

/**
 * Given a single star
 * When any arrow moves from it
 * Then it reaches nothing
 */
TEST(StarNavigation, ALoneStarHasNoNeighbour) {
  const std::vector<sf::Vector2i> stars{MIDDLE_STAR};

  EXPECT_EQ(neighbourOf(stars, 0, MapDirection::Left), std::nullopt);
}

/**
 * Given a list of no star
 * When an arrow moves from a star it does not hold
 * Then it reaches nothing, and reads nothing out of the list
 */
TEST(StarNavigation, FromAStarTheListDoesNotHoldReachesNothing) {
  const std::vector<sf::Vector2i> stars;

  EXPECT_EQ(neighbourOf(stars, NO_SUCH_STAR, MapDirection::Down), std::nullopt);
}

/**
 * Given a star and the pointer 6 pixels right of it and 6 below
 * When the star under the pointer is looked for
 * Then it is that star: 6 pixels is still close enough to pick it
 */
TEST(StarNavigation, APointerAtTheEdgeOfThePickFindsTheStar) {
  const std::vector<sf::Vector2i> stars{FAR_ON_THE_RIGHT, MIDDLE_STAR};

  EXPECT_EQ(starUnder(stars, AT_THE_EDGE_OF_THE_PICK),
            std::optional<std::size_t>{1});
}

/**
 * Given a star and the pointer 7 pixels right of it
 * When the star under the pointer is looked for
 * Then there is none: the pointer is past the pick
 */
TEST(StarNavigation, APointerPastThePickFindsNoStar) {
  const std::vector<sf::Vector2i> stars{MIDDLE_STAR};

  EXPECT_EQ(starUnder(stars, JUST_PAST_THE_PICK), std::nullopt);
}
