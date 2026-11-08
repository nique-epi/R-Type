#pragma once

#include <chrono>
#include <vector>
#include "Ban.hpp"
#include "BanCandidate.hpp"
#include "BanTarget.hpp"

namespace rtype::server {

/**
 * @brief Bans, one per target: who they bar at a given moment. A ban past its
 * end bars nobody. Not thread-safe: a thread reads the copy LatestBans gives.
 */
class BanList {
 public:
  explicit BanList(std::vector<Ban> bans = {});

  /** @returns Whether a ban that has not ended at the moment of @p candidate
   * names its nickname or its address. */
  [[nodiscard]] bool bars(const BanCandidate& candidate) const;

  /** @brief Adds @p ban, in place of the ban of the same target when there is
   * one. */
  void put(Ban ban);

  /** @returns Whether a ban of @p target was there; it is gone. */
  bool remove(const BanTarget& target);

  [[nodiscard]] const std::vector<Ban>& bans() const;

  /** @returns The bans that have not ended at @p moment, in the order of
   * bans(). */
  [[nodiscard]] std::vector<Ban> inForceAt(
      std::chrono::sys_seconds moment) const;

 private:
  std::vector<Ban> bans_;
};

}  // namespace rtype::server
