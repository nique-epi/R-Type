#include <gtest/gtest.h>
#include <memory>
#include "PixelMask.hpp"
#include "RolledMaskCatalog.hpp"
#include "RolledMasks.hpp"
#include "RolledMasksFixtures.hpp"
#include "RolledMasksTestConstants.hpp"

using rtype::engine::PixelMask;
using rtype::game::RolledMaskCatalog;
using rtype::game::RolledMasks;
using rtype::game::testing::DIAGONAL_SIDE;
using rtype::game::testing::diagonalMask;
using rtype::game::testing::ONE_ROW;

/**
 * Given a catalog asked once for the masks of an upright mask
 * When it is asked again for the same mask
 * Then it gives the same masks: they are built once, then shared
 */
TEST(RolledMaskCatalog, BuildsTheMasksOfAnUprightMaskOnce) {
  RolledMaskCatalog catalog;
  const PixelMask upright = diagonalMask(DIAGONAL_SIDE);
  const std::shared_ptr<const RolledMasks> first = catalog.rolledOf(upright);

  const std::shared_ptr<const RolledMasks> second = catalog.rolledOf(upright);

  ASSERT_NE(first, nullptr);
  EXPECT_EQ(first, second);
}

/**
 * Given a mask one row tall, shorter than the rows a rolling ship keeps
 * When the catalog is asked for its masks
 * Then it gives none: such a ship keeps its upright mask while it rolls
 */
TEST(RolledMaskCatalog, GivesNoMasksForAMaskShorterThanTheThinnestRoll) {
  RolledMaskCatalog catalog;

  EXPECT_EQ(catalog.rolledOf(PixelMask{DIAGONAL_SIDE, ONE_ROW}), nullptr);
}
