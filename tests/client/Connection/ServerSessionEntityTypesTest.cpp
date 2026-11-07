#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <string>
#include "ConnectionPhase.hpp"
#include "ConnectionStatus.hpp"
#include "EntitySide.hpp"
#include "EntityTypeEntry.hpp"
#include "ServerSessionEntityTypesFixture.hpp"
#include "ServerSessionEntityTypesTestConstants.hpp"
#include "ServerSessionTestConstants.hpp"
#include "SessionEntityTypesFixtures.hpp"
#include "SessionEntityTypesTestConstants.hpp"

namespace rtype::client {

using network::EntitySide;
using network::EntityTypeEntry;
using testing::DISCONNECTION_COPIES_AFTER_THE_FIRST;
using testing::DISCONNECTION_COPIES_SENT;
using testing::DRAWN_ENEMY_NAME;
using testing::DRAWN_SHIP_NAME;
using testing::drawnTypes;
using ::testing::Each;
using ::testing::ElementsAre;
using testing::FIRST_TYPE_NUMBER;
using testing::oneUndrawnType;
using testing::OTHER_DRAWN_ENEMY_NAME;
using testing::otherDrawnTypes;
using testing::PROTOCOL_DISCONNECTION_COPY_INTERVAL;
using testing::SECOND_TYPE_NUMBER;
using testing::ServerSessionEntityTypesFixture;
using testing::SESSION_TOKEN;
using ::testing::SizeIs;
using testing::SOME_SHIP_GUN;
using testing::THIRD_TYPE_NUMBER;
using testing::threeUndrawnTypes;
using testing::UNDRAWN_ENEMY_NAME;

/**
 * Given a client connecting to a server
 * When the server accepts it with a table this client draws whole
 * Then the session opens
 */
TEST_F(ServerSessionEntityTypesFixture, AnAcceptanceItDrawsOpensTheSession) {
  connectAndGetAcceptedWith(drawnTypes());

  EXPECT_EQ(session().status().phase, ConnectionPhase::Accepted);
}

/**
 * Given a session accepted with a table that lists an enemy, then a ship
 * When its second number is looked up
 * Then it is the ship, with its side and its name
 */
TEST_F(ServerSessionEntityTypesFixture,
       TheSessionKeepsTheTableOfItsAcceptance) {
  connectAndGetAcceptedWith(drawnTypes());

  const EntityTypeEntry* const entry =
      entityTypes().entryNumbered(SECOND_TYPE_NUMBER);

  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(*entry, (EntityTypeEntry{.side = EntitySide::Player,
                                     .name = std::string{DRAWN_SHIP_NAME}}));
}

/**
 * Given a client connecting to a server
 * When the server accepts it with the gun of its ships
 * Then the session keeps that gun with the table
 */
TEST_F(ServerSessionEntityTypesFixture, TheSessionKeepsTheGunOfItsAcceptance) {
  connectAndGetAcceptedWith(drawnTypes(), SOME_SHIP_GUN);

  EXPECT_EQ(entityTypes().shipGun(), SOME_SHIP_GUN);
}

/**
 * Given a session accepted with a table of two types
 * When an entity of its third number is looked up
 * Then it names no type, so the entity is skipped
 */
TEST_F(ServerSessionEntityTypesFixture, ANumberPastTheTableNamesNoType) {
  connectAndGetAcceptedWith(drawnTypes());

  EXPECT_EQ(entityTypes().entryNumbered(THIRD_TYPE_NUMBER), nullptr);
}

/**
 * Given a client connecting to a server
 * When the server accepts it with a table naming a type it has no sheet for
 * Then no session opens: the attempt ends as MissingEntitySheet
 */
TEST_F(ServerSessionEntityTypesFixture, AMissingSheetEndsTheAttempt) {
  connectAndGetAcceptedWith(oneUndrawnType());

  EXPECT_EQ(session().status().phase, ConnectionPhase::MissingEntitySheet);
}

/**
 * Given a client connecting to a server
 * When the server accepts it with a table naming one type it has no sheet for
 * Then the status names that sheet, and no other is missing
 */
TEST_F(ServerSessionEntityTypesFixture, TheStatusNamesTheMissingSheet) {
  connectAndGetAcceptedWith(oneUndrawnType());

  EXPECT_EQ(session().status(),
            (ConnectionStatus{.phase = ConnectionPhase::MissingEntitySheet,
                              .missingSheet = std::string{UNDRAWN_ENEMY_NAME},
                              .otherMissingSheets = 0}));
}

/**
 * Given a client connecting to a server
 * When the server accepts it with a table naming three types it has no sheet
 * for
 * Then the status names the first of them, and counts the two others
 */
TEST_F(ServerSessionEntityTypesFixture, TheStatusCountsTheOtherMissingSheets) {
  connectAndGetAcceptedWith(threeUndrawnTypes());

  EXPECT_EQ(session().status(),
            (ConnectionStatus{.phase = ConnectionPhase::MissingEntitySheet,
                              .missingSheet = std::string{UNDRAWN_ENEMY_NAME},
                              .otherMissingSheets = 2}));
}

/**
 * Given a client connecting to a server
 * When the server accepts it with a table naming a type it has no sheet for
 * Then a Disconnection carrying the token of the acceptance goes out at once,
 * so the server frees the seat it gave
 */
TEST_F(ServerSessionEntityTypesFixture, TellsTheServerItLeavesAtOnce) {
  connectAndGetAcceptedWith(oneUndrawnType());

  EXPECT_THAT(disconnectionTokens(), ElementsAre(SESSION_TOKEN));
}

/**
 * Given a client that left an acceptance naming a sheet it lacks
 * When 100 ms pass, in two frames of 50 ms
 * Then the two other copies of the Disconnection went out, under the same
 * token, as when the player leaves
 */
TEST_F(ServerSessionEntityTypesFixture, SendsEveryCopyOfItsDisconnection) {
  connectAndGetAcceptedWith(oneUndrawnType());

  for (std::size_t copy = 0; copy < DISCONNECTION_COPIES_AFTER_THE_FIRST;
       ++copy) {
    update(PROTOCOL_DISCONNECTION_COPY_INTERVAL);
  }

  EXPECT_THAT(disconnectionTokens(), SizeIs(DISCONNECTION_COPIES_SENT));
  EXPECT_THAT(disconnectionTokens(), Each(SESSION_TOKEN));
}

/**
 * Given a client that left an acceptance naming a sheet it lacks
 * When the first number is looked up
 * Then it names no type: no table is kept for a session that never opened
 */
TEST_F(ServerSessionEntityTypesFixture, KeepsNoTableWhenASheetIsMissing) {
  connectAndGetAcceptedWith(oneUndrawnType());

  EXPECT_EQ(entityTypes().entryNumbered(FIRST_TYPE_NUMBER), nullptr);
}

/**
 * Given a session accepted with a table
 * When the server repeats its acceptance with another table, in a newer
 * datagram, as it does for a request that crossed the first answer
 * Then the table of the session stays the first one
 */
TEST_F(ServerSessionEntityTypesFixture, ARepeatedAcceptanceChangesNothing) {
  connectAndGetAcceptedWith(drawnTypes());

  getAcceptedAgainWith(otherDrawnTypes());

  const EntityTypeEntry* const entry =
      entityTypes().entryNumbered(FIRST_TYPE_NUMBER);

  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(*entry, (EntityTypeEntry{.side = EntitySide::Enemy,
                                     .name = std::string{DRAWN_ENEMY_NAME}}));
}

/**
 * Given a session accepted with a table
 * When the session is left
 * Then its table is forgotten
 */
TEST_F(ServerSessionEntityTypesFixture, ForgetsTheTableWhenTheSessionCloses) {
  connectAndGetAcceptedWith(drawnTypes());

  session().cancel();

  EXPECT_EQ(entityTypes().entryNumbered(FIRST_TYPE_NUMBER), nullptr);
}

/**
 * Given a session accepted with a table, then left
 * When a new session is accepted with another table
 * Then the new session numbers its types by the new table
 */
TEST_F(ServerSessionEntityTypesFixture,
       ANewSessionTakesTheTableOfItsAcceptance) {
  connectAndGetAcceptedWith(drawnTypes());
  session().cancel();

  connectAndGetAcceptedWith(otherDrawnTypes());

  const EntityTypeEntry* const entry =
      entityTypes().entryNumbered(FIRST_TYPE_NUMBER);

  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(*entry,
            (EntityTypeEntry{.side = EntitySide::Enemy,
                             .name = std::string{OTHER_DRAWN_ENEMY_NAME}}));
}

/**
 * Given an attempt that ended because a sheet was missing
 * When a new attempt is accepted with a table this client draws whole
 * Then the session opens
 */
TEST_F(ServerSessionEntityTypesFixture, ANewAttemptAfterAMissingSheetCanOpen) {
  connectAndGetAcceptedWith(oneUndrawnType());

  connectAndGetAcceptedWith(drawnTypes());

  EXPECT_EQ(session().status().phase, ConnectionPhase::Accepted);
}

}  // namespace rtype::client
