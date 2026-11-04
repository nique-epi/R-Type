#include "AllowanceSpending.hpp"
#include <algorithm>
#include <chrono>
#include <optional>
#include "AllowanceVerdict.hpp"
#include "RateLimit.hpp"
#include "SenderAllowance.hpp"

namespace rtype::network {

namespace {

/**
 * @returns How early a datagram may come before its theoretical arrival:
 * burst - 1 intervals.
 */
std::chrono::nanoseconds toleranceOf(const RateLimit& limit) {
  return limit.interval *
         static_cast<std::chrono::nanoseconds::rep>(limit.burst - 1);
}

bool conforms(const SenderAllowance& allowance, const RateLimit& limit,
              std::chrono::nanoseconds now) {
  const std::chrono::nanoseconds theoretical =
      allowance.theoreticalArrival.value_or(now);
  return now >= theoretical - toleranceOf(limit);
}

void accept(SenderAllowance& allowance, const RateLimit& limit,
            std::chrono::nanoseconds now) {
  allowance.theoreticalArrival =
      std::max(allowance.theoreticalArrival.value_or(now), now) +
      limit.interval;
}

void countDrop(SenderAllowance& allowance, const RateLimit& limit,
               std::chrono::nanoseconds now) {
  if (!allowance.dropWindowEnd.has_value() || now >= *allowance.dropWindowEnd) {
    allowance.dropWindowEnd = now + limit.dropWindow;
    allowance.dropsInWindow = 0;
  }
  ++allowance.dropsInWindow;
}

bool reachesBan(const SenderAllowance& allowance, const RateLimit& limit) {
  return allowance.dropsInWindow >= limit.dropsBeforeBan;
}

void ban(SenderAllowance& allowance, const RateLimit& limit,
         std::chrono::nanoseconds now) {
  allowance = SenderAllowance{.theoreticalArrival = std::nullopt,
                              .dropWindowEnd = std::nullopt,
                              .dropsInWindow = 0,
                              .bannedUntil = now + limit.banDuration};
}

}  // namespace

AllowanceVerdict spendAllowance(SenderAllowance& allowance,
                                const RateLimit& limit,
                                std::chrono::nanoseconds now) {
  if (isBannedAt(allowance, now)) {
    return AllowanceVerdict::Banned;
  }
  if (conforms(allowance, limit, now)) {
    accept(allowance, limit, now);
    return AllowanceVerdict::Allowed;
  }
  countDrop(allowance, limit, now);
  if (!reachesBan(allowance, limit)) {
    return AllowanceVerdict::OverCap;
  }
  ban(allowance, limit, now);
  return AllowanceVerdict::NewlyBanned;
}

bool isBannedAt(const SenderAllowance& allowance,
                std::chrono::nanoseconds now) {
  return allowance.bannedUntil.has_value() && now < *allowance.bannedUntil;
}

}  // namespace rtype::network
