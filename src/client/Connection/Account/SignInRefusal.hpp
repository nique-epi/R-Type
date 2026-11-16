#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Why a token did not sign the player in: the master knows no
 * session of it, or the session expired; the master refused for another
 * reason, or answered what the client cannot read; no master answered. */
enum class SignInRefusal : std::uint8_t {
  TokenRefused,
  MasterRefused,
  MasterUnreachable,
};

}  // namespace rtype::client
