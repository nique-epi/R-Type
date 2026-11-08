#pragma once

#include <chrono>
#include <cstdint>
#include <deque>
#include "LagometerFigures.hpp"
#include "LagometerPoint.hpp"
#include "LagometerReading.hpp"
#include "SessionArrivals.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief What the lagometer shows, from what it reads at each frame: the
 * curve of the round trips, as measured, and the figures of its lines.
 *
 * A round trip whose number it has not seen yet becomes a point, with the
 * datagrams accepted and missed since the point before; past
 * LAGOMETER_POINT_COUNT points, the oldest goes. The frames and the bytes are
 * counted over each LAGOMETER_FIGURES_PERIOD, from the first frame read. The
 * totals of the first frame are where the counting starts, so what was
 * counted before the lagometer was built never shows.
 */
class Lagometer {
 public:
  /** @brief Takes what one frame read. */
  void update(const LagometerReading& reading);

  /** @returns The figures of the lines of the panel. */
  [[nodiscard]] const LagometerFigures& figures() const;

  /** @returns The points of the curve, the oldest first. */
  [[nodiscard]] const std::deque<LagometerPoint>& points() const;

  /** @returns The top of the scale of the curve: LOWEST_LAGOMETER_SCALE_TOP,
   * doubled until no point lies above it. */
  [[nodiscard]] std::chrono::milliseconds scaleTop() const;

 private:
  /** @brief Adds the round trip of @p reading as a point when it is new. */
  void addPoint(const LagometerReading& reading);

  /** @brief Counts the frame of @p reading, and writes the frames and the
   * bytes per second once a period passed. */
  void countFrame(const LagometerReading& reading);

  /** @returns The share of the datagrams missed over the points, in whole
   * percent, rounded to the nearest. */
  [[nodiscard]] std::uint64_t lossPercent() const;

  LagometerFigures figures_;
  std::deque<LagometerPoint> points_;
  bool started_{false};
  std::uint64_t newestNumber_{0};
  SessionArrivals arrivalsAtNewestPoint_;
  engine::Duration sincePeriod_{};
  std::uint64_t framesInPeriod_{0};
  std::uint64_t bytesReceivedAtPeriod_{0};
  std::uint64_t bytesSentAtPeriod_{0};
};

}  // namespace rtype::client
