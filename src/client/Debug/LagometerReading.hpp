#pragma once

#include <cstdint>
#include <optional>
#include "RoundTripFigures.hpp"
#include "RoundTripSample.hpp"
#include "SessionArrivals.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief What the lagometer reads at each frame. */
struct LagometerReading {
  /** @brief How long the frame took. */
  engine::Duration frame{};
  /** @brief The smoothed round trip and the jitter of the player. */
  std::optional<network::RoundTripFigures> figures = std::nullopt;
  /** @brief The newest round trip of the player, as measured. */
  std::optional<RoundTripSample> newestRoundTrip = std::nullopt;
  /** @brief The datagrams of the server accepted and missed so far. */
  SessionArrivals arrivals;
  /** @brief The bytes the socket received and sent so far. */
  std::uint64_t bytesReceived{0};
  std::uint64_t bytesSent{0};
  /** @brief The frames the game drew past the newest state so far, and how
   * far behind the server the newest frame drew, in ticks. */
  std::uint64_t pastTheNewest{0};
  std::optional<double> delayTicks = std::nullopt;
};

}  // namespace rtype::client
