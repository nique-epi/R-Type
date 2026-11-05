#include "RateLimitFixtures.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "AllowanceSpending.hpp"
#include "AllowanceVerdict.hpp"
#include "Endpoint.hpp"
#include "HandshakeTestConstants.hpp"
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

SenderAllowance bannedAtStart(const RateLimit& limit) {
  SenderAllowance allowance;
  for (std::size_t sent = 0; sent < limit.burst + limit.dropsBeforeBan;
       ++sent) {
    static_cast<void>(spendAllowance(allowance, limit, START));
  }
  return allowance;
}

Endpoint limitedSender(std::uint16_t port) {
  return Endpoint{.address = std::string{SENDER_ADDRESS}, .port = port};
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

void banAt(RequestAllowances& allowances, const Endpoint& sender,
           std::chrono::nanoseconds now) {
  for (std::size_t sent = 0; sent < TEST_BURST + TEST_DROPS_BEFORE_BAN;
       ++sent) {
    static_cast<void>(allowances.spend(sender, now));
  }
}

std::chrono::nanoseconds fillARowWithStaggeredBans(
    RequestAllowances& allowances) {
  for (const std::size_t number :
       {ROW_SENDER_BANNED_THIRD, ROW_SENDER_BANNED_FIRST,
        ROW_SENDER_BANNED_LAST, ROW_SENDER_BANNED_SECOND}) {
    static_cast<void>(allowances.spend(forgedSender(number), START));
  }
  std::chrono::nanoseconds now = START;
  for (const std::size_t number :
       {ROW_SENDER_BANNED_FIRST, ROW_SENDER_BANNED_SECOND,
        ROW_SENDER_BANNED_THIRD, ROW_SENDER_BANNED_LAST}) {
    now += TIME_BETWEEN_BANS;
    banAt(allowances, forgedSender(number), now);
  }
  now += TIME_BETWEEN_BANS;
  static_cast<void>(
      allowances.spend(forgedSender(ROW_SENDER_BANNED_FIRST), now));
  now += TIME_BETWEEN_BANS;
  static_cast<void>(
      allowances.spend(forgedSender(ROW_SENDER_ARRIVING_LAST), now));
  return now;
}

void spendOnceFromEachForged(RequestAllowances& allowances, std::size_t count,
                             std::chrono::nanoseconds now) {
  for (std::size_t number = 0; number < count; ++number) {
    static_cast<void>(allowances.spend(forgedSender(number), now));
  }
}

}  // namespace rtype::network::testing
