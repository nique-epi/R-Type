#include <gtest/gtest.h>
#include <optional>
#include <string>
#include "FakeReleaseUpdater.hpp"
#include "ReleaseOffer.hpp"
#include "RunFixtures.hpp"
#include "RunTestConstants.hpp"
#include "UpdateCheck.hpp"

using rtype::client::ReleaseOffer;
using rtype::client::UpdateCheck;
using rtype::client::testing::heldWithinTheWait;
using rtype::client::testing::NOTES_VERSION;
using rtype::client::testing::TAG_NOTES;

/**
 * Given a feed offering a newer release
 * When the check runs
 * Then that release is found, with its notes
 */
TEST(UpdateCheck, FindsTheNewerReleaseTheFeedOffers) {
  FakeReleaseUpdater updater;
  updater.offer(ReleaseOffer{.versionText = std::string{NOTES_VERSION},
                             .notesMarkdown = std::string{TAG_NOTES}});
  const UpdateCheck check{updater};

  ASSERT_TRUE(heldWithinTheWait([&check] { return check.found(); }));
  const ReleaseOffer found = check.offer().value_or(ReleaseOffer{});
  EXPECT_EQ(found.versionText, NOTES_VERSION);
  EXPECT_EQ(found.notesMarkdown, TAG_NOTES);
}

/**
 * Given a feed offering nothing newer
 * When the check has run
 * Then nothing is found
 */
TEST(UpdateCheck, FindsNothingWhenNoReleaseIsNewer) {
  FakeReleaseUpdater updater;
  const UpdateCheck check{updater};

  ASSERT_TRUE(
      heldWithinTheWait([&updater] { return updater.checkCount() == 1; }));
  EXPECT_FALSE(check.found());
}

/**
 * Given a feed that cannot be trusted, which makes the check throw
 * When the check has run
 * Then nothing is found, and the client goes on
 */
TEST(UpdateCheck, FindsNothingWhenTheCheckFails) {
  FakeReleaseUpdater updater;
  updater.offer(ReleaseOffer{.versionText = std::string{NOTES_VERSION},
                             .notesMarkdown = {}});
  updater.failCheck();
  const UpdateCheck check{updater};

  ASSERT_TRUE(
      heldWithinTheWait([&updater] { return updater.checkCount() == 1; }));
  EXPECT_FALSE(check.found());
}
