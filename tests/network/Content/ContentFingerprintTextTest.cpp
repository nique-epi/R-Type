#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <string_view>
#include "ContentFingerprint.hpp"
#include "ContentFingerprintText.hpp"
#include "ContentPack.hpp"
#include "ContentPackFixtures.hpp"
#include "ContentPackTestConstants.hpp"

using rtype::network::ContentFingerprint;
using rtype::network::contentFingerprintOf;
using rtype::network::encodeContentPack;
using rtype::network::fingerprintFromText;
using rtype::network::fingerprintText;
using rtype::network::testing::FINGERPRINT_TEXT_LENGTH;
using rtype::network::testing::LOWERCASE_DIGITS;
using rtype::network::testing::REFUSED_FINGERPRINT_TEXTS;
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

/**
 * Given the fingerprint of a pack written as text
 * When the text is read back
 * Then it gives the same fingerprint
 */
TEST(ContentFingerprintText, ReadsBackTheFingerprintItWrote) {
  const ContentFingerprint fingerprint =
      contentFingerprintOf(encodeContentPack(twoContentFiles()));

  const std::optional<ContentFingerprint> read =
      fingerprintFromText(fingerprintText(fingerprint));

  ASSERT_TRUE(read.has_value());
  EXPECT_EQ(read.value_or(ContentFingerprint{}), fingerprint);
}

/**
 * Given texts of another length, with an uppercase digit or a letter that is
 * no digit
 * When each is read as a fingerprint
 * Then none gives one
 */
TEST(ContentFingerprintText, RefusesATextThatWritesNoFingerprint) {
  for (const std::string_view text : REFUSED_FINGERPRINT_TEXTS) {
    SCOPED_TRACE(std::string{text});
    EXPECT_FALSE(fingerprintFromText(text).has_value());
  }
}
