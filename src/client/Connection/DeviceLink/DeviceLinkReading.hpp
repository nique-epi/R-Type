#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include "DeviceLinkPhase.hpp"

namespace rtype::client {

/**
 * @brief Where the sign-in through the web site stands, as the thread of a
 * MasterDeviceLinkFlow publishes it: the ask it answers, the phase, the short
 * code and the pages a player validates it on, its expiry, and once approved
 * the token of the session. It never holds the secret of the link.
 */
struct DeviceLinkReading {
  /** @brief The number of the ask it answers (see DeviceLinkAsks). */
  std::uint64_t ask{};
  DeviceLinkPhase phase{DeviceLinkPhase::Requesting};
  std::string userCode;
  std::string verificationUri;
  std::string verificationUriComplete;
  std::chrono::sys_seconds expiresAt;
  /** @brief The token of the session, once SignedIn; kept in memory only. */
  std::string token;
};

}  // namespace rtype::client
