#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include "ServerKeyText.hpp"
#include "ServerKeyTextTestConstants.hpp"

using rtype::master::fingerprintOf;
using rtype::master::isWellFormedServerKey;
using rtype::master::newServerKeyText;

/**
 * Given a new key
 * When it is read
 * Then it is 64 lowercase hexadecimal digits, a well-formed key
 */
TEST(ServerKeyText, ANewKeyIsSixtyFourHexadecimalDigits) {
  const std::string key = newServerKeyText();

  EXPECT_EQ(key.size(), EXPECTED_KEY_TEXT_SIZE);
  EXPECT_TRUE(isWellFormedServerKey(key));
}

/**
 * Given two new keys
 * When they are compared
 * Then they differ, since each is drawn from the random source
 */
TEST(ServerKeyText, TwoNewKeysDiffer) {
  EXPECT_NE(newServerKeyText(), newServerKeyText());
}

/**
 * Given texts of 63 and 65 digits, uppercase digits and a letter past f
 * When each is checked
 * Then none is a well-formed key
 */
TEST(ServerKeyText, RefusesATextShapedAlmostLikeAKey) {
  for (const std::string_view text :
       {KEY_OF_63_DIGITS, KEY_OF_65_DIGITS, UPPERCASE_KEY, KEY_WITH_A_G}) {
    SCOPED_TRACE(text);
    EXPECT_FALSE(isWellFormedServerKey(text));
  }
}

/**
 * Given a key
 * When its fingerprint is computed twice
 * Then both are the same, and neither is the key
 */
TEST(ServerKeyText, TheFingerprintOfAKeyIsStableAndIsNotTheKey) {
  const std::string key = newServerKeyText();

  EXPECT_EQ(fingerprintOf(key), fingerprintOf(key));
  EXPECT_NE(fingerprintOf(key), key);
}

/**
 * Given two keys
 * When their fingerprints are computed
 * Then they differ
 */
TEST(ServerKeyText, TwoKeysHaveTwoFingerprints) {
  EXPECT_NE(fingerprintOf(newServerKeyText()), fingerprintOf(KEY_NEVER_ISSUED));
}
