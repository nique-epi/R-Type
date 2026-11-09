#include "LagometerRates.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include "TimeConstants.hpp"

namespace rtype::client {

std::uint64_t perSecond(std::uint64_t count, engine::Duration period) {
  const auto ticks = static_cast<std::uint64_t>(period.count());
  const auto ticksPerSecond = static_cast<std::uint64_t>(
      std::chrono::duration_cast<engine::Duration>(std::chrono::seconds{1})
          .count());
  return ((count * ticksPerSecond) + (ticks / 2)) / ticks;
}

std::optional<std::chrono::microseconds> displayDelayOf(
    std::optional<double> delayTicks) {
  if (!delayTicks.has_value()) {
    return std::nullopt;
  }
  return std::chrono::round<std::chrono::microseconds>(
      *delayTicks * std::chrono::duration<double, engine::Duration::period>(
                        engine::SIMULATION_TICK_DURATION));
}

}  // namespace rtype::client
