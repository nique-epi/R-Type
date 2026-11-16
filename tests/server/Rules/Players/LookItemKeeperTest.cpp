#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EntityIdConstants.hpp"
#include "LookItemKeeperFixture.hpp"
#include "LookItemKeeperTestConstants.hpp"
#include "LookItemWon.hpp"

namespace rtype::server::testing {

/**
 * Given the ships of players 1 and 2 and a crown, and the touches published
 * player 2 first, then player 1, and delivered
 * When the keeper runs its next tick
 * Then player 1, the lowest number, alone is told it won the crown
 */
TEST_F(LookItemKeeperFixture,
       TheLowestPlayerNumberWinsAnItemTouchedByTwoShipsInOneTick) {
  const ConnectedPlayer& first = join(FIRST_SEAT);
  const ConnectedPlayer& second = join(SECOND_SEAT);
  const std::uint32_t crown = spawnACrown();
  publishTouch(second, crown);
  publishTouch(first, crown);
  deliver();

  tick();

  EXPECT_EQ(wonBy(FIRST_SEAT),
            (std::vector<network::LookItemWon>{{.item = CROWN_NUMBER}}));
  EXPECT_TRUE(wonBy(SECOND_SEAT).empty());
}

/**
 * Given the ship of player 1 and a crown it touched, delivered
 * When the keeper runs its next tick
 * Then player 1 keeps the crown among the items it won in the game
 */
TEST_F(LookItemKeeperFixture, TheWinnerKeepsTheItemForTheResults) {
  const ConnectedPlayer& first = join(FIRST_SEAT);
  publishTouch(first, spawnACrown());
  deliver();

  tick();

  EXPECT_EQ(first.wonLookNumbers, (std::vector<std::uint8_t>{CROWN_NUMBER}));
}

/**
 * Given the ship of player 1 and two crowns it touched in one tick, delivered
 * When the keeper runs its next tick
 * Then player 1 keeps the crown once among the items it won
 */
TEST_F(LookItemKeeperFixture, AnItemWonTwiceIsKeptOnce) {
  const ConnectedPlayer& first = join(FIRST_SEAT);
  publishTouch(first, spawnACrown());
  publishTouch(first, spawnACrown());
  deliver();

  tick();

  EXPECT_EQ(first.wonLookNumbers, (std::vector<std::uint8_t>{CROWN_NUMBER}));
}

/**
 * Given a crown touched by two ships in one tick
 * When the keeper runs its next tick and the bus delivers its events
 * Then the crown is removed once, and gone from the world
 */
TEST_F(LookItemKeeperFixture, TheItemIsRemovedOnce) {
  const ConnectedPlayer& first = join(FIRST_SEAT);
  const ConnectedPlayer& second = join(SECOND_SEAT);
  const std::uint32_t crown = spawnACrown();
  publishTouch(first, crown);
  publishTouch(second, crown);
  deliver();

  tick();
  deliver();

  EXPECT_EQ(removalsOf(crown), 1U);
  EXPECT_FALSE(worldHolds(crown));
}

/**
 * Given a touch delivered
 * When the keeper has not run its tick
 * Then nothing was sent and the crown is still in the world
 */
TEST_F(LookItemKeeperFixture, ATouchAloneGivesNothingBeforeTheNextTick) {
  const ConnectedPlayer& first = join(FIRST_SEAT);
  const std::uint32_t crown = spawnACrown();
  publishTouch(first, crown);

  deliver();

  EXPECT_TRUE(wonBy(FIRST_SEAT).empty());
  EXPECT_TRUE(worldHolds(crown));
}

/**
 * Given a crown touched by players 1 and 2, then the ship of player 1
 * destroyed before the tick
 * When the keeper runs its tick
 * Then player 2 wins the crown
 */
TEST_F(LookItemKeeperFixture, APilotWhoseShipWasDestroyedWinsNothing) {
  ConnectedPlayer& first = join(FIRST_SEAT);
  const ConnectedPlayer& second = join(SECOND_SEAT);
  const std::uint32_t crown = spawnACrown();
  publishTouch(first, crown);
  publishTouch(second, crown);
  deliver();
  first.shipEntityId = identifiers::NO_ENTITY_ID;

  tick();

  EXPECT_TRUE(wonBy(FIRST_SEAT).empty());
  EXPECT_EQ(wonBy(SECOND_SEAT),
            (std::vector<network::LookItemWon>{{.item = CROWN_NUMBER}}));
}

/**
 * Given a crown touched by the only ship that lost its pilot before the tick
 * When the keeper runs its tick
 * Then the crown stays in the world and nothing is sent
 */
TEST_F(LookItemKeeperFixture, AnItemNobodyCanWinStays) {
  ConnectedPlayer& first = join(FIRST_SEAT);
  const std::uint32_t crown = spawnACrown();
  publishTouch(first, crown);
  deliver();
  first.shipEntityId = identifiers::NO_ENTITY_ID;

  tick();

  EXPECT_TRUE(worldHolds(crown));
  EXPECT_TRUE(wonBy(FIRST_SEAT).empty());
}

}  // namespace rtype::server::testing
