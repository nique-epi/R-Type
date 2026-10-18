#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include "GameException.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "ShipStartPosition.hpp"
#include "SpawningConstants.hpp"

namespace {

constexpr std::size_t PLAYER_COUNT = 4;
constexpr float SHIP_ROWS_MINIMUM_GAP = 200.0F;

}  // namespace

/**
 * Given a game of four players
 * When the start position of each player is asked
 * Then every ship sits at the same column, in the left tenth of the playfield
 */
TEST(ShipStartPosition, EveryShipStartsInTheLeftColumn) {
  for (std::uint8_t player = 1; player <= PLAYER_COUNT; ++player) {
    const rtype::game::Position position =
        rtype::game::shipStartPosition(player, PLAYER_COUNT);

    EXPECT_FLOAT_EQ(position.x, rtype::game::PLAYFIELD_WIDTH *
                                    rtype::game::SHIP_START_X_RATIO);
    EXPECT_LT(position.x, rtype::game::PLAYFIELD_WIDTH / 2);
  }
}

/**
 * Given a game of four players
 * When the start positions of the players 1 to 4 are asked
 * Then the rows follow the player numbers and any two are at least 200 units
 * apart, so the ships never overlap
 */
TEST(ShipStartPosition, ShipsOfTwoPlayersNeverOverlap) {
  for (std::uint8_t first = 1; first <= PLAYER_COUNT; ++first) {
    for (std::uint8_t second = first + 1; second <= PLAYER_COUNT; ++second) {
      const float firstRow =
          rtype::game::shipStartPosition(first, PLAYER_COUNT).y;
      const float secondRow =
          rtype::game::shipStartPosition(second, PLAYER_COUNT).y;

      EXPECT_LT(firstRow, secondRow);
      EXPECT_GE(secondRow - firstRow, SHIP_ROWS_MINIMUM_GAP);
    }
  }
}

/**
 * Given a game of four players
 * When the first and the last player are placed
 * Then both rows are inside the playfield, away from its edges
 */
TEST(ShipStartPosition, RowsStayInsideThePlayfield) {
  EXPECT_GT(rtype::game::shipStartPosition(1, PLAYER_COUNT).y, 0.0F);
  EXPECT_LT(rtype::game::shipStartPosition(PLAYER_COUNT, PLAYER_COUNT).y,
            rtype::game::PLAYFIELD_HEIGHT);
}

/**
 * Given a game of four players
 * When the start position of player 0 or of player 5 is asked
 * Then it is refused, one step outside each bound
 */
TEST(ShipStartPosition, RefusesANumberOutsideTheGame) {
  EXPECT_THROW(std::ignore = rtype::game::shipStartPosition(0, PLAYER_COUNT),
               rtype::game::InvalidPlayerNumberException);
  EXPECT_THROW(std::ignore = rtype::game::shipStartPosition(PLAYER_COUNT + 1,
                                                            PLAYER_COUNT),
               rtype::game::InvalidPlayerNumberException);
}
