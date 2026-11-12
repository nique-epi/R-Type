#include "LagometerJournal.hpp"
#include <algorithm>
#include <chrono>
#include <optional>
#include "ClockRecalibrations.hpp"
#include "LagometerJournalConstants.hpp"
#include "LagometerJournalFigures.hpp"
#include "LagometerRates.hpp"
#include "LagometerReading.hpp"
#include "SessionArrivals.hpp"

namespace rtype::client {

std::optional<LagometerJournalFigures> LagometerJournal::update(
    const LagometerReading& reading) {
  if (!started_) {
    started_ = true;
    startPeriod(reading);
  }
  count(reading);
  if (sincePeriod_ < LAGOMETER_JOURNAL_PERIOD) {
    return std::nullopt;
  }
  std::optional<LagometerJournalFigures> figures = figuresOf(reading);
  startPeriod(reading);
  return figures;
}

void LagometerJournal::count(const LagometerReading& reading) {
  sincePeriod_ += reading.frame;
  ++frames_;
  longestFrame_ = std::max(longestFrame_, reading.frame);
  if (reading.frame > LAGOMETER_SLOW_FRAME) {
    ++slowFrames_;
  }
  if (!reading.delayTicks.has_value()) {
    return;
  }
  smallestDelayTicks_ = std::min(
      smallestDelayTicks_.value_or(*reading.delayTicks), *reading.delayTicks);
  largestDelayTicks_ = std::max(
      largestDelayTicks_.value_or(*reading.delayTicks), *reading.delayTicks);
}

std::optional<LagometerJournalFigures> LagometerJournal::figuresOf(
    const LagometerReading& reading) const {
  if (!smallestDelayTicks_.has_value() || !largestDelayTicks_.has_value()) {
    return std::nullopt;
  }
  const SessionArrivals arrivals{
      .accepted = reading.arrivals.accepted - periodStart_.arrivals.accepted,
      .missed = reading.arrivals.missed - periodStart_.arrivals.missed};
  return LagometerJournalFigures{
      .period = sincePeriod_,
      .roundTrip = reading.figures,
      .arrivals = arrivals,
      .frames = frames_,
      .pastTheNewest = reading.pastTheNewest - periodStart_.pastTheNewest,
      .smallestDelay = displayDelayOf(smallestDelayTicks_)
                           .value_or(std::chrono::microseconds::zero()),
      .largestDelay = displayDelayOf(largestDelayTicks_)
                          .value_or(std::chrono::microseconds::zero()),
      .recalibrations =
          ClockRecalibrations{.count = reading.recalibrations.count -
                                       periodStart_.recalibrations.count,
                              .ticks = reading.recalibrations.ticks -
                                       periodStart_.recalibrations.ticks},
      .longestFrame = longestFrame_,
      .slowFrames = slowFrames_};
}

void LagometerJournal::startPeriod(const LagometerReading& reading) {
  periodStart_ = reading;
  sincePeriod_ = engine::Duration::zero();
  frames_ = 0;
  longestFrame_ = engine::Duration::zero();
  slowFrames_ = 0;
  smallestDelayTicks_.reset();
  largestDelayTicks_.reset();
}

}  // namespace rtype::client
