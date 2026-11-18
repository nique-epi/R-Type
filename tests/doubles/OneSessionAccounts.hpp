#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include "ISessionAccounts.hpp"
#include "User.hpp"

/**
 * @brief Accounts that know one session: its token names one account, at
 * any date; every other token names none. Keeps the date it was last asked
 * at, so a test sees which date a route read the session at.
 */
class OneSessionAccounts final : public rtype::master::ISessionAccounts {
 public:
  OneSessionAccounts(std::string token, rtype::network::User user);

  [[nodiscard]] std::optional<rtype::network::User> userOfToken(
      std::string_view token, std::chrono::sys_seconds now) override;

  /** @returns The date of the last read, nothing before the first one. */
  [[nodiscard]] std::optional<std::chrono::sys_seconds> lastAskedAt() const;

 private:
  std::string token_;
  rtype::network::User user_;
  std::optional<std::chrono::sys_seconds> lastAskedAt_;
};
