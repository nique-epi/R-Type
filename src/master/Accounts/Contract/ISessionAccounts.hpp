#pragma once

#include <chrono>
#include <optional>
#include <string_view>
#include "User.hpp"

namespace rtype::master {

/**
 * @brief What the other modules of the master read of the accounts: the
 * account behind the token of a session. The module accounts provides it; it
 * is compiled without that module, so a module that reads it finds nothing
 * when the accounts are off.
 */
class ISessionAccounts {
 public:
  virtual ~ISessionAccounts() = default;

  /**
   * @returns The account the session of @p token names at @p now, or nothing
   * when the token is not well formed, unknown or expired; extends the
   * session as a read of `GET /api/users/me` does.
   */
  [[nodiscard]] virtual std::optional<network::User> userOfToken(
      std::string_view token, std::chrono::sys_seconds now) = 0;
};

}  // namespace rtype::master
