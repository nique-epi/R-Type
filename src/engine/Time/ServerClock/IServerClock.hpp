#pragma once

#include <cstdint>
#include <optional>
#include "TimeConstants.hpp"

namespace rtype::engine {

/**
 * @brief The clock of the server seen from the client: which tick the server
 * is at on a given instant of the clock of the client, and the other way
 * round. Both answers are empty until the client knows the clock of the
 * server.
 */
class IServerClock {
 public:
  virtual ~IServerClock() = default;

  /** @returns The instant, on the clock of the client, at which the server
   * was or will be at @p tick; empty before the first sample. */
  [[nodiscard]] virtual std::optional<Duration> localInstantOf(
      std::uint32_t tick) const = 0;

  /** @returns The tick the server is at on the instant @p localNow of the
   * clock of the client, with the part of a tick elapsed after the point;
   * negative before tick 0; empty before the first sample. */
  [[nodiscard]] virtual std::optional<double> tickAt(
      Duration localNow) const = 0;
};

}  // namespace rtype::engine
