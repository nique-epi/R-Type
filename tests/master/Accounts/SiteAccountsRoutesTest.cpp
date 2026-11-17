#include <gtest/gtest.h>
#include <httplib.h>
#include <string>
#include "AccountsTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "SiteAccountsRoutesFixture.hpp"
#include "SiteClaim.hpp"
#include "SiteClaimTestConstants.hpp"

using rtype::master::SiteClaim;

/**
 * Given a master holding the site key, and a visitor of the site who created
 * the five accounts of an hour
 * When the site creates a sixth one for that visitor
 * Then it gets 429: the quota applies to the visitor the site names
 */
TEST_F(SiteAccountsRoutesFixture, ASixthAccountForOneVisitorIsTooManyRequests) {
  const SiteClaim visitor{.key = std::string{TEST_SITE_KEY},
                          .clientAddress = std::string{FIRST_VISITOR}};
  createEveryAccountOfAnHour(visitor);

  const httplib::Result refused = createAccount(visitor, ACE_NICKNAME);

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_TOO_MANY_REQUESTS);
}

/**
 * Given a master holding the site key, and a visitor of the site who created
 * the five accounts of an hour
 * When the site creates one for another visitor, from the same address
 * Then it is created: each visitor has a quota of its own
 */
TEST_F(SiteAccountsRoutesFixture, AnotherVisitorOfTheSiteStillCreates) {
  createEveryAccountOfAnHour(
      SiteClaim{.key = std::string{TEST_SITE_KEY},
                .clientAddress = std::string{FIRST_VISITOR}});

  const httplib::Result created =
      createAccount(SiteClaim{.key = std::string{TEST_SITE_KEY},
                              .clientAddress = std::string{SECOND_VISITOR}},
                    ACE_NICKNAME);

  ASSERT_TRUE(created);
  EXPECT_EQ(created->status, EXPECTED_CREATED);
}

/**
 * Given a master holding the site key, and five accounts created with another
 * key and the address of a visitor
 * When a sixth one names another visitor with that wrong key
 * Then it gets 429: all six are counted under the address they came from
 */
TEST_F(SiteAccountsRoutesFixture, AWrongKeyCountsUnderTheAddressOfTheCaller) {
  createEveryAccountOfAnHour(
      SiteClaim{.key = std::string{WRONG_SITE_KEY},
                .clientAddress = std::string{FIRST_VISITOR}});

  const httplib::Result refused =
      createAccount(SiteClaim{.key = std::string{WRONG_SITE_KEY},
                              .clientAddress = std::string{SECOND_VISITOR}},
                    ACE_NICKNAME);

  ASSERT_TRUE(refused);
  EXPECT_EQ(refused->status, EXPECTED_TOO_MANY_REQUESTS);
}

/**
 * Given a master holding the site key, the account ACE, and a visitor of the
 * site whose sign-ins as ACE failed as often as the master allows
 * When the site signs ACE in for another visitor, from the same address
 * Then the session opens: each visitor has a count of failures of its own
 */
TEST_F(SiteAccountsRoutesFixture, AnotherVisitorStillSignsIn) {
  const SiteClaim first{.key = std::string{TEST_SITE_KEY},
                        .clientAddress = std::string{FIRST_VISITOR}};
  ASSERT_TRUE(createAccount(first, ACE_NICKNAME));
  failEverySignIn(first);

  const httplib::Result opened =
      signInAsAce(SiteClaim{.key = std::string{TEST_SITE_KEY},
                            .clientAddress = std::string{SECOND_VISITOR}},
                  ACE_PASSWORD);

  ASSERT_TRUE(opened);
  EXPECT_EQ(opened->status, EXPECTED_CREATED);
}
