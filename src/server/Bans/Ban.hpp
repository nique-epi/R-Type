#pragma once

#include <chrono>
#include <optional>
#include "BanTarget.hpp"

namespace rtype::server {

/** @brief One ban: who, since when, and until when; no end for a ban for
 * good. */
struct Ban {
  BanTarget target;
  std::chrono::sys_seconds bannedAt;
  std::optional<std::chrono::sys_seconds> expiresAt = std::nullopt;

  bool operator==(const Ban&) const = default;
};

}  // namespace rtype::server
