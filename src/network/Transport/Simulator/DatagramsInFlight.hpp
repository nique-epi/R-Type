#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <random>
#include <span>
#include <vector>
#include "DatagramInFlight.hpp"
#include "Endpoint.hpp"
#include "NetworkConditions.hpp"

namespace rtype::network {

/**
 * @brief The datagrams crossing one direction of a simulated network, and the
 * draws that decide the fate of each.
 *
 * A datagram is lost first or not at all: a lost one is never copied. One that
 * is not lost may be copied, and each of its arrivals waits half the lag, then
 * up to half the jitter more, drawn at random but never arriving before the
 * arrival in order before it. Each arrival may then be held REORDER_HOLD more,
 * which the next ones do not wait for, so they arrive first.
 *
 * It reads no clock: each call gives the time. The draws come from a generator
 * started at a seed, so the same datagrams handed over at the same times meet
 * the same fate with the same standard library.
 */
class DatagramsInFlight {
 public:
  /**
   * @param conditions What the ping shows: each datagram that crosses here
   * waits half their lag and up to half their jitter.
   * @param seed The start of the draws.
   */
  DatagramsInFlight(const NetworkConditions& conditions, std::uint32_t seed);

  /**
   * @brief Draws the fate of a datagram handed over at @p now for @p peer, and
   * keeps a copy of @p payload for each time it arrives: none, once or twice.
   */
  void add(const Endpoint& peer, std::span<const std::byte> payload,
           std::chrono::steady_clock::time_point now);

  /** @returns When the next datagram arrives; nothing when none is in
   * flight. */
  [[nodiscard]] std::optional<std::chrono::steady_clock::time_point>
  nextArrival() const;

  /**
   * @brief Moves every datagram that arrived at @p now or before to the end of
   * @p destination, in the order they arrive; those that arrive at the same
   * time keep the order they were added in.
   */
  void takeArrived(std::chrono::steady_clock::time_point now,
                   std::vector<DatagramInFlight>& destination);

 private:
  /** @returns How many times the next datagram arrives: 0 when it is lost,
   * COPIED_DATAGRAM_ARRIVALS when it is copied, 1 otherwise. */
  [[nodiscard]] int arrivalCount();

  /** @returns When an arrival of a datagram handed over at @p now happens. */
  [[nodiscard]] std::chrono::steady_clock::time_point arrivalOf(
      std::chrono::steady_clock::time_point now);

  /** @returns Whether a draw of one in a hundred falls below @p percent. */
  [[nodiscard]] bool drawn(int percent);

  /** @returns A wait drawn from zero to half the jitter. */
  [[nodiscard]] std::chrono::steady_clock::duration jitterDrawn();

  /** @brief Inserts @p datagram after every datagram that arrives at the same
   * time or before it. */
  void keep(DatagramInFlight datagram);

  std::chrono::steady_clock::duration lagPerCrossing_;
  std::chrono::steady_clock::duration jitterPerCrossing_;
  NetworkConditions conditions_;
  std::mt19937 generator_;
  std::chrono::steady_clock::time_point latestInOrder_;
  std::vector<DatagramInFlight> inFlight_;
};

}  // namespace rtype::network
