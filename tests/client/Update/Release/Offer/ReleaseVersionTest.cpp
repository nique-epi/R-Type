#include <gtest/gtest.h>
#include <optional>
#include "ReleaseVersion.hpp"
#include "ReleaseVersionTestConstants.hpp"

using rtype::client::ReleaseVersion;
using rtype::client::releaseVersionOf;
using rtype::client::testing::LARGEST_VERSION_NUMBER_TEXT;
using rtype::client::testing::NUMBER_PAST_ITS_WIDTH_TEXT;

/**
 * Given a version written as project() writes it
 * When it is read
 * Then each of its three numbers is read
 */
TEST(ReleaseVersion, ReadsThreeNumbers) {
  const std::optional<ReleaseVersion> version = releaseVersionOf("1.10.0");

  EXPECT_EQ(
      version,
      (ReleaseVersion{.majorNumber = 1, .minorNumber = 10, .patchNumber = 0}));
}

/**
 * Given texts that are not three numbers joined by dots
 * When each is read
 * Then none is a version
 */
TEST(ReleaseVersion, RefusesWhatIsNotThreeNumbers) {
  EXPECT_FALSE(releaseVersionOf("1.2").has_value());
  EXPECT_FALSE(releaseVersionOf("1.2.3.4").has_value());
  EXPECT_FALSE(releaseVersionOf("1..3").has_value());
  EXPECT_FALSE(releaseVersionOf("v1.2.3").has_value());
  EXPECT_FALSE(releaseVersionOf("1.2.3 ").has_value());
  EXPECT_FALSE(releaseVersionOf("").has_value());
}

/**
 * Given a number one past the largest std::uint32_t, whose cut would be 0
 * When it is read, then the largest one
 * Then the first is refused and the second read
 */
TEST(ReleaseVersion, RefusesANumberPastItsWidth) {
  EXPECT_FALSE(releaseVersionOf(NUMBER_PAST_ITS_WIDTH_TEXT).has_value());
  EXPECT_TRUE(releaseVersionOf(LARGEST_VERSION_NUMBER_TEXT).has_value());
}

/**
 * Given 1.10.0, which as text comes before 1.9.9
 * When both are compared
 * Then 1.10.0 is the newer
 */
TEST(ReleaseVersion, OrdersNumberByNumberNotAsText) {
  const ReleaseVersion newer =
      releaseVersionOf("1.10.0").value_or(ReleaseVersion{});
  const ReleaseVersion older =
      releaseVersionOf("1.9.9").value_or(ReleaseVersion{});

  EXPECT_GT(newer, older);
}
