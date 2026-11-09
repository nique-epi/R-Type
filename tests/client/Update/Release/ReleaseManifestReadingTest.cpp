#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>
#include "ReleaseManifestReading.hpp"
#include "ReleaseManifestReadingFixtures.hpp"
#include "ReleaseManifestReadingTestConstants.hpp"
#include "ReleaseOffer.hpp"

using rtype::client::ReleaseOffer;
using rtype::client::releaseOfferOf;
using rtype::client::testing::EMPTY_PACKAGE;
using rtype::client::testing::GOOD_LINUX_FILE;
using rtype::client::testing::GOOD_LINUX_PACKAGE;
using rtype::client::testing::GOOD_LINUX_SIZE;
using rtype::client::testing::HIDDEN_PACKAGE;
using rtype::client::testing::JSON_ARRAY;
using rtype::client::testing::manifestWithNotes;
using rtype::client::testing::manifestWithPackage;
using rtype::client::testing::NINE_NOTES;
using rtype::client::testing::NOT_JSON;
using rtype::client::testing::NOTE_TOO_LONG;
using rtype::client::testing::ONE_NOTE_LINE;
using rtype::client::testing::PACKAGE_GOING_UP;
using rtype::client::testing::PACKAGE_IN_A_FOLDER;
using rtype::client::testing::PACKAGE_OF_NEGATIVE_SIZE;
using rtype::client::testing::PACKAGE_OF_SHORT_DIGEST;
using rtype::client::testing::PACKAGE_OF_UPPERCASE_DIGEST;
using rtype::client::testing::PACKAGE_TOO_LARGE;

/**
 * Given a manifest of 1.1.0 with one note and a Linux package
 * When Linux reads it
 * Then it is offered the version, the note and the package
 */
TEST(ReleaseManifestReading, ReadsWhatItOffersThisSystem) {
  const std::optional<ReleaseOffer> offer =
      releaseOfferOf(manifestWithPackage(GOOD_LINUX_PACKAGE), "Linux");

  ASSERT_TRUE(offer.has_value());
  const ReleaseOffer read = offer.value_or(ReleaseOffer{});
  EXPECT_EQ(read.versionText, "1.1.0");
  EXPECT_EQ(read.notes, std::vector<std::string>{std::string{ONE_NOTE_LINE}});
  EXPECT_EQ(read.package.file, GOOD_LINUX_FILE);
  EXPECT_EQ(read.package.size, GOOD_LINUX_SIZE);
}

/**
 * Given a manifest with a Linux package only
 * When Windows reads it
 * Then it is offered nothing
 */
TEST(ReleaseManifestReading, OffersNothingToASystemItDoesNotName) {
  EXPECT_FALSE(releaseOfferOf(manifestWithPackage(GOOD_LINUX_PACKAGE), "win64")
                   .has_value());
}

/**
 * Given packages whose file name goes up, into a folder, or starts with a dot
 * When each manifest is read
 * Then none is offered
 */
TEST(ReleaseManifestReading, RefusesAFileNameThatLeavesItsFolder) {
  EXPECT_FALSE(releaseOfferOf(manifestWithPackage(PACKAGE_GOING_UP), "Linux")
                   .has_value());
  EXPECT_FALSE(releaseOfferOf(manifestWithPackage(PACKAGE_IN_A_FOLDER), "Linux")
                   .has_value());
  EXPECT_FALSE(
      releaseOfferOf(manifestWithPackage(HIDDEN_PACKAGE), "Linux").has_value());
}

/**
 * Given packages of size 0, one past the largest, and negative
 * When each manifest is read
 * Then none is offered
 */
TEST(ReleaseManifestReading, RefusesASizeOutOfItsBounds) {
  EXPECT_FALSE(
      releaseOfferOf(manifestWithPackage(EMPTY_PACKAGE), "Linux").has_value());
  EXPECT_FALSE(releaseOfferOf(manifestWithPackage(PACKAGE_TOO_LARGE), "Linux")
                   .has_value());
  EXPECT_FALSE(
      releaseOfferOf(manifestWithPackage(PACKAGE_OF_NEGATIVE_SIZE), "Linux")
          .has_value());
}

/**
 * Given packages whose digest has 63 digits, or uppercase ones
 * When each manifest is read
 * Then none is offered
 */
TEST(ReleaseManifestReading, RefusesADigestOfAnotherShape) {
  EXPECT_FALSE(
      releaseOfferOf(manifestWithPackage(PACKAGE_OF_SHORT_DIGEST), "Linux")
          .has_value());
  EXPECT_FALSE(
      releaseOfferOf(manifestWithPackage(PACKAGE_OF_UPPERCASE_DIGEST), "Linux")
          .has_value());
}

/**
 * Given a manifest of nine notes, then one of a note of 49 characters
 * When each is read
 * Then none is offered
 */
TEST(ReleaseManifestReading, RefusesNineNotesOrALongOne) {
  EXPECT_FALSE(
      releaseOfferOf(manifestWithNotes(NINE_NOTES), "Linux").has_value());
  EXPECT_FALSE(
      releaseOfferOf(manifestWithNotes(NOTE_TOO_LONG), "Linux").has_value());
}

/**
 * Given a text that is no JSON, then a JSON array
 * When each is read
 * Then none is offered
 */
TEST(ReleaseManifestReading, RefusesWhatIsNotAJsonObject) {
  EXPECT_FALSE(releaseOfferOf(NOT_JSON, "Linux").has_value());
  EXPECT_FALSE(releaseOfferOf(JSON_ARRAY, "Linux").has_value());
}
