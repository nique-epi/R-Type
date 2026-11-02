#include "DatagramsInFlightFixtures.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <vector>
#include "DatagramInFlight.hpp"
#include "DatagramsInFlight.hpp"
#include "DatagramsInFlightTestConstants.hpp"
#include "SimulatorFixtures.hpp"

namespace {

/** @returns When datagram @p number is handed over, @p spacing apart from
 * HAND_OVER_START. */
std::chrono::steady_clock::time_point handOverOf(
    std::size_t number, std::chrono::milliseconds spacing) {
  return HAND_OVER_START + spacing * static_cast<std::int64_t>(number);
}

/** @brief Hands datagram @p number to @p inFlight, its payload holding the
 * number. */
void handNumber(rtype::network::DatagramsInFlight& inFlight, std::size_t number,
                std::chrono::milliseconds spacing) {
  const auto payload =
      std::bit_cast<std::array<std::byte, sizeof(std::uint32_t)>>(
          static_cast<std::uint32_t>(number));
  inFlight.add(peer(), payload, handOverOf(number, spacing));
}

}  // namespace

void handNumbered(rtype::network::DatagramsInFlight& inFlight,
                  std::size_t count, std::chrono::milliseconds spacing) {
  for (std::size_t number = 0; number < count; ++number) {
    handNumber(inFlight, number, spacing);
  }
}

std::vector<rtype::network::DatagramInFlight> arrivalsWhileHanding(
    rtype::network::DatagramsInFlight& inFlight, std::size_t count,
    std::chrono::milliseconds spacing) {
  std::vector<rtype::network::DatagramInFlight> arrivals;
  for (std::size_t number = 0; number < count; ++number) {
    handNumber(inFlight, number, spacing);
    std::ranges::move(inFlight.takeArrived(handOverOf(number, spacing)),
                      std::back_inserter(arrivals));
  }
  std::ranges::move(everyArrival(inFlight), std::back_inserter(arrivals));
  return arrivals;
}

std::vector<rtype::network::DatagramInFlight> everyArrival(
    rtype::network::DatagramsInFlight& inFlight) {
  return inFlight.takeArrived(std::chrono::steady_clock::time_point::max());
}

std::size_t numberOf(const rtype::network::DatagramInFlight& datagram) {
  std::array<std::byte, sizeof(std::uint32_t)> bytes{};
  std::ranges::copy(datagram.payload, bytes.begin());
  return std::bit_cast<std::uint32_t>(bytes);
}

std::chrono::steady_clock::duration waitOf(
    const rtype::network::DatagramInFlight& datagram,
    std::chrono::milliseconds spacing) {
  return datagram.arrival - handOverOf(numberOf(datagram), spacing);
}

std::vector<std::chrono::steady_clock::duration> waitsOf(
    const std::vector<rtype::network::DatagramInFlight>& arrivals,
    std::chrono::milliseconds spacing) {
  std::vector<std::chrono::steady_clock::duration> waits;
  waits.reserve(arrivals.size());
  for (const rtype::network::DatagramInFlight& datagram : arrivals) {
    waits.push_back(waitOf(datagram, spacing));
  }
  return waits;
}

std::vector<std::size_t> arrivalsByNumber(
    const std::vector<rtype::network::DatagramInFlight>& arrivals,
    std::size_t count) {
  std::vector<std::size_t> counts(count, 0);
  for (const rtype::network::DatagramInFlight& datagram : arrivals) {
    ++counts.at(numberOf(datagram));
  }
  return counts;
}
