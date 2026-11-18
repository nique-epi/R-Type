#pragma once

#include <chrono>
#include <string>

namespace rtype::network {

/**
 * @brief What the master answers when a game asked for a device link: the
 * secret only the game keeps, which it never shows again, the short code a
 * player reads, the page of the web site that validates it, with and without
 * the code, its expiry and the seconds the game waits between two reads.
 */
struct CreatedDeviceLink {
  /** @brief DEVICE_LINK_SECRET_PREFIX then lowercase hexadecimal,
   * DEVICE_LINK_SECRET_SIZE long, sent as `Authorization: Bearer`. */
  std::string secret;
  std::string userCode;
  std::string verificationUri;
  std::string verificationUriComplete;
  std::chrono::sys_seconds expiresAt;
  std::chrono::seconds interval{};

  bool operator==(const CreatedDeviceLink&) const = default;
};

}  // namespace rtype::network
