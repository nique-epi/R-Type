#include <gtest/gtest.h>
#include <limits>
#include <tuple>
#include "BydoSpawnPosition.hpp"
#include "GameException.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "SpawningConstants.hpp"

namespace {

constexpr float MIDDLE_DRAW = 0.5F;
constexpr float JUST_UNDER_ONE = 0.999999F;
constexpr float ONE = 1.0F;
constexpr float BELOW_ZERO = -0.1F;

/** @brief The lowest row a Bydo may appear on: one margin of 64 units above
 * the status band, which covers the bottom 96 units of the 1080 of the
 * playfield. */
constexpr float LOWEST_ROW_ABOVE_THE_STATUS_BAND = 920.0F;

/** @brief Tolerance for a row computed from a draw just under 1. */
constexpr float ROW_TOLERANCE = 0.01F;

}  // namespace

/**
 * Given any draw from 0 up to just under 1
 * When the spawn position is asked
 * Then the Bydo is past the right edge of the playfield, so off screen
 */
TEST(BydoSpawnPosition, AlwaysAppearsPastTheRightEdge) {
  for (const float draw : {0.0F, MIDDLE_DRAW, JUST_UNDER_ONE}) {
    EXPECT_GT(rtype::game::bydoSpawnPosition(draw).x,
              rtype::game::PLAYFIELD_WIDTH);
  }
}

/**
 * Given the draws 0, 0.5 and just under 1
 * When the spawn position is asked
 * Then the height is the highest row, one margin under the top edge, and the
 * middle of the area of play; the lowest row has its own test
 */
TEST(BydoSpawnPosition, TheDrawPicksTheHeightFromTopToBottom) {
  EXPECT_FLOAT_EQ(rtype::game::bydoSpawnPosition(0.0F).y,
                  rtype::game::BYDO_SPAWN_VERTICAL_MARGIN);
  EXPECT_FLOAT_EQ(rtype::game::bydoSpawnPosition(MIDDLE_DRAW).y,
                  rtype::game::PLAY_AREA_HEIGHT / 2.0F);
}

/**
 * Given the last draw before 1
 * When the spawn position is asked
 * Then the Bydo appears one margin above the status band of the clients, on
 * row 920, never under the band
 */
TEST(BydoSpawnPosition, TheLowestRowStaysAboveTheStatusBand) {
  EXPECT_NEAR(rtype::game::bydoSpawnPosition(JUST_UNDER_ONE).y,
              LOWEST_ROW_ABOVE_THE_STATUS_BAND, ROW_TOLERANCE);
}

/**
 * Given a draw of 1, below 0, or not a number
 * When the spawn position is asked
 * Then it is refused, one step outside each bound
 */
TEST(BydoSpawnPosition, RefusesADrawOutsideZeroToJustUnderOne) {
  for (const float draw :
       {ONE, BELOW_ZERO, std::numeric_limits<float>::quiet_NaN()}) {
    EXPECT_THROW(std::ignore = rtype::game::bydoSpawnPosition(draw),
                 rtype::game::InvalidRandomUnitException);
  }
}
