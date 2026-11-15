#include <gtest/gtest.h>
#include "ContactShape.hpp"
#include "MaskArea.hpp"
#include "PixelMask.hpp"
#include "PlacedMask.hpp"
#include "Position.hpp"
#include "ShapeContacts.hpp"
#include "ShapeContactsFixtures.hpp"
#include "ShapeContactsTestConstants.hpp"

using rtype::engine::MaskArea;
using rtype::engine::PixelMask;
using rtype::engine::PlacedMask;
using rtype::game::areaOf;
using rtype::game::ContactShape;
using rtype::game::placedMaskOf;
using rtype::game::Position;
using rtype::game::shapesTouch;
using rtype::game::testing::BODY_ONE_CELL_RIGHT;
using rtype::game::testing::BODY_OVER_TWO_CELLS;
using rtype::game::testing::BOX_A_TENTH_LEFT;
using rtype::game::testing::BOX_IN_THE_CLEAR_CELL;
using rtype::game::testing::BOX_IN_THE_DRAWN_CELL;
using rtype::game::testing::BOX_OF_NO_WIDTH;
using rtype::game::testing::BOX_ON_CELL_EDGES;
using rtype::game::testing::CELL_HALF_A_CELL_BELOW;
using rtype::game::testing::CELL_HALF_A_CELL_LEFT;
using rtype::game::testing::CELL_HALF_A_CELL_RIGHT;
using rtype::game::testing::CELL_OF_THE_BOX_ON_EDGES;
using rtype::game::testing::CELL_UNDER_A_CENTER_ON_AN_EDGE;
using rtype::game::testing::CELLS_OF_THE_BOX_A_TENTH_LEFT;
using rtype::game::testing::CENTER_HALF_A_CELL_BELOW_Y;
using rtype::game::testing::CENTER_HALF_A_CELL_LEFT_X;
using rtype::game::testing::CENTER_HALF_A_CELL_RIGHT_X;
using rtype::game::testing::CENTER_ON_A_CELL_EDGE_X;
using rtype::game::testing::CENTER_ON_A_CELL_EDGE_Y;
using rtype::game::testing::FAR_AWAY_BOX;
using rtype::game::testing::FRAME_HEIGHT;
using rtype::game::testing::FRAME_WIDTH;
using rtype::game::testing::leftPixelDrawn;
using rtype::game::testing::NO_CELL;
using rtype::game::testing::ONE_CELL_RIGHT;

/**
 * Given a mask as large as the ship and a center on the edge of a cell
 * When the mask is laid there
 * Then its top-left pixel lies on cell 0, 0, as the client draws it
 */
TEST(ShapeContacts, AMaskCenteredOnACellEdgeStartsOnThatEdge) {
  const PixelMask frame{FRAME_WIDTH, FRAME_HEIGHT};

  const PlacedMask placed = placedMaskOf(
      Position{.x = CENTER_ON_A_CELL_EDGE_X, .y = CENTER_ON_A_CELL_EDGE_Y},
      frame);

  EXPECT_EQ(placed.left, CELL_UNDER_A_CENTER_ON_AN_EDGE);
  EXPECT_EQ(placed.top, CELL_UNDER_A_CENTER_ON_AN_EDGE);
}

/**
 * Given a mask centered half a cell right of a cell edge
 * When the mask is laid there
 * Then it starts one cell right: the half is rounded away from zero
 */
TEST(ShapeContacts, AHalfCellRightRoundsToTheNextCell) {
  const PixelMask frame{FRAME_WIDTH, FRAME_HEIGHT};

  const PlacedMask placed = placedMaskOf(
      Position{.x = CENTER_HALF_A_CELL_RIGHT_X, .y = CENTER_ON_A_CELL_EDGE_Y},
      frame);

  EXPECT_EQ(placed.left, CELL_HALF_A_CELL_RIGHT);
}

/**
 * Given a mask centered half a cell left of the edge of cell 0
 * When the mask is laid there
 * Then it starts on cell -1: the half is rounded away from zero
 */
TEST(ShapeContacts, AHalfCellLeftOfZeroRoundsToTheCellBefore) {
  const PixelMask frame{FRAME_WIDTH, FRAME_HEIGHT};

  const PlacedMask placed = placedMaskOf(
      Position{.x = CENTER_HALF_A_CELL_LEFT_X, .y = CENTER_ON_A_CELL_EDGE_Y},
      frame);

  EXPECT_EQ(placed.left, CELL_HALF_A_CELL_LEFT);
}

/**
 * Given a mask centered half a cell below a cell edge
 * When the mask is laid there
 * Then it starts one row lower
 */
TEST(ShapeContacts, AHalfCellBelowRoundsToTheNextRow) {
  const PixelMask frame{FRAME_WIDTH, FRAME_HEIGHT};

  const PlacedMask placed = placedMaskOf(
      Position{.x = CENTER_ON_A_CELL_EDGE_X, .y = CENTER_HALF_A_CELL_BELOW_Y},
      frame);

  EXPECT_EQ(placed.top, CELL_HALF_A_CELL_BELOW);
}

/**
 * Given a box whose edges fall on cell edges
 * When its cells are taken
 * Then it covers the cells inside its edges, none past them
 */
TEST(ShapeContacts, ABoxOnCellEdgesCoversTheCellsInside) {
  const MaskArea area = areaOf(BOX_ON_CELL_EDGES);

  EXPECT_EQ(area.left, CELL_OF_THE_BOX_ON_EDGES.left);
  EXPECT_EQ(area.top, CELL_OF_THE_BOX_ON_EDGES.top);
  EXPECT_EQ(area.width, CELL_OF_THE_BOX_ON_EDGES.width);
  EXPECT_EQ(area.height, CELL_OF_THE_BOX_ON_EDGES.height);
}

/**
 * Given a box a tenth of a unit left of cell edges
 * When its cells are taken
 * Then it covers the cell its left edge reaches into too
 */
TEST(ShapeContacts, ABoxPastACellEdgeCoversThatCell) {
  const MaskArea area = areaOf(BOX_A_TENTH_LEFT);

  EXPECT_EQ(area.left, CELLS_OF_THE_BOX_A_TENTH_LEFT.left);
  EXPECT_EQ(area.width, CELLS_OF_THE_BOX_A_TENTH_LEFT.width);
}

/**
 * Given a box of no width standing on a cell edge
 * When its cells are taken
 * Then it covers no column
 */
TEST(ShapeContacts, ABoxOfNoWidthCoversNoColumn) {
  EXPECT_EQ(areaOf(BOX_OF_NO_WIDTH).width, NO_CELL);
}

/**
 * Given two shapes without a mask whose boxes overlap
 * When they are tested
 * Then they touch, as two boxes did before masks
 */
TEST(ShapeContacts, TwoBoxesThatOverlapTouch) {
  EXPECT_TRUE(shapesTouch(ContactShape{.bounds = BODY_OVER_TWO_CELLS},
                          ContactShape{.bounds = BOX_IN_THE_CLEAR_CELL}));
}

/**
 * Given a shape with a mask and a box far away
 * When they are tested
 * Then they do not touch: the boxes are compared first
 */
TEST(ShapeContacts, ShapesWhoseBoxesDoNotOverlapNeverTouch) {
  const PixelMask mask = leftPixelDrawn();

  EXPECT_FALSE(shapesTouch(
      ContactShape{.bounds = BODY_OVER_TWO_CELLS,
                   .mask = PlacedMask{.mask = &mask, .left = 0, .top = 0}},
      ContactShape{.bounds = FAR_AWAY_BOX}));
}

/**
 * Given a shape with a mask and a box over its drawn pixel
 * When they are tested
 * Then they touch
 */
TEST(ShapeContacts, ABoxOverADrawnPixelTouchesTheMask) {
  const PixelMask mask = leftPixelDrawn();

  EXPECT_TRUE(shapesTouch(
      ContactShape{.bounds = BODY_OVER_TWO_CELLS,
                   .mask = PlacedMask{.mask = &mask, .left = 0, .top = 0}},
      ContactShape{.bounds = BOX_IN_THE_DRAWN_CELL}));
}

/**
 * Given a shape with a mask and a box over its clear pixel only, inside its
 * own box
 * When they are tested
 * Then they do not touch
 */
TEST(ShapeContacts, ABoxOverAClearPixelMissesTheMask) {
  const PixelMask mask = leftPixelDrawn();

  EXPECT_FALSE(shapesTouch(
      ContactShape{.bounds = BODY_OVER_TWO_CELLS,
                   .mask = PlacedMask{.mask = &mask, .left = 0, .top = 0}},
      ContactShape{.bounds = BOX_IN_THE_CLEAR_CELL}));
}

/**
 * Given a box over a clear pixel, given first, and a shape with a mask
 * When they are tested
 * Then they do not touch: the order of the shapes does not matter
 */
TEST(ShapeContacts, TheMaskIsReadWhicheverShapeComesFirst) {
  const PixelMask mask = leftPixelDrawn();

  EXPECT_FALSE(shapesTouch(
      ContactShape{.bounds = BOX_IN_THE_CLEAR_CELL},
      ContactShape{.bounds = BODY_OVER_TWO_CELLS,
                   .mask = PlacedMask{.mask = &mask, .left = 0, .top = 0}}));
}

/**
 * Given two masks whose drawn pixels lie on different cells, the second laid
 * one cell right, their boxes overlapping
 * When they are tested
 * Then they do not touch
 */
TEST(ShapeContacts, TwoMasksWhoseDrawnPixelsDifferDoNotTouch) {
  const PixelMask mask = leftPixelDrawn();

  EXPECT_FALSE(shapesTouch(
      ContactShape{.bounds = BODY_OVER_TWO_CELLS,
                   .mask = PlacedMask{.mask = &mask, .left = 0, .top = 0}},
      ContactShape{.bounds = BODY_ONE_CELL_RIGHT,
                   .mask = PlacedMask{
                       .mask = &mask, .left = ONE_CELL_RIGHT, .top = 0}}));
}

/**
 * Given two masks laid on the same cells
 * When they are tested
 * Then they touch
 */
TEST(ShapeContacts, TwoMasksSharingADrawnCellTouch) {
  const PixelMask mask = leftPixelDrawn();

  EXPECT_TRUE(shapesTouch(
      ContactShape{.bounds = BODY_OVER_TWO_CELLS,
                   .mask = PlacedMask{.mask = &mask, .left = 0, .top = 0}},
      ContactShape{.bounds = BODY_OVER_TWO_CELLS,
                   .mask = PlacedMask{.mask = &mask, .left = 0, .top = 0}}));
}
