#include <gtest/gtest.h>
#include "ContentPackTestFiles.hpp"
#include "ContentPackTestFilesConstants.hpp"
#include "EntityType.hpp"
#include "EntityTypeConstants.hpp"
#include "NumberedEntityType.hpp"
#include "PackContentReading.hpp"
#include "SessionEntityTypes.hpp"
#include "SessionEntityTypesFixtures.hpp"
#include "SessionEntityTypesTestConstants.hpp"
#include "ShipGun.hpp"
#include "TemporaryFolder.hpp"

namespace rtype::client {

using identifiers::NO_ENTITY_TYPE;
using testing::DRAWN_ENEMY_NAME;
using testing::DRAWN_SHIP_NAME;
using testing::FIRST_TYPE_NUMBER;
using testing::LANCER_TYPE;
using testing::lancerFiles;
using testing::LOCAL_SHIP_GUN;
using testing::SECOND_TYPE_NUMBER;
using testing::testLocalContent;
using testing::THIRD_TYPE_NUMBER;

/**
 * Given the client's own content, an enemy then a ship
 * When the second number is looked up
 * Then it is the ship, with its side and its name
 */
TEST(SessionEntityTypes, NumbersItsOwnTypesInTheirOrder) {
  const SessionEntityTypes table{testLocalContent()};

  const game::NumberedEntityType* const entry =
      table.entryNumbered(SECOND_TYPE_NUMBER);

  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->name, DRAWN_SHIP_NAME);
  EXPECT_EQ(entry->side, game::EntityType::Player);
}

/**
 * Given the client's own content of two types
 * When the third number and the number of no type are looked up
 * Then neither names a type
 */
TEST(SessionEntityTypes, KnowsNoNumberPastItsTypesNorNoType) {
  const SessionEntityTypes table{testLocalContent()};

  EXPECT_EQ(table.entryNumbered(THIRD_TYPE_NUMBER), nullptr);
  EXPECT_EQ(table.entryNumbered(NO_ENTITY_TYPE), nullptr);
}

/**
 * Given the client's own content
 * When its fingerprint is asked
 * Then it is the one it was built with
 */
TEST(SessionEntityTypes, KeepsTheFingerprintOfItsOwnContent) {
  const SessionEntityTypes table{testLocalContent()};

  EXPECT_EQ(table.localFingerprint(), testLocalContent().fingerprint);
}

/**
 * Given a session that plays the pack of a server, of one Bydo
 * When its first number is looked up
 * Then it is the Bydo of the pack, drawn with the appearance of the pack
 */
TEST(SessionEntityTypes, NumbersTheTypesOfThePackItPlays) {
  const TemporaryFolder folder;
  SessionEntityTypes table{testLocalContent()};

  table.use(packContentOf(lancerFiles(), folder.path()));

  const game::NumberedEntityType* const entry =
      table.entryNumbered(FIRST_TYPE_NUMBER);
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->name, LANCER_TYPE);
  EXPECT_NE(table.appearances().named(LANCER_TYPE), nullptr);
}

/**
 * Given a session that played the pack of a server
 * When the session is forgotten
 * Then the client's own types are numbered again
 */
TEST(SessionEntityTypes, ComesBackToItsOwnTypesOnceForgotten) {
  const TemporaryFolder folder;
  SessionEntityTypes table{testLocalContent()};
  table.use(packContentOf(lancerFiles(), folder.path()));

  table.forget();

  const game::NumberedEntityType* const entry =
      table.entryNumbered(FIRST_TYPE_NUMBER);
  ASSERT_NE(entry, nullptr);
  EXPECT_EQ(entry->name, DRAWN_ENEMY_NAME);
  EXPECT_EQ(table.appearances().named(LANCER_TYPE), nullptr);
}

/**
 * Given the client's own content, whose ship has a gun
 * When the gun of the ships is asked
 * Then it is the gun of that content
 */
TEST(SessionEntityTypes, ArmsTheShipsWithTheGunOfItsOwnContent) {
  const SessionEntityTypes table{testLocalContent()};

  EXPECT_EQ(table.shipGun(), LOCAL_SHIP_GUN);
}

/**
 * Given a session that plays the pack of a server, of one Bydo and no ship
 * When the gun of the ships is asked
 * Then it is the gun of the pack, which predicts nothing
 */
TEST(SessionEntityTypes, ArmsTheShipsWithTheGunOfThePackItPlays) {
  const TemporaryFolder folder;
  SessionEntityTypes table{testLocalContent()};

  table.use(packContentOf(lancerFiles(), folder.path()));

  EXPECT_EQ(table.shipGun(), game::ShipGun{});
}

/**
 * Given a session that played the pack of a server
 * When the session is forgotten
 * Then the ships have the gun of the client's own content again
 */
TEST(SessionEntityTypes, ComesBackToItsOwnGunOnceForgotten) {
  const TemporaryFolder folder;
  SessionEntityTypes table{testLocalContent()};
  table.use(packContentOf(lancerFiles(), folder.path()));

  table.forget();

  EXPECT_EQ(table.shipGun(), LOCAL_SHIP_GUN);
}

}  // namespace rtype::client
