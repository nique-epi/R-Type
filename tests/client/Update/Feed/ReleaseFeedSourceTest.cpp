#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "ReleaseFeedSourceFixture.hpp"
#include "ReleaseFeedSourceTestConstants.hpp"

using rtype::client::testing::FEED_NAME;
using rtype::client::testing::FEED_PATH;
using rtype::client::testing::FEED_TEXT;
using rtype::client::testing::LEAVING_FEED_NAME;
using rtype::client::testing::LEAVING_FEED_PATH;
using rtype::client::testing::LEAVING_PACKAGE_FILE;
using rtype::client::testing::LEAVING_PACKAGE_PATH;
using rtype::client::testing::PACKAGE_CONTENT;
using rtype::client::testing::PACKAGE_FILE;
using rtype::client::testing::PACKAGE_PATH;
using rtype::client::testing::WHOLE_PACKAGE;

/**
 * Given the latest release serving a feed
 * When Velopack asks for that feed
 * Then it gets the feed
 */
TEST_F(ReleaseFeedSourceFixture, HandsOverTheFeedOfTheLatestRelease) {
  files().serve(std::string{FEED_PATH}, std::string{FEED_TEXT});

  EXPECT_EQ(source().GetReleaseFeed(std::string{FEED_NAME}), FEED_TEXT);
}

/**
 * Given a latest release serving no feed
 * When Velopack asks for one
 * Then it gets an empty text, which it refuses
 */
TEST_F(ReleaseFeedSourceFixture, HandsOverNothingWhenTheFeedIsMissing) {
  EXPECT_EQ(source().GetReleaseFeed(std::string{FEED_NAME}), "");
}

/**
 * Given a feed served at a path outside the latest release
 * When Velopack asks for a feed whose name climbs to that path
 * Then it gets an empty text
 */
TEST_F(ReleaseFeedSourceFixture, RefusesAFeedNameThatLeavesTheRelease) {
  files().serve(std::string{LEAVING_FEED_PATH}, std::string{FEED_TEXT});

  EXPECT_EQ(source().GetReleaseFeed(std::string{LEAVING_FEED_NAME}), "");
}

/**
 * Given release 1.1.0 serving a package
 * When Velopack asks to download that package of version 1.1.0
 * Then it is read from the release of its version
 */
TEST_F(ReleaseFeedSourceFixture, ReadsAPackageFromTheReleaseOfItsVersion) {
  files().serve(std::string{PACKAGE_PATH}, std::string{PACKAGE_CONTENT});

  ASSERT_TRUE(save(packageNamed(PACKAGE_FILE)));

  EXPECT_EQ(files().savedPaths(),
            std::vector<std::string>{std::string{PACKAGE_PATH}});
}

/**
 * Given release 1.1.0 serving a package
 * When it is downloaded
 * Then Velopack hears the whole package received
 */
TEST_F(ReleaseFeedSourceFixture, ReportsTheWholePackageOnceSaved) {
  files().serve(std::string{PACKAGE_PATH}, std::string{PACKAGE_CONTENT});

  ASSERT_TRUE(save(packageNamed(PACKAGE_FILE)));

  EXPECT_EQ(lastProgress(), WHOLE_PACKAGE);
}

/**
 * Given a package served where a name climbing out of its release leads
 * When Velopack asks to download it
 * Then nothing is read and the download fails
 */
TEST_F(ReleaseFeedSourceFixture, RefusesAPackageNameThatLeavesTheRelease) {
  files().serve(std::string{LEAVING_PACKAGE_PATH},
                std::string{PACKAGE_CONTENT});

  EXPECT_FALSE(save(packageNamed(LEAVING_PACKAGE_FILE)));
  EXPECT_TRUE(files().savedPaths().empty());
}

/**
 * Given a source reading release files
 * When it is stopped
 * Then the files stop their requests
 */
TEST_F(ReleaseFeedSourceFixture, StopsItsFiles) {
  source().stop();

  EXPECT_TRUE(files().stopped());
}
