#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include "RoundTripFigures.hpp"

namespace rtype::client {

/** @brief The figures the lines of the panel write. */
struct LagometerFigures {
  /** @brief The smoothed round trip and the jitter; nothing before the first
   * round trip. */
  std::optional<network::RoundTripFigures> roundTrip = std::nullopt;
  /** @brief The share of the datagrams of the server missed over the points of
   * the curve, in whole percent, rounded to the nearest. */
  std::uint64_t lossPercent{0};
  /** @brief The frames, and the bytes received and sent, in the last whole
   * period; nothing before the first period ends. */
  std::optional<std::uint64_t> framesPerSecond = std::nullopt;
  std::optional<std::uint64_t> bytesReceivedPerSecond = std::nullopt;
  std::optional<std::uint64_t> bytesSentPerSecond = std::nullopt;
  /** @brief How far behind the server the game drew the other entities at
   * the end of the last period, and how many frames per second of it drew
   * them past the newest state received; nothing while no game draws: before
   * the first one, and once one ended. */
  std::optional<std::chrono::microseconds> displayDelay = std::nullopt;
  std::optional<std::uint64_t> pastTheNewestPerSecond = std::nullopt;

  bool operator==(const LagometerFigures&) const = default;
};

}  // namespace rtype::client
