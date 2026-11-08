#include <gtest/gtest.h>
#include "LabelSide.hpp"
#include "MapView.hpp"
#include "MotionPreference.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "SectorMapConstants.hpp"
#include "SectorMapPainting.hpp"
#include "SectorMapPiecesTestConstants.hpp"
#include "StarLook.hpp"
#include "StarMark.hpp"
#include "StarName.hpp"
#include "TimeConstants.hpp"

using rtype::client::LabelSide;
using rtype::client::MapView;
using rtype::client::MotionPreference;
using rtype::client::paintPingRings;
using rtype::client::paintStar;
using rtype::client::paintStarName;
using rtype::client::planetTurnAfter;
using rtype::client::SECTOR_MAP_AREA;
using rtype::client::SECTOR_PLANET_CENTER;
using rtype::client::StarLook;
using rtype::client::StarMark;
using rtype::client::StarName;
using rtype::client::testing::A_WHOLE_TURN;
using rtype::client::testing::CROWD_OF_PILOTS;
using rtype::client::testing::FEW_PILOTS;
using rtype::client::testing::FOURTH_TURN;
using rtype::client::testing::JUST_BEFORE_A_WHOLE_TURN;
using rtype::client::testing::LARGE_PLANET_SOURCE;
using rtype::client::testing::LAST_TURN;
using rtype::client::testing::MAP_SHEET;
using rtype::client::testing::MEDIUM_PLANET_AT_THE_MIDDLE;
using rtype::client::testing::MEDIUM_PLANET_FOURTH_SOURCE;
using rtype::client::testing::MEDIUM_PLANET_SOURCE;
using rtype::client::testing::MIDDLE_STAR;
using rtype::client::testing::NO_PILOT;
using rtype::client::testing::ONE_TURN_FRAME;
using rtype::client::testing::OTHER_VERSION_PLANET_SOURCE;
using rtype::client::testing::SLOW_PLANET_SOURCE;
using rtype::client::testing::SMALL_PLANET_SOURCE;
using rtype::client::testing::STAR_NAME;
using rtype::client::testing::STAR_NEAR_THE_TOP;
using rtype::engine::Duration;

/**
 * Given an empty server, one of a few pilots and one of a crowd
 * When their stars are painted
 * Then each is a planet from the sheet of the map, and it grows: 5, 7, then 9
 * pixels
 */
TEST(SectorMapPainting, AStarGrowsWithItsPilots) {
  InterfaceCanvasRecord empty;
  InterfaceCanvasRecord few;
  InterfaceCanvasRecord crowd;
  RecordingInterfaceCanvas emptyCanvas{empty};
  RecordingInterfaceCanvas fewCanvas{few};
  RecordingInterfaceCanvas crowdCanvas{crowd};

  paintStar(emptyCanvas, StarMark{.point = MIDDLE_STAR,
                                  .look = StarLook{.playerCount = NO_PILOT}});
  paintStar(fewCanvas, StarMark{.point = MIDDLE_STAR,
                                .look = StarLook{.playerCount = FEW_PILOTS}});
  paintStar(crowdCanvas,
            StarMark{.point = MIDDLE_STAR,
                     .look = StarLook{.playerCount = CROWD_OF_PILOTS}});

  ASSERT_EQ(empty.images.size(), 1U);
  ASSERT_EQ(few.images.size(), 1U);
  ASSERT_EQ(crowd.images.size(), 1U);
  EXPECT_EQ(empty.images.front().assetId, MAP_SHEET);
  EXPECT_EQ(empty.images.front().source, SMALL_PLANET_SOURCE);
  EXPECT_EQ(few.images.front().source, MEDIUM_PLANET_SOURCE);
  EXPECT_EQ(crowd.images.front().source, LARGE_PLANET_SOURCE);
}

/**
 * Given a server of a few pilots at (200, 130)
 * When its star is painted
 * Then its planet of 7 pixels is centered on it, from (197, 127)
 */
TEST(SectorMapPainting, APlanetIsCenteredOnItsStar) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas{record};

  paintStar(canvas, StarMark{.point = MIDDLE_STAR,
                             .look = StarLook{.playerCount = FEW_PILOTS}});

  ASSERT_EQ(record.images.size(), 1U);
  EXPECT_EQ(record.images.front().topLeft, MEDIUM_PLANET_AT_THE_MIDDLE);
}

/**
 * Given a slow server of a few pilots, and a server of another version
 * When their stars are painted
 * Then the first is the planet in an orange ring, the second the dark one
 */
TEST(SectorMapPainting, ASlowServerAndAnotherVersionHaveTheirOwnPlanet) {
  InterfaceCanvasRecord slow;
  InterfaceCanvasRecord other;
  RecordingInterfaceCanvas slowCanvas{slow};
  RecordingInterfaceCanvas otherCanvas{other};

  paintStar(slowCanvas, StarMark{.point = MIDDLE_STAR,
                                 .look = StarLook{.playerCount = FEW_PILOTS,
                                                  .slow = true}});
  paintStar(otherCanvas, StarMark{.point = MIDDLE_STAR,
                                  .look = StarLook{.playerCount = FEW_PILOTS,
                                                   .incompatible = true}});

  ASSERT_EQ(slow.images.size(), 1U);
  ASSERT_EQ(other.images.size(), 1U);
  EXPECT_EQ(slow.images.front().source, SLOW_PLANET_SOURCE);
  EXPECT_EQ(other.images.front().source, OTHER_VERSION_PLANET_SOURCE);
}

/**
 * Given a server of a few pilots whose planet turned 3 frames
 * When its star is painted
 * Then it shows the fourth frame of its row, at left 27
 */
TEST(SectorMapPainting, APlanetShowsTheFrameOfItsTurn) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas{record};

  paintStar(canvas, StarMark{.point = MIDDLE_STAR,
                             .look = StarLook{.playerCount = FEW_PILOTS},
                             .turn = FOURTH_TURN});

  ASSERT_EQ(record.images.size(), 1U);
  EXPECT_EQ(record.images.front().source, MEDIUM_PLANET_FOURTH_SOURCE);
}

/**
 * Given a server of another version whose planet turned 3 frames
 * When its star is painted
 * Then it still shows its one frame: it stands still
 */
TEST(SectorMapPainting, ThePlanetOfAnotherVersionStandsStill) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas{record};

  paintStar(canvas, StarMark{.point = MIDDLE_STAR,
                             .look = StarLook{.incompatible = true},
                             .turn = FOURTH_TURN});

  ASSERT_EQ(record.images.size(), 1U);
  EXPECT_EQ(record.images.front().source, OTHER_VERSION_PLANET_SOURCE);
}

/**
 * Given a star 2 pixels inside the top of the map, within its margin
 * When it is painted
 * Then nothing is painted
 */
TEST(SectorMapPainting, AStarAtTheEdgeOfTheMapIsNotPainted) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas{record};

  paintStar(canvas, StarMark{.point = STAR_NEAR_THE_TOP,
                             .look = StarLook{.playerCount = FEW_PILOTS}});

  EXPECT_TRUE(record.images.empty());
  EXPECT_TRUE(litPixelsOf(record).empty());
}

/**
 * Given a player who did not ask for less motion
 * When no time, 150 ms, 1 ns less than 1.2 s, then 1.2 s have passed
 * Then the planets show the first frame, the second, the last, then the first
 * again: a whole turn is back where it started
 */
TEST(SectorMapPainting, APlanetTurnsAFrameEvery150Milliseconds) {
  EXPECT_EQ(planetTurnAfter(Duration::zero(), MotionPreference::Full), 0U);
  EXPECT_EQ(planetTurnAfter(ONE_TURN_FRAME, MotionPreference::Full), 1U);
  EXPECT_EQ(planetTurnAfter(JUST_BEFORE_A_WHOLE_TURN, MotionPreference::Full),
            LAST_TURN);
  EXPECT_EQ(planetTurnAfter(A_WHOLE_TURN, MotionPreference::Full), 0U);
}

/**
 * Given a player who asked for less motion
 * When 150 ms have passed
 * Then the planets still show their first frame
 */
TEST(SectorMapPainting, APlanetStandsStillWithLessMotion) {
  EXPECT_EQ(planetTurnAfter(ONE_TURN_FRAME, MotionPreference::Reduced), 0U);
}

/**
 * Given the name of a star with no free side
 * When it is painted
 * Then nothing is written
 */
TEST(SectorMapPainting, ANameWithNoSideIsNotWritten) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas{record};

  paintStarName(canvas, StarName{.point = MIDDLE_STAR,
                                 .name = STAR_NAME,
                                 .side = LabelSide::None});

  EXPECT_TRUE(record.writings.empty());
}

/**
 * Given the map zoomed out
 * When its rings are painted
 * Then every dash lies inside the map, whose edge clips them
 */
TEST(SectorMapPainting, PaintsTheRingsInsideTheMapOnly) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas{record};

  paintPingRings(canvas, MapView{.zoom = 1.0,
                                 .centerX = SECTOR_PLANET_CENTER.x,
                                 .centerY = SECTOR_PLANET_CENTER.y});

  ASSERT_FALSE(litPixelsOf(record).empty());
  for (const auto& [column, row] : litPixelsOf(record)) {
    EXPECT_TRUE(SECTOR_MAP_AREA.contains({column, row}));
  }
}
