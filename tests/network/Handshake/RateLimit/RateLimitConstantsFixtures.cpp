#include "RateLimitConstantsFixtures.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <vector>
#include "AllowanceSpending.hpp"
#include "AllowanceVerdict.hpp"
#include "RateLimit.hpp"
#include "RateLimitConstantsTestConstants.hpp"
#include "SenderAllowance.hpp"

namespace rtype::network::testing {

std::vector<std::chrono::nanoseconds> joiningClientTimes(
    std::chrono::nanoseconds duration) {
  std::vector<std::chrono::nanoseconds> times;
  for (std::chrono::nanoseconds request{0}; request < duration;
       request += CLIENT_REQUEST_INTERVAL) {
    times.push_back(request);
    times.emplace_back(request + CHALLENGE_ROUND_TRIP);
  }
  return times;
}

std::vector<std::chrono::nanoseconds> playerTimes(
    std::chrono::nanoseconds duration) {
  const std::chrono::nanoseconds oneSecond{std::chrono::seconds{1}};
  const auto inputsPerSecond =
      static_cast<std::chrono::nanoseconds::rep>(INPUTS_PER_SECOND);
  std::vector<std::chrono::nanoseconds> times;
  for (std::chrono::nanoseconds second{0}; second < duration;
       second += oneSecond) {
    times.push_back(second);
    for (std::chrono::nanoseconds::rep input = 0; input < inputsPerSecond;
         ++input) {
      times.push_back(second + (oneSecond * input / inputsPerSecond));
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
