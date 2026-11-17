#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include "BodyMasks.hpp"
#include "CollisionScene.hpp"
#include "Entity.hpp"
#include "ManeuverKind.hpp"
#include "PixelMask.hpp"
#include "PlacedMask.hpp"
#include "RolledMasksFixtures.hpp"
#include "RolledMasksTestConstants.hpp"
#include "ShipMotion.hpp"
#include "ShipRollMasks.hpp"

using rtype::engine::Entity;
using rtype::engine::PixelMask;
using rtype::engine::PlacedMask;
using rtype::game::bodyMaskAt;
using rtype::game::forgetMaskOf;
using rtype::game::ManeuverKind;
using rtype::game::ShipMotion;
using rtype::game::ShipRollMasks;
using rtype::game::testing::DIAGONAL_SIDE;
using rtype::game::testing::diagonalMask;
using rtype::game::testing::EDGE_ON_INTO_THE_ROLL;
using rtype::game::testing::FIRST_GRID_ROW_EDGE_ON;
using rtype::game::testing::ROWS_SEEN_EDGE_ON;
using rtype::game::testing::SHIP_HALF_A_PIXEL_DOWN;
using rtype::game::testing::shipRollingOn;

/**
 * Given a ship with a mask of 16 rows and the masks of its roll, a quarter
 * turn into its roll, half a pixel below a row of the grid
 * When the mask it shows is laid
 * Then it is the mask of 2 rows the client draws, on the row of the grid the
 * client draws its first row on
 */
TEST(BodyMasks, ARollingShipShowsTheMaskOfItsRollWhereTheClientDrawsIt) {
  CollisionScene scene;
  const auto upright =
      std::make_shared<const PixelMask>(diagonalMask(DIAGONAL_SIDE));
  const Entity ship = shipRollingOn(scene, upright);
  scene.components.add(ship, ShipMotion{.maneuver = ManeuverKind::Roll,
                                        .maneuverTime = EDGE_ON_INTO_THE_ROLL});

  const std::optional<PlacedMask> placed =
      bodyMaskAt(scene.components, ship, SHIP_HALF_A_PIXEL_DOWN);

  ASSERT_TRUE(placed.has_value());
  const PlacedMask found = placed.value_or(PlacedMask{});
  ASSERT_NE(found.mask, nullptr);
  EXPECT_EQ(found.mask->height(), ROWS_SEEN_EDGE_ON);
  EXPECT_EQ(found.top, FIRST_GRID_ROW_EDGE_ON);
}

/**
 * Given a ship with a mask of 16 rows and the masks of its roll, not rolling
 * When the mask it shows is laid
 * Then it is its upright mask
 */
TEST(BodyMasks, AShipThatDoesNotRollShowsItsUprightMask) {
  CollisionScene scene;
  const auto upright =
      std::make_shared<const PixelMask>(diagonalMask(DIAGONAL_SIDE));
  const Entity ship = shipRollingOn(scene, upright);
  scene.components.add(ship, ShipMotion{});

  const std::optional<PlacedMask> placed =
      bodyMaskAt(scene.components, ship, SHIP_HALF_A_PIXEL_DOWN);

  ASSERT_TRUE(placed.has_value());
  EXPECT_EQ(placed.value_or(PlacedMask{}).mask, upright.get());
}

/**
 * Given a ship with a mask and the masks of its roll
 * When its mask is forgotten, as when a script resizes its box
 * Then the masks of its roll are emptied too
 */
TEST(BodyMasks, ForgettingTheMaskForgetsTheMasksOfTheRoll) {
  CollisionScene scene;
  const auto upright =
      std::make_shared<const PixelMask>(diagonalMask(DIAGONAL_SIDE));
  const Entity ship = shipRollingOn(scene, upright);

  forgetMaskOf(scene.components, ship);

  const auto* const rolled = scene.components.get<ShipRollMasks>(ship);
  ASSERT_NE(rolled, nullptr);
  EXPECT_EQ(rolled->masks, nullptr);
}
