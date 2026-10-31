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

}  // namespace

DatagramsInFlight::DatagramsInFlight(const NetworkConditions& conditions,
                                     std::uint32_t seed)
    : lagPerCrossing_(perCrossing(conditions.lag)),
      jitterPerCrossing_(perCrossing(conditions.jitter)),
      conditions_(conditions),
      generator_(seed) {}

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

void DatagramsInFlight::takeArrived(
    std::chrono::steady_clock::time_point now,
    std::vector<DatagramInFlight>& destination) {
  const auto firstLater =
      std::ranges::upper_bound(inFlight_, now, {}, &DatagramInFlight::arrival);
  std::ranges::move(inFlight_.begin(), firstLater,
                    std::back_inserter(destination));
  inFlight_.erase(inFlight_.begin(), firstLater);
}

int DatagramsInFlight::arrivalCount() {
  if (drawn(conditions_.lossPercent)) {
    return 0;
  }
  return drawn(conditions_.duplicatePercent) ? COPIED_DATAGRAM_ARRIVALS : 1;
}

std::chrono::steady_clock::time_point DatagramsInFlight::arrivalOf(
    std::chrono::steady_clock::time_point now) {
  latestInOrder_ =
      std::max(now + lagPerCrossing_ + jitterDrawn(), latestInOrder_);
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
      0, jitterPerCrossing_.count()};
  return std::chrono::steady_clock::duration{extra(generator_)};
}

void DatagramsInFlight::keep(DatagramInFlight datagram) {
  const auto later = std::ranges::upper_bound(inFlight_, datagram.arrival, {},
                                              &DatagramInFlight::arrival);
  inFlight_.insert(later, std::move(datagram));
}

}  // namespace rtype::network
