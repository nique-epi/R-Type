#include "RateLimitConstantsFixtures.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <vector>
#include "AllowanceSpending.hpp"
#include "AllowanceVerdict.hpp"
#include "ProtocolConstants.hpp"
#include "RateLimit.hpp"
#include "RateLimitConstantsTestConstants.hpp"
#include "SenderAllowance.hpp"
#include "TimeTestConstants.hpp"

namespace rtype::network::testing {

namespace {

/**
 * @returns When an attempt sends its requests for @p duration: one at its
 * start, then one every CONNECTION_REQUEST_INTERVAL.
 */
std::vector<std::chrono::nanoseconds> requestTimes(
    std::chrono::nanoseconds duration) {
  std::vector<std::chrono::nanoseconds> times;
  for (std::chrono::nanoseconds request{0}; request < duration;
       request += CONNECTION_REQUEST_INTERVAL) {
    times.push_back(request);
  }
  return times;
}

}  // namespace

std::vector<std::chrono::nanoseconds> joiningClientTimes(
    std::chrono::nanoseconds duration) {
  std::vector<std::chrono::nanoseconds> times;
  for (const std::chrono::nanoseconds request : requestTimes(duration)) {
    times.push_back(request);
    times.emplace_back(request + CHALLENGE_ROUND_TRIP);
  }
  return times;
}

std::vector<std::chrono::nanoseconds> answersAfterAStall() {
  return std::vector<std::chrono::nanoseconds>(
      requestTimes(CONNECTION_ATTEMPT_TIMEOUT).size());
}

std::vector<std::chrono::nanoseconds> playerTimes(
    std::chrono::nanoseconds duration) {
  const auto inputsPerSecond =
      static_cast<std::chrono::nanoseconds::rep>(PLAYER_INPUTS_PER_SECOND);
  std::vector<std::chrono::nanoseconds> times;
  for (std::chrono::nanoseconds second{0}; second < duration;
       second += engine::testing::ONE_SECOND) {
    times.push_back(second);
    for (std::chrono::nanoseconds::rep input = 0; input < inputsPerSecond;
         ++input) {
      times.push_back(second +
                      (engine::testing::ONE_SECOND * input / inputsPerSecond));
    }
  }
  return times;
}

std::vector<AllowanceVerdict> verdictsAt(
    const RateLimit& limit,
    const std::vector<std::chrono::nanoseconds>& times) {
  SenderAllowance allowance;
  std::vector<AllowanceVerdict> verdicts;
  verdicts.reserve(times.size());
  for (const std::chrono::nanoseconds now : times) {
    verdicts.push_back(spendAllowance(allowance, limit, now));
  }
  return verdicts;
}

std::size_t allowedCount(const std::vector<AllowanceVerdict>& verdicts) {
  return static_cast<std::size_t>(
      std::ranges::count(verdicts, AllowanceVerdict::Allowed));
}

}  // namespace rtype::network::testing
