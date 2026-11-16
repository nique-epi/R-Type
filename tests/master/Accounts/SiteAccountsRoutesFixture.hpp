#pragma once

#include <gtest/gtest.h>
#include <httplib.h>
#include <string_view>
#include "RunningMaster.hpp"
#include "SiteClaim.hpp"

/** @brief A master holding TEST_SITE_KEY, called as the web site calls it:
 * over HTTPS, with the site headers of a claim. */
class SiteAccountsRoutesFixture : public ::testing::Test {
 protected:
  SiteAccountsRoutesFixture();

  /** @returns The answer to the creation of the account @p nickname, with
   * @p claim. */
  [[nodiscard]] httplib::Result createAccount(
      const rtype::master::SiteClaim& claim, std::string_view nickname) const;

  /** @brief Creates, with @p claim, every account one address may create in
   * an hour, each under a nickname of its own. */
  void createEveryAccountOfAnHour(const rtype::master::SiteClaim& claim) const;

  /** @returns The answer to a sign-in as ACE with @p password, with
   * @p claim. */
  [[nodiscard]] httplib::Result signInAsAce(
      const rtype::master::SiteClaim& claim, std::string_view password) const;

  /** @brief Fails, with @p claim, every sign-in as ACE one address may fail.
   */
  void failEverySignIn(const rtype::master::SiteClaim& claim) const;

 private:
  RunningMaster master_;
};
