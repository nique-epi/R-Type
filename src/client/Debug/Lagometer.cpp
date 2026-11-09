#include "Lagometer.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <deque>
#include <optional>
#include "LagometerConstants.hpp"
#include "LagometerFigures.hpp"
#include "LagometerPoint.hpp"
#include "LagometerReading.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns @p count over @p period, per second, rounded to the nearest. */
std::uint64_t perSecond(std::uint64_t count, engine::Duration period) {
  const auto ticks = static_cast<std::uint64_t>(period.count());
  const auto ticksPerSecond = static_cast<std::uint64_t>(
      std::chrono::duration_cast<engine::Duration>(std::chrono::seconds{1})
          .count());
  return ((count * ticksPerSecond) + (ticks / 2)) / ticks;
}

/** @returns @p delayTicks of the server as a duration; nothing without it. */
std::optional<std::chrono::microseconds> displayDelayOf(
    std::optional<double> delayTicks) {
  if (!delayTicks.has_value()) {
    return std::nullopt;
  }
  return std::chrono::round<std::chrono::microseconds>(
      std::chrono::duration<double>(
          *delayTicks /
          static_cast<double>(engine::SIMULATION_TICKS_PER_SECOND)));
}

}  // namespace

void Lagometer::update(const LagometerReading& reading) {
  if (!started_) {
    started_ = true;
    arrivalsAtNewestPoint_ = reading.arrivals;
    bytesReceivedAtPeriod_ = reading.bytesReceived;
    bytesSentAtPeriod_ = reading.bytesSent;
    pastTheNewestAtPeriod_ = reading.interpolation.pastTheNewest;
  }
  if (figures_.roundTrip != reading.figures) {
    figures_.roundTrip = reading.figures;
    ++revision_;
  }
  addPoint(reading);
  countFrame(reading);
}

const LagometerFigures& Lagometer::figures() const { return figures_; }

const std::deque<LagometerPoint>& Lagometer::points() const { return points_; }

std::chrono::milliseconds Lagometer::scaleTop() const { return scaleTop_; }

std::uint64_t Lagometer::revision() const { return revision_; }

bool Lagometer::readsTrafficAfter(engine::Duration frame) const {
  return !started_ || sincePeriod_ + frame >= LAGOMETER_FIGURES_PERIOD;
}

std::chrono::milliseconds Lagometer::topOverThePoints() const {
  std::chrono::microseconds highest{0};
  for (const LagometerPoint& point : points_) {
    highest = std::max(highest, point.roundTrip);
  }
  std::chrono::milliseconds top = LOWEST_LAGOMETER_SCALE_TOP;
  while (top < highest) {
    top *= 2;
  }
  return top;
}

void Lagometer::addPoint(const LagometerReading& reading) {
  if (!reading.newestRoundTrip.has_value() ||
      reading.newestRoundTrip->number == newestNumber_) {
    return;
  }
  newestNumber_ = reading.newestRoundTrip->number;
  points_.push_back(LagometerPoint{
      .roundTrip = reading.newestRoundTrip->roundTrip,
      .accepted = reading.arrivals.accepted - arrivalsAtNewestPoint_.accepted,
      .missed = reading.arrivals.missed - arrivalsAtNewestPoint_.missed});
  arrivalsAtNewestPoint_ = reading.arrivals;
  if (points_.size() > LAGOMETER_POINT_COUNT) {
    points_.pop_front();
  }
  figures_.lossPercent = lossPercent();
  scaleTop_ = topOverThePoints();
  ++revision_;
}

void Lagometer::countFrame(const LagometerReading& reading) {
  sincePeriod_ += reading.frame;
  ++framesInPeriod_;
  if (sincePeriod_ < LAGOMETER_FIGURES_PERIOD) {
    return;
  }
  figures_.framesPerSecond = perSecond(framesInPeriod_, sincePeriod_);
  figures_.bytesReceivedPerSecond =
      perSecond(reading.bytesReceived - bytesReceivedAtPeriod_, sincePeriod_);
  figures_.bytesSentPerSecond =
      perSecond(reading.bytesSent - bytesSentAtPeriod_, sincePeriod_);
  figures_.pastTheNewestPerSecond =
      perSecond(reading.interpolation.pastTheNewest - pastTheNewestAtPeriod_,
                sincePeriod_);
  figures_.displayDelay = displayDelayOf(reading.delayTicks);
  ++revision_;
  sincePeriod_ = engine::Duration::zero();
  framesInPeriod_ = 0;
  bytesReceivedAtPeriod_ = reading.bytesReceived;
  bytesSentAtPeriod_ = reading.bytesSent;
  pastTheNewestAtPeriod_ = reading.interpolation.pastTheNewest;
}

std::uint64_t Lagometer::lossPercent() const {
  std::uint64_t accepted = 0;
  std::uint64_t missed = 0;
  for (const LagometerPoint& point : points_) {
    accepted += point.accepted;
    missed += point.missed;
  }
  const std::uint64_t expected = accepted + missed;
  if (expected == 0) {
    return 0;
  }
  return ((missed * PERCENT) + (expected / 2)) / expected;
}

}  // namespace rtype::client
