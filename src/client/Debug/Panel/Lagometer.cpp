#include "Lagometer.hpp"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <deque>
#include <optional>
#include "LagometerConstants.hpp"
#include "LagometerFigures.hpp"
#include "LagometerPoint.hpp"
#include "LagometerRates.hpp"
#include "LagometerReading.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

void Lagometer::update(const LagometerReading& reading) {
  if (!started_) {
    started_ = true;
    arrivalsAtNewestPoint_ = reading.arrivals;
    periodStart_ = reading;
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
  figures_.bytesReceivedPerSecond = perSecond(
      reading.bytesReceived - periodStart_.bytesReceived, sincePeriod_);
  figures_.bytesSentPerSecond =
      perSecond(reading.bytesSent - periodStart_.bytesSent, sincePeriod_);
  figures_.displayDelay = displayDelayOf(reading.delayTicks);
  figures_.pastTheNewestPerSecond =
      figures_.displayDelay.has_value()
          ? std::optional<std::uint64_t>{perSecond(
                reading.pastTheNewest - periodStart_.pastTheNewest,
                sincePeriod_)}
          : std::nullopt;
  ++revision_;
  sincePeriod_ = engine::Duration::zero();
  framesInPeriod_ = 0;
  periodStart_ = reading;
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
