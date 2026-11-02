#include "DatagramsInFlight.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>
#include <random>
#include <span>
#include <utility>
#include <vector>
#include "DatagramInFlight.hpp"
#include "Endpoint.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConditionsConstants.hpp"
#include "NetworkException.hpp"
#include "SimulatorConstants.hpp"

namespace rtype::network {

namespace {

/** @returns The share of @p delay one crossing of the simulated network
 * takes. */
std::chrono::steady_clock::duration perCrossing(
    std::chrono::milliseconds delay) {
  return std::chrono::duration_cast<std::chrono::steady_clock::duration>(
             delay) /
         CROSSINGS_PER_ROUND_TRIP;
}

/**
 * @returns Whether @p delay is from 0 to MAXIMUM_SIMULATED_DELAY, so half of
 * it converts to the clock of the simulator without overflow.
 */
bool isSimulatedDelay(std::chrono::milliseconds delay) {
  return delay >= std::chrono::milliseconds::zero() &&
         delay <= MAXIMUM_SIMULATED_DELAY;
}

/**
 * @returns A copy of @p conditions, once their lag and jitter are known to be
 * from 0 to MAXIMUM_SIMULATED_DELAY.
 * @throws SimulatedDelayOutOfRangeException when either is not.
 */
NetworkConditions checkedConditions(const NetworkConditions& conditions) {
  if (!isSimulatedDelay(conditions.lag) ||
      !isSimulatedDelay(conditions.jitter)) {
    throw SimulatedDelayOutOfRangeException(conditions.lag, conditions.jitter,
                                            MAXIMUM_SIMULATED_DELAY);
  }
  return conditions;
}

}  // namespace

DatagramsInFlight::DatagramsInFlight(const NetworkConditions& conditions,
                                     std::uint32_t seed)
    : conditions_(checkedConditions(conditions)), generator_(seed) {}

void DatagramsInFlight::add(const Endpoint& peer,
                            std::span<const std::byte> payload,
                            std::chrono::steady_clock::time_point now) {
  const int arrivals = arrivalCount();
  for (int arrival = 0; arrival < arrivals; ++arrival) {
    keep(DatagramInFlight{.arrival = arrivalOf(now),
                          .peer = peer,
                          .payload = {payload.begin(), payload.end()}});
  }
}

std::optional<std::chrono::steady_clock::time_point>
DatagramsInFlight::nextArrival() const {
  if (inFlight_.empty()) {
    return std::nullopt;
  }
  return inFlight_.front().arrival;
}

std::vector<DatagramInFlight> DatagramsInFlight::takeArrived(
    std::chrono::steady_clock::time_point now) {
  const auto firstLater =
      std::ranges::upper_bound(inFlight_, now, {}, &DatagramInFlight::arrival);
  std::vector<DatagramInFlight> arrived{
      std::make_move_iterator(inFlight_.begin()),
      std::make_move_iterator(firstLater)};
  inFlight_.erase(inFlight_.begin(), firstLater);
  return arrived;
}

int DatagramsInFlight::arrivalCount() {
  if (drawn(conditions_.lossPercent)) {
    return 0;
  }
  return drawn(conditions_.duplicatePercent) ? COPIED_DATAGRAM_ARRIVALS : 1;
}

std::chrono::steady_clock::time_point DatagramsInFlight::arrivalOf(
    std::chrono::steady_clock::time_point now) {
  latestInOrder_ = std::max(now + perCrossing(conditions_.lag) + jitterDrawn(),
                            latestInOrder_);
  if (drawn(conditions_.reorderPercent)) {
    return latestInOrder_ + REORDER_HOLD;
  }
  return latestInOrder_;
}

bool DatagramsInFlight::drawn(int percent) {
  std::uniform_int_distribution<int> hundredth{0, MAXIMUM_PERCENT - 1};
  return hundredth(generator_) < percent;
}

std::chrono::steady_clock::duration DatagramsInFlight::jitterDrawn() {
  std::uniform_int_distribution<std::chrono::steady_clock::duration::rep> extra{
      0, perCrossing(conditions_.jitter).count()};
  return std::chrono::steady_clock::duration{extra(generator_)};
}

void DatagramsInFlight::keep(DatagramInFlight datagram) {
  const auto later = std::ranges::upper_bound(inFlight_, datagram.arrival, {},
                                              &DatagramInFlight::arrival);
  inFlight_.insert(later, std::move(datagram));
}

}  // namespace rtype::network
