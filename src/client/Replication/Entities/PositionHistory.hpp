#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include "Position.hpp"
#include "PositionHistoryConstants.hpp"
#include "PositionSample.hpp"

namespace rtype::client {

/**
 * @brief The newest positions the server announced for one entity, so that the
 * client can show it where it was a little earlier instead of where the last
 * state put it.
 *
 * Samples are ordered by tick, oldest first. A component of the entity: it is
 * destroyed with it.
 */
class PositionHistory {
 public:
  /**
   * @brief Keeps @p sample as the newest one. Nothing for a sample whose tick
   * is not newer than the newest kept: over UDP a state can arrive after a
   * newer one, and a part of a state of the same tick lists other entities.
   * The oldest sample is dropped once HISTORY_SAMPLES are kept.
   */
  void record(PositionSample sample);

  /**
   * @returns Where the entity is at @p tick, a tick that may fall between two
   * server ticks: nothing without a sample; the oldest sample for a tick
   * before it; the linear interpolation between the two samples around the
   * tick; after the newest sample, the position moved on at the speed of the
   * last two samples for at most MAXIMUM_EXTRAPOLATION_TICKS (the newest
   * position when only one sample is kept).
   */
  [[nodiscard]] std::optional<game::Position> at(double tick) const;

  /** @returns The tick of the oldest sample kept; nothing without a sample. */
  [[nodiscard]] std::optional<std::uint32_t> oldestTick() const;

 private:
  std::array<PositionSample, HISTORY_SAMPLES> samples_{};
  std::size_t count_{0};
};

}  // namespace rtype::client
