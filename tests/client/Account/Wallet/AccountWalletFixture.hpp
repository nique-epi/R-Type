#pragma once

#include <gtest/gtest.h>
#include <string>
#include "AccountWallet.hpp"
#include "FakeAccountSession.hpp"
#include "RecordingAccountLink.hpp"

namespace rtype::client::testing {

/** @brief The wallet of an account session a test signs in at will, over a
 * link that answers what the test sets. */
class AccountWalletFixture : public ::testing::Test {
 protected:
  AccountWalletFixture();

  [[nodiscard]] RecordingAccountLink& link();
  [[nodiscard]] FakeAccountSession& session();
  [[nodiscard]] AccountWallet& wallet();

  /** @brief Signs ACE in, and lets the link read its balances and the crown
   * with @p token. */
  void readAceWalletWith(const std::string& token);

 private:
  RecordingAccountLink link_;
  FakeAccountSession session_;
  AccountWallet wallet_;
};

}  // namespace rtype::client::testing
