#pragma once

#include <cstdint>

namespace rtype::master {

/** @brief Why the master refused a request on a device link. */
enum class DeviceLinkRefusal : std::uint8_t {
  /** @brief The address asked for too many links in the last hour. */
  TooManyCreations,
  /** @brief No link has that secret or that short code, or it was forgotten.
   */
  Unknown,
  /** @brief The game read its link again before the interval passed. */
  TooSoon,
  /** @brief Another decision was already made on the link, or it expired. */
  AlreadyDecided,
  /** @brief The link waits for a decision: no session yet. */
  Pending,
  /** @brief The player refused to connect the game. */
  Denied,
  /** @brief The link outlived its lifetime before it was claimed. */
  Expired,
};

}  // namespace rtype::master
