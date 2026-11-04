#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <string>
#include "ServerKeyTextTestConstants.hpp"
#include "ServerKeysFixture.hpp"
#include "ServerKeysTestConstants.hpp"

/**
 * Given a key the master issued
 * When a request presents it
 * Then the master knows which key it is
 */
TEST_F(ServerKeysFixture, RecognisesAKeyItIssued) {
  const std::string key =
      keys().issue(std::string{PARIS_KEY_NAME}, KEY_ISSUE_DATE);

  EXPECT_TRUE(keys().keyIdOf(key).has_value());
}

/**
 * Given two keys the master issued
 * When each is presented
 * Then each is a key of its own
 */
TEST_F(ServerKeysFixture, TwoKeysAreTwoIds) {
  const std::string paris =
      keys().issue(std::string{PARIS_KEY_NAME}, KEY_ISSUE_DATE);
  const std::string lyon =
      keys().issue(std::string{LYON_KEY_NAME}, KEY_ISSUE_DATE);

  const std::optional<std::int64_t> parisId = keys().keyIdOf(paris);
  ASSERT_TRUE(parisId.has_value());
  EXPECT_NE(keys().keyIdOf(lyon), parisId);
}

/**
 * Given a key the master issued
 * When the files of the database are read
 * Then the key is written in none of them
 */
TEST_F(ServerKeysFixture, TheDatabaseNeverHoldsTheKey) {
  const std::string key =
      keys().issue(std::string{PARIS_KEY_NAME}, KEY_ISSUE_DATE);

  EXPECT_FALSE(databaseFilesHold(key));
  EXPECT_TRUE(databaseFilesHold(PARIS_KEY_NAME));
}

/**
 * Given a key of the right shape that the master never issued
 * When a request presents it
 * Then the master knows no such key
 */
TEST_F(ServerKeysFixture, RefusesAKeyItNeverIssued) {
  static_cast<void>(keys().issue(std::string{PARIS_KEY_NAME}, KEY_ISSUE_DATE));

  EXPECT_EQ(keys().keyIdOf(KEY_NEVER_ISSUED), std::nullopt);
}

/**
 * Given a key the master issued before it restarted
 * When a request presents it after the restart
 * Then the master still knows it
 */
TEST_F(ServerKeysFixture, KnowsAKeyAfterARestart) {
  const std::string key =
      keys().issue(std::string{PARIS_KEY_NAME}, KEY_ISSUE_DATE);

  restart();

  EXPECT_TRUE(keys().keyIdOf(key).has_value());
}
