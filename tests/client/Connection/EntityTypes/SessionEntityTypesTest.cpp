#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include "EntitySide.hpp"
#include "EntityTypeConstants.hpp"
#include "EntityTypeEntry.hpp"
#include "SessionEntityTypes.hpp"
#include "SessionEntityTypesFixtures.hpp"
#include "SessionEntityTypesTestConstants.hpp"
#include "ShipGun.hpp"

namespace rtype::client {

using identifiers::NO_ENTITY_TYPE;
using network::EntitySide;
using network::EntityTypeEntry;
using testing::DRAWN_SHIP_NAME;
using testing::drawnTypes;
using ::testing::ElementsAre;
using testing::FIRST_TYPE_NUMBER;
using testing::isDrawnInTheTests;
using ::testing::IsEmpty;
using testing::oneUndrawnType;
using testing::OTHER_UNDRAWN_ENEMY_NAME;
using testing::SECOND_TYPE_NUMBER;
using testing::SOME_SHIP_GUN;
using testing::THIRD_TYPE_NUMBER;
using testing::THIRD_UNDRAWN_ENEMY_NAME;
using testing::threeUndrawnTypes;
using testing::UNDRAWN_ENEMY_NAME;

/**
 * Given a table that lists an enemy, then a ship
 * When the second number is looked up
 * Then it is the ship, with its side and its name
 */
TEST(SessionEntityTypes, NumbersTheTypesInTheOrderOfTheTable) {
  SessionEntityTypes table{isDrawnInTheTests};
  table.keep(drawnTypes());

  const EntityTypeEntry* const entry = table.entryNumbered(SECOND_TYPE_NUMBER);

  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(*entry, (EntityTypeEntry{.side = EntitySide::Player,
                                     .name = std::string{DRAWN_SHIP_NAME}}));
}

/**
 * Given a table of two types
 * When the third number is looked up
 * Then it names no type
 */
TEST(SessionEntityTypes, KnowsNoNumberPastTheTable) {
  SessionEntityTypes table{isDrawnInTheTests};
  table.keep(drawnTypes());

  EXPECT_EQ(table.entryNumbered(THIRD_TYPE_NUMBER), nullptr);
}

/**
 * Given a table of two types
 * When NO_ENTITY_TYPE is looked up
 * Then it names no type
 */
TEST(SessionEntityTypes, TheNumberOfNoTypeNamesNone) {
  SessionEntityTypes table{isDrawnInTheTests};
  table.keep(drawnTypes());

  EXPECT_EQ(table.entryNumbered(NO_ENTITY_TYPE), nullptr);
}

/**
 * Given a table kept for a session
 * When it is forgotten
 * Then its first number names no type
 */
TEST(SessionEntityTypes, KnowsNoNumberOnceForgotten) {
  SessionEntityTypes table{isDrawnInTheTests};
  table.keep(drawnTypes());

  table.forget();

  EXPECT_EQ(table.entryNumbered(FIRST_TYPE_NUMBER), nullptr);
}

/**
 * Given a table kept for a session with the gun of its ships
 * When the gun is asked for
 * Then it is that gun
 */
TEST(SessionEntityTypes, KeepsTheGunOfTheShips) {
  SessionEntityTypes table{isDrawnInTheTests};

  table.keep(drawnTypes(), SOME_SHIP_GUN);

  EXPECT_EQ(table.shipGun(), SOME_SHIP_GUN);
}

/**
 * Given a table kept with the gun of its ships
 * When it is forgotten
 * Then the gun predicts nothing any more
 */
TEST(SessionEntityTypes, ForgetsTheGunWithTheTable) {
  SessionEntityTypes table{isDrawnInTheTests};
  table.keep(drawnTypes(), SOME_SHIP_GUN);

  table.forget();

  EXPECT_EQ(table.shipGun(), network::ShipGun{});
}

/**
 * Given a client that draws every type of a table
 * When the names it cannot draw are asked for
 * Then there is none
 */
TEST(SessionEntityTypes, ListsNoNameWhenItDrawsThemAll) {
  const SessionEntityTypes table{isDrawnInTheTests};

  EXPECT_THAT(table.undrawableNamesOf(drawnTypes()), IsEmpty());
}

/**
 * Given a table with one type the client cannot draw
 * When the names it cannot draw are asked for
 * Then that name alone is listed
 */
TEST(SessionEntityTypes, ListsTheNameItCannotDraw) {
  const SessionEntityTypes table{isDrawnInTheTests};

  EXPECT_THAT(table.undrawableNamesOf(oneUndrawnType()),
              ElementsAre(std::string{UNDRAWN_ENEMY_NAME}));
}

/**
 * Given a table with three types the client cannot draw, a drawn one among
 * them
 * When the names it cannot draw are asked for
 * Then the three are listed, in the order of the table
 */
TEST(SessionEntityTypes, ListsEveryNameItCannotDrawInTheOrderOfTheTable) {
  const SessionEntityTypes table{isDrawnInTheTests};

  EXPECT_THAT(table.undrawableNamesOf(threeUndrawnTypes()),
              ElementsAre(std::string{UNDRAWN_ENEMY_NAME},
                          std::string{OTHER_UNDRAWN_ENEMY_NAME},
                          std::string{THIRD_UNDRAWN_ENEMY_NAME}));
}

}  // namespace rtype::client
