#include <gtest/gtest.h>
#include <string>
#include "MasterMessageText.hpp"
#include "MasterTestConstants.hpp"
#include "ServerKey.hpp"
#include "ServerKeyTestConstants.hpp"

using rtype::master::fingerprintOf;
using rtype::master::newServerKey;
using rtype::network::isWellFormedServerKey;

/**
 * Given a new key
 * When it is read
 * Then it is the prefix and 64 hexadecimal digits, a well-formed key
 */
TEST(ServerKey, ANewKeyIsThePrefixAndSixtyFourDigits) {
  const std::string key = newServerKey();

  EXPECT_EQ(key.size(), EXPECTED_KEY_SIZE);
  EXPECT_TRUE(key.starts_with(EXPECTED_KEY_PREFIX));
  EXPECT_TRUE(isWellFormedServerKey(key));
}

/**
 * Given two new keys
 * When they are compared
 * Then they differ, since each is drawn from the random source
 */
TEST(ServerKey, TwoNewKeysDiffer) { EXPECT_NE(newServerKey(), newServerKey()); }

/**
 * Given a key
 * When its fingerprint is computed twice
 * Then both are the same, and neither is the key
 */
TEST(ServerKey, TheFingerprintOfAKeyIsStableAndIsNotTheKey) {
  const std::string key = newServerKey();

  EXPECT_EQ(fingerprintOf(key), fingerprintOf(key));
  EXPECT_NE(fingerprintOf(key), key);
}

/**
 * Given two keys
 * When their fingerprints are computed
 * Then they differ
 */
TEST(ServerKey, TwoKeysHaveTwoFingerprints) {
  EXPECT_NE(fingerprintOf(newServerKey()), fingerprintOf(UNKNOWN_SERVER_KEY));
}
