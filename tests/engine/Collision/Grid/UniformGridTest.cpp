#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "CandidatePair.hpp"
#include "EngineException.hpp"
#include "UniformGrid.hpp"
#include "UniformGridTestConstants.hpp"

using rtype::engine::CandidatePair;
using rtype::engine::InvalidGridLayoutException;
using rtype::engine::UniformGrid;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::testing::UnorderedElementsAre;

/**
 * Given a grid that holds no box
 * When its pairs are listed
 * Then there is none
 */
TEST(UniformGrid, AnEmptyGridListsNoPair) {
  UniformGrid grid(TEST_AREA, TEST_CELL_SIDE);

  EXPECT_THAT(grid.candidatePairs(), IsEmpty());
}

/**
 * Given a grid that holds one box covering six cells
 * When its pairs are listed
 * Then there is none: a box is never paired with itself
 */
TEST(UniformGrid, ABoxAloneOverSeveralCellsIsPairedWithNothing) {
  UniformGrid grid(TEST_AREA, TEST_CELL_SIDE);
  grid.insert(WIDE_BOX);

  EXPECT_THAT(grid.candidatePairs(), IsEmpty());
}

/**
 * Given two small squares in the same cell
 * When the pairs are listed
 * Then the two squares are the only pair
 */
TEST(UniformGrid, TwoBoxesInOneCellArePaired) {
  UniformGrid grid(TEST_AREA, TEST_CELL_SIDE);
  grid.insert(FIRST_CELL_SQUARE);
  grid.insert(OTHER_FIRST_CELL_SQUARE);

  EXPECT_THAT(
      grid.candidatePairs(),
      ElementsAre(CandidatePair{.first = FIRST_BOX, .second = SECOND_BOX}));
}

/**
 * Given two boxes that share four cells
 * When the pairs are listed
 * Then they are paired once, not once per shared cell
 */
TEST(UniformGrid, TwoBoxesSharingSeveralCellsArePairedOnce) {
  UniformGrid grid(TEST_AREA, TEST_CELL_SIDE);
  grid.insert(WIDE_BOX);
  grid.insert(SECOND_WIDE_BOX);

  EXPECT_THAT(
      grid.candidatePairs(),
      ElementsAre(CandidatePair{.first = FIRST_BOX, .second = SECOND_BOX}));
}

/**
 * Given a small square in the first cell and one in the last cell
 * When the pairs are listed
 * Then there is none
 */
TEST(UniformGrid, BoxesInCellsApartAreNotPaired) {
  UniformGrid grid(TEST_AREA, TEST_CELL_SIDE);
  grid.insert(FIRST_CELL_SQUARE);
  grid.insert(LAST_CELL_SQUARE);

  EXPECT_THAT(grid.candidatePairs(), IsEmpty());
}

/**
 * Given a box over six cells and three small squares in three of them, one
 * square per cell
 * When the pairs are listed
 * Then the box is paired once with each square, and no two squares are paired
 */
TEST(UniformGrid, AWideBoxIsPairedOnceWithEachBoxOfItsCells) {
  UniformGrid grid(TEST_AREA, TEST_CELL_SIDE);
  grid.insert(WIDE_BOX);
  grid.insert(FIRST_CELL_SQUARE);
  grid.insert(SECOND_ROW_SQUARE);
  grid.insert(THIRD_COLUMN_SQUARE);

  EXPECT_THAT(grid.candidatePairs(),
              UnorderedElementsAre(
                  CandidatePair{.first = FIRST_BOX, .second = SECOND_BOX},
                  CandidatePair{.first = FIRST_BOX, .second = THIRD_BOX},
                  CandidatePair{.first = FIRST_BOX, .second = FOURTH_BOX}));
}

/**
 * Given two small squares left of the area, one just beyond its edge and one
 * far beyond, on the same row
 * When the pairs are listed
 * Then they are paired: both are counted in the first column
 */
TEST(UniformGrid, TwoBoxesBeyondTheSameEdgeArePaired) {
  UniformGrid grid(TEST_AREA, TEST_CELL_SIDE);
  grid.insert(BEYOND_LEFT_EDGE_SQUARE);
  grid.insert(FAR_BEYOND_LEFT_EDGE_SQUARE);

  EXPECT_THAT(
      grid.candidatePairs(),
      ElementsAre(CandidatePair{.first = FIRST_BOX, .second = SECOND_BOX}));
}

/**
 * Given a small square in the last cell and one a billion units right of the
 * area, on the same row
 * When the pairs are listed
 * Then they are paired: the far one is counted in the last column
 */
TEST(UniformGrid, ABoxFarBeyondTheRightEdgeSharesTheLastColumn) {
  UniformGrid grid(TEST_AREA, TEST_CELL_SIDE);
  grid.insert(LAST_CELL_SQUARE);
  grid.insert(FAR_BEYOND_RIGHT_EDGE_SQUARE);

  EXPECT_THAT(
      grid.candidatePairs(),
      ElementsAre(CandidatePair{.first = FIRST_BOX, .second = SECOND_BOX}));
}

/**
 * Given a small square in the first cell and a square whose center is not a
 * number
 * When the pairs are listed
 * Then they are paired: a center that is not a number counts as the first cell
 */
TEST(UniformGrid, ABoxWithoutCenterSharesTheFirstCell) {
  UniformGrid grid(TEST_AREA, TEST_CELL_SIDE);
  grid.insert(FIRST_CELL_SQUARE);
  grid.insert(SQUARE_WITHOUT_CENTER);

  EXPECT_THAT(
      grid.candidatePairs(),
      ElementsAre(CandidatePair{.first = FIRST_BOX, .second = SECOND_BOX}));
}

/**
 * Given a grid that held three boxes and listed their pairs, then was cleared
 * When two small squares of one cell are inserted and the pairs are listed
 * Then the only pair is these squares, numbered from 0 again
 */
TEST(UniformGrid, AClearedGridForgetsItsBoxes) {
  UniformGrid grid(TEST_AREA, TEST_CELL_SIDE);
  grid.insert(WIDE_BOX);
  grid.insert(SECOND_WIDE_BOX);
  grid.insert(FIRST_CELL_SQUARE);
  static_cast<void>(grid.candidatePairs());
  grid.clear();
  grid.insert(FIRST_CELL_SQUARE);
  grid.insert(OTHER_FIRST_CELL_SQUARE);

  EXPECT_THAT(
      grid.candidatePairs(),
      ElementsAre(CandidatePair{.first = FIRST_BOX, .second = SECOND_BOX}));
}

/**
 * Given a grid over an area with no size, and two squares far apart
 * When the pairs are listed
 * Then they are paired: such a grid has one cell
 */
TEST(UniformGrid, AGridOverAnAreaWithoutSizeHasOneCell) {
  UniformGrid grid(AREA_WITHOUT_SIZE, TEST_CELL_SIDE);
  grid.insert(FIRST_CELL_SQUARE);
  grid.insert(LAST_CELL_SQUARE);

  EXPECT_THAT(
      grid.candidatePairs(),
      ElementsAre(CandidatePair{.first = FIRST_BOX, .second = SECOND_BOX}));
}

/**
 * Given a negative cell side
 * When a grid is laid with it
 * Then the layout is refused
 */
TEST(UniformGrid, ANegativeCellSideIsRefused) {
  EXPECT_THROW(const UniformGrid grid(TEST_AREA, NEGATIVE_CELL_SIDE),
               InvalidGridLayoutException);
}

/**
 * Given a cell side that would lay 4,000 columns over the area
 * When a grid is laid with it
 * Then the layout is refused
 */
TEST(UniformGrid, AnAreaOfTooManyCellsIsRefused) {
  EXPECT_THROW(const UniformGrid grid(TEST_AREA, TINY_CELL_SIDE),
               InvalidGridLayoutException);
}
