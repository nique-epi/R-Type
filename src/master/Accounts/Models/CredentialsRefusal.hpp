#pragma once

#include <cstdint>

namespace rtype::master {

/** @brief Why the master refused to create an account or to open a session. */
enum class CredentialsRefusal : std::uint8_t {
  /** @brief Another account has the nickname, compared without the case of
   * its ASCII letters. */
  NicknameTaken,
  /** @brief The address created too many accounts in the last hour. */
  TooManyCreations,
  /** @brief No account has the nickname, or its password is another one. */
  WrongCredentials,
  /** @brief The address failed too many sign-ins in the last minutes. */
  TooManyFailures,
};

}  // namespace rtype::master
