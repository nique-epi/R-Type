#include <gtest/gtest.h>
#include <string>
#include "ContentFingerprint.hpp"
#include "ContentFingerprintText.hpp"
#include "ContentPack.hpp"
#include "ContentPackFixtures.hpp"
#include "ContentPackTestConstants.hpp"

using rtype::network::ContentFingerprint;
using rtype::network::contentFingerprintOf;
using rtype::network::encodeContentPack;
using rtype::network::fingerprintText;
using rtype::network::testing::FINGERPRINT_TEXT_LENGTH;
using rtype::network::testing::LOWERCASE_DIGITS;
using rtype::network::testing::twoContentFiles;

/**
 * Given the fingerprint of a pack
 * When it is written as text
 * Then it takes 64 lowercase hexadecimal digits
 */
TEST(ContentFingerprintText, WritesSixtyFourLowercaseDigits) {
  const ContentFingerprint fingerprint =
      contentFingerprintOf(encodeContentPack(twoContentFiles()));

  const std::string text = fingerprintText(fingerprint);

  EXPECT_EQ(text.size(), FINGERPRINT_TEXT_LENGTH);
  EXPECT_EQ(text.find_first_not_of(LOWERCASE_DIGITS), std::string::npos);
}
