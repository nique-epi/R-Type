#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <optional>
#include <string>
#include "AccountsService.hpp"
#include "MasterDatabaseFile.hpp"
#include "User.hpp"

/** @brief The accounts of the master on a database of their own, keyed with
 * TEST_PASSWORD_KEY, holding the account ACE. */
class AccountsServiceFixture : public ::testing::Test {
 protected:
  AccountsServiceFixture();

  /** @returns The token of a session of ACE opened at SESSION_OPENED_AT. */
  [[nodiscard]] std::string aceToken();

  /** @returns The account of the session of @p token at @p date. */
  [[nodiscard]] std::optional<rtype::network::User> accountAt(
      const std::string& token, std::chrono::sys_seconds date);

  /** @returns What the database keeps of the password of ACE, its bytes as
   * they are. */
  [[nodiscard]] std::string keptPassword() const;

 private:
  MasterDatabaseFile file_;
  rtype::master::AccountsService service_;
};
