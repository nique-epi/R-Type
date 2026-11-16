#pragma once

#include <gtest/gtest.h>
#include <httplib.h>
#include <string>
#include "RouteCall.hpp"
#include "RunningMaster.hpp"

/** @brief The master, started on a free port with wallets that open with 1000
 * kopecks, the account ACE, and the token of a session of ACE. */
class WalletRoutesFixture : public ::testing::Test {
 protected:
  WalletRoutesFixture();

  /** @returns The token of the session of ACE. */
  [[nodiscard]] const std::string& token() const;

  /** @returns The answer of the master to @p call. */
  [[nodiscard]] httplib::Result send(const RouteCall& call) const;

 private:
  RunningMaster master_;
  std::string token_;
};
