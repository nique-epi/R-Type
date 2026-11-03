#include "RateLimitFixtures.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "AllowanceSpending.hpp"
#include "AllowanceVerdict.hpp"
#include "Endpoint.hpp"
#include "RateLimit.hpp"
#include "RateLimitTestConstants.hpp"
#include "RequestAllowances.hpp"
#include "SenderAllowance.hpp"

namespace rtype::network::testing {

std::vector<AllowanceVerdict> spendTimes(SenderAllowance& allowance,
                                         std::size_t count,
                                         std::chrono::nanoseconds now) {
  std::vector<AllowanceVerdict> verdicts;
  verdicts.reserve(count);
  for (std::size_t sent = 0; sent < count; ++sent) {
    verdicts.push_back(spendAllowance(allowance, TEST_RATE_LIMIT, now));
  }
  return verdicts;
}

std::vector<AllowanceVerdict> spendEveryInterval(SenderAllowance& allowance,
                                                 std::size_t count) {
  std::vector<AllowanceVerdict> verdicts;
  verdicts.reserve(count);
  for (std::size_t sent = 0; sent < count; ++sent) {
    const std::chrono::nanoseconds now =
        START +
        (TEST_INTERVAL * static_cast<std::chrono::milliseconds::rep>(sent));
    verdicts.push_back(spendAllowance(allowance, TEST_RATE_LIMIT, now));
  }
  return verdicts;
}

SenderAllowance burstSpentAtStart() {
  SenderAllowance allowance;
  static_cast<void>(spendTimes(allowance, TEST_BURST, START));
  return allowance;
}

SenderAllowance bannedAtStart() { return bannedAtStartUnder(TEST_RATE_LIMIT); }

SenderAllowance bannedAtStartUnder(const RateLimit& limit) {
  SenderAllowance allowance;
  for (std::size_t sent = 0; sent < limit.burst + limit.dropsBeforeBan;
       ++sent) {
    static_cast<void>(spendAllowance(allowance, limit, START));
  }
  return allowance;
}

std::vector<AllowanceVerdict> spendBurstUnder(SenderAllowance& allowance,
                                              const RateLimit& limit,
                                              std::chrono::nanoseconds now) {
  std::vector<AllowanceVerdict> verdicts;
  verdicts.reserve(limit.burst);
  for (std::size_t sent = 0; sent < limit.burst; ++sent) {
    verdicts.push_back(spendAllowance(allowance, limit, now));
  }
  return verdicts;
}

Endpoint limitedSender(std::uint16_t port) {
  return Endpoint{.address = std::string{LIMITED_SENDER_ADDRESS}, .port = port};
}

Endpoint forgedSender(std::size_t number) {
  return Endpoint{
      .address = std::string{FORGED_ADDRESS},
      .port = static_cast<std::uint16_t>(FIRST_FORGED_PORT + number)};
}

std::vector<AllowanceVerdict> spendFromSender(RequestAllowances& allowances,
                                              const Endpoint& sender,
                                              std::size_t count) {
  std::vector<AllowanceVerdict> verdicts;
  verdicts.reserve(count);
  for (std::size_t sent = 0; sent < count; ++sent) {
    verdicts.push_back(allowances.spend(sender, START));
  }
  return verdicts;
}

void spendOnceFromEachForged(RequestAllowances& allowances, std::size_t count,
                             std::chrono::nanoseconds now) {
  for (std::size_t number = 0; number < count; ++number) {
    static_cast<void>(allowances.spend(forgedSender(number), now));
  }
}

}  // namespace rtype::network::testing
