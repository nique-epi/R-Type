#include <gtest/gtest.h>
#include <memory>
#include "IInterfaceCanvas.hpp"
#include "LabelSide.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "SectorMapConstants.hpp"
#include "SectorMapPainting.hpp"
#include "SectorMapPiecesTestConstants.hpp"
#include "StarLook.hpp"
#include "StarMark.hpp"
#include "StarName.hpp"

using rtype::client::LabelSide;
using rtype::client::MapView;
using rtype::client::paintPingRings;
using rtype::client::paintStar;
using rtype::client::paintStarName;
using rtype::client::paintZoomLoupe;
using rtype::client::SECTOR_MAP_AREA;
using rtype::client::SECTOR_PLANET_CENTER;
using rtype::client::StarLook;
using rtype::client::StarMark;
using rtype::client::StarName;
using rtype::client::testing::CROSS_PIXELS;
using rtype::client::testing::CROWD_OF_PILOTS;
using rtype::client::testing::EMPTY_STAR_PIXELS;
using rtype::client::testing::FEW_PILOTS;
using rtype::client::testing::LARGE_CROSS_PIXELS;
using rtype::client::testing::MIDDLE_STAR;
using rtype::client::testing::NO_PILOT;
using rtype::client::testing::STAR_NAME;
using rtype::client::testing::STAR_NEAR_THE_TOP;
using rtype::client::testing::ZOOM_TWICE;

/**
 * Given an empty server, one of a few pilots and one of a crowd
 * When their stars are painted
 * Then the glyph grows: a dot, a cross, a large cross
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

  EXPECT_EQ(litPixelsOf(empty).size(), EMPTY_STAR_PIXELS);
  EXPECT_EQ(litPixelsOf(few).size(), CROSS_PIXELS);
  EXPECT_EQ(litPixelsOf(crowd).size(), LARGE_CROSS_PIXELS);
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

  EXPECT_TRUE(litPixelsOf(record).empty());
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

/**
 * Given the loupe of the zoom
 * When it is painted
 * Then it writes its zoom under it
 */
TEST(SectorMapPainting, TheLoupeWritesItsZoom) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas{record};

  paintZoomLoupe(canvas, MIDDLE_STAR, ZOOM_TWICE);

  EXPECT_NE(writingOf(record, ZOOM_TWICE), nullptr);
}
