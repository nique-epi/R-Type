#pragma once

#include <gtest/gtest.h>
#include <httplib.h>
#include <string_view>
#include "RouteCall.hpp"
#include "RunningMaster.hpp"

/** @brief The master, started on a free port, a database of its own and the
 * test key of the passwords, and a client of the routes of its accounts. */
class AccountsRoutesFixture : public ::testing::Test {
 protected:
  /** @returns The master the tests call. */
  [[nodiscard]] const RunningMaster& master() const;

  /** @returns The answer of the master to @p call. */
  [[nodiscard]] httplib::Result send(const RouteCall& call) const;

  /** @returns The answer to the creation of ACE, over HTTPS. */
  [[nodiscard]] httplib::Result createAce() const;

  /** @returns The answer to a sign-in as ACE with @p password, over HTTPS. */
  [[nodiscard]] httplib::Result signInAsAce(std::string_view password) const;

  /** @returns The answer to the account of the session of @p token. */
  [[nodiscard]] httplib::Result readOwnUser(std::string_view token) const;

  /** @returns The answer to the closing of the session of @p token. */
  [[nodiscard]] httplib::Result deleteSession(std::string_view token) const;

 private:
  RunningMaster master_;
};
