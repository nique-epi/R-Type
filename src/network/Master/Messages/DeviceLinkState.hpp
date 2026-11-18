#pragma once

#include <chrono>
#include <string>
#include "DeviceLinkStatus.hpp"

namespace rtype::network {

/**
 * @brief A device link as `GET /api/device-links/me` and
 * `GET /api/device-links/{code}` show it: its short code, where it stands,
 * when the game asked for it and when it expires, and the seconds the game
 * waits between two reads. It never holds the secret of the game.
 */
struct DeviceLinkState {
  /** @brief DEVICE_LINK_USER_CODE_SIZE characters, `XXXX-XXXX`. */
  std::string userCode;
  DeviceLinkStatus status{DeviceLinkStatus::Pending};
  std::chrono::sys_seconds createdAt;
  std::chrono::sys_seconds expiresAt;
  std::chrono::seconds interval{};

  bool operator==(const DeviceLinkState&) const = default;
};

}  // namespace rtype::network
