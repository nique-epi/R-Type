#pragma once

#include <string>

namespace rtype::network {

/**
 * @brief The nickname and the password a player sends to create an account or
 * to open a session; the master keeps only a hash of the password.
 */
struct UserCredentials {
  /** @brief A nickname of the protocol (isValidNickname()). */
  std::string nickname;
  /** @brief MINIMUM_PASSWORD_SIZE to MAXIMUM_PASSWORD_SIZE bytes of UTF-8. */
  std::string password;

  bool operator==(const UserCredentials&) const = default;
};

}  // namespace rtype::network
