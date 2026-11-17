#include "SiteAccountsRoutesFixture.hpp"
#include <gtest/gtest.h>
#include <httplib.h>
#include <cstddef>
#include <string>
#include <string_view>
#include "AccountsRoutesFixtures.hpp"
#include "AccountsRoutesTestConstants.hpp"
#include "AccountsTestConstants.hpp"
#include "MasterTestConstants.hpp"
#include "RouteCall.hpp"
#include "RouteCallFixtures.hpp"
#include "SiteClaim.hpp"
#include "SiteCreationRoutesFixtures.hpp"
#include "UserCredentials.hpp"

SiteAccountsRoutesFixture::SiteAccountsRoutesFixture()
    : master_(siteKeyLines()) {}

httplib::Result SiteAccountsRoutesFixture::createAccount(
    const rtype::master::SiteClaim& claim, std::string_view nickname) const {
  RouteCall call = credentialsCall(
      USERS_URL,
      rtype::network::UserCredentials{.nickname = std::string{nickname},
                                      .password = std::string{ACE_PASSWORD}});
  call.site = claim;
  return sendRouteCall(master_.port(), call);
}

void SiteAccountsRoutesFixture::createEveryAccountOfAnHour(
    const rtype::master::SiteClaim& claim) const {
  for (std::size_t index = 0; index < ALLOWED_ACCOUNT_CREATIONS; ++index) {
    const httplib::Result created =
        createAccount(claim, std::string{ACE_NICKNAME} + std::to_string(index));
    ASSERT_TRUE(created);
    ASSERT_EQ(created->status, EXPECTED_CREATED);
  }
}

httplib::Result SiteAccountsRoutesFixture::signInAsAce(
    const rtype::master::SiteClaim& claim, std::string_view password) const {
  RouteCall call = credentialsCall(SESSIONS_URL, aceWith(password));
  call.site = claim;
  return sendRouteCall(master_.port(), call);
}

void SiteAccountsRoutesFixture::failEverySignIn(
    const rtype::master::SiteClaim& claim) const {
  for (std::size_t index = 0; index < ALLOWED_SIGN_IN_FAILURES; ++index) {
    const httplib::Result failed = signInAsAce(claim, WRONG_PASSWORD);
    ASSERT_TRUE(failed);
    ASSERT_EQ(failed->status, EXPECTED_UNAUTHORIZED);
  }
}
