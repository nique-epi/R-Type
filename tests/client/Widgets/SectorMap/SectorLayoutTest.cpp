#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <cstdlib>
#include "MapView.hpp"
#include "MockupPlacement.hpp"
#include "SectorLayout.hpp"
#include "SectorLayoutTestConstants.hpp"
#include "SectorMapConstants.hpp"

using rtype::client::angleOf;
using rtype::client::MapView;
using rtype::client::placeOf;
using rtype::client::reachOf;
using rtype::client::SECTOR_PLANET_RADIUS;
using rtype::client::viewOn;
using rtype::client::ZOOMED_IN_FACTOR;
using rtype::client::testing::BELOW_FLOOR_BEARING;
using rtype::client::testing::CHOSEN_STAR_PIXEL;
using rtype::client::testing::FAR_PING_MILLISECONDS;
using rtype::client::testing::FIRST_SERVER;
using rtype::client::testing::FLOOR_BEARING;
using rtype::client::testing::FLOOR_PIXEL;
using rtype::client::testing::LOW_PING_MILLISECONDS;
using rtype::client::testing::MOCKUP_PLACEMENTS;
using rtype::client::testing::MOCKUP_REACH_TO_THE_RIGHT;
using rtype::client::testing::MOCKUP_ZOOMED_IN_CENTER_X;
using rtype::client::testing::MOCKUP_ZOOMED_IN_CENTER_Y;
using rtype::client::testing::MOCKUP_ZOOMED_OUT;
using rtype::client::testing::MockupPlacement;
using rtype::client::testing::NEBULA_BEARING;
using rtype::client::testing::NEBULA_PIXEL_ZOOMED_IN;
using rtype::client::testing::ORBITAL_BEARING;
using rtype::client::testing::REACH_TOLERANCE;
using rtype::client::testing::SECOND_SERVER;
using rtype::client::testing::SERVERS_TO_SPREAD;
using rtype::client::testing::WIDEST_ANGLE;
using rtype::client::testing::WIDEST_ANGLE_AT_THE_FAR_PING;

/**
 * Given the planet of the map and its ring leaning in "\"
 * When its reach to the right is computed
 * Then it is the reach the kit of the mockup computes
 */
TEST(SectorLayout, ReachesToTheRightAsTheMockup) {
  EXPECT_NEAR(reachOf(0.0, SECTOR_PLANET_RADIUS), MOCKUP_REACH_TO_THE_RIGHT,
              REACH_TOLERANCE);
}

/**
 * Given four servers of the mockup, from 16 to 225 ms, up and down
 * When they are placed on the map zoomed out
 * Then each lands on the pixel the kit of the mockup gives it
 */
TEST(SectorLayout, PlacesTheServersOnThePixelsOfTheMockup) {
  for (const MockupPlacement& placement : MOCKUP_PLACEMENTS) {
    EXPECT_EQ(placeOf(placement.bearing, MOCKUP_ZOOMED_OUT), placement.pixel);
  }
}

/**
 * Given the chosen server of the zoom of the mockup
 * When the view at zoom 2 is set on it
 * Then the planet lands where the mockup puts it
 */
TEST(SectorLayout, SetsTheZoomedViewWhereTheMockupDoes) {
  const MapView view =
      viewOn(ORBITAL_BEARING, ZOOMED_IN_FACTOR, CHOSEN_STAR_PIXEL);

  EXPECT_EQ(view.centerX, MOCKUP_ZOOMED_IN_CENTER_X);
  EXPECT_EQ(view.centerY, MOCKUP_ZOOMED_IN_CENTER_Y);
}

/**
 * Given the view at zoom 2 set on the chosen server
 * When another server is placed in it
 * Then it lands where the mockup puts it
 */
TEST(SectorLayout, PlacesAnotherServerInTheZoomedViewAsTheMockup) {
  const MapView view =
      viewOn(ORBITAL_BEARING, ZOOMED_IN_FACTOR, CHOSEN_STAR_PIXEL);

  EXPECT_EQ(placeOf(NEBULA_BEARING, view), NEBULA_PIXEL_ZOOMED_IN);
}

/**
 * Given a server of 9 ms, the floor of the spread
 * When it is placed
 * Then it stands just past the reach of the planet, as in the mockup
 */
TEST(SectorLayout, PlacesAServerAtTheFloorJustPastThePlanet) {
  EXPECT_EQ(placeOf(FLOOR_BEARING, MOCKUP_ZOOMED_OUT), FLOOR_PIXEL);
}

/**
 * Given a server of 4 ms, below the floor of the spread
 * When it is placed
 * Then it stands where a server at the floor stands, never on the ring
 */
TEST(SectorLayout, NeverPlacesAServerInsideTheReachOfThePlanet) {
  EXPECT_EQ(placeOf(BELOW_FLOOR_BEARING, MOCKUP_ZOOMED_OUT), FLOOR_PIXEL);
}

/**
 * Given a server and its ping
 * When its direction is taken twice
 * Then both are the same
 */
TEST(SectorLayout, GivesAServerTheSameDirectionEveryTime) {
  EXPECT_EQ(angleOf(FIRST_SERVER, LOW_PING_MILLISECONDS),
            angleOf(FIRST_SERVER, LOW_PING_MILLISECONDS));
}

/**
 * Given two servers of the same ping
 * When their directions are taken
 * Then they differ, so the two stars do not overlap
 */
TEST(SectorLayout, GivesTwoServersOfOnePingTwoDirections) {
  EXPECT_NE(angleOf(FIRST_SERVER, LOW_PING_MILLISECONDS),
            angleOf(SECOND_SERVER, LOW_PING_MILLISECONDS));
}

/**
 * Given fifty servers of a low ping
 * When their directions are taken
 * Then none is wider than 75 degrees
 */
TEST(SectorLayout, KeepsEveryDirectionWithinTheWidestAngle) {
  for (std::int64_t id = 1; id <= SERVERS_TO_SPREAD; ++id) {
    EXPECT_LE(std::abs(angleOf(id, LOW_PING_MILLISECONDS)), WIDEST_ANGLE);
  }
}

/**
 * Given fifty servers of 225 ms, far from the planet
 * When their directions are taken
 * Then none is wider than the 11 degrees the map shows at that distance
 */
TEST(SectorLayout, NarrowsTheDirectionsOfFarServers) {
  for (std::int64_t id = 1; id <= SERVERS_TO_SPREAD; ++id) {
    EXPECT_LE(std::abs(angleOf(id, FAR_PING_MILLISECONDS)),
              WIDEST_ANGLE_AT_THE_FAR_PING);
  }
}
