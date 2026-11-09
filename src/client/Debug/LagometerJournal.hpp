#pragma once

#include <cstdint>
#include <optional>
#include "LagometerJournalFigures.hpp"
#include "LagometerReading.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief What the lagometer writes in the journal, from what it reads at each
 * frame, shown or not: once each LAGOMETER_JOURNAL_PERIOD, the round trip,
 * the datagrams missed, the frames, the frames drawn past the newest state
 * and the range of the delay of the display over the period, so a game can
 * be read again once it is over.
 *
 * A period in which no game drew gives nothing: the title and the waiting
 * room have no display to measure, and the traffic already has its line.
 */
class LagometerJournal {
 public:
  /** @brief Takes what one frame read.
   * @returns The figures of the period this frame ends, when a game drew in
   * it; nothing otherwise. */
  [[nodiscard]] std::optional<LagometerJournalFigures> update(
      const LagometerReading& reading);

 private:
  /** @brief Counts the frame of @p reading and the delay it drew at. */
  void count(const LagometerReading& reading);

  /** @returns The figures of the period @p reading ends, when a game drew
   * in it. */
  [[nodiscard]] std::optional<LagometerJournalFigures> figuresOf(
      const LagometerReading& reading) const;

  /** @brief Starts a period at @p reading, which counts nothing yet. */
  void startPeriod(const LagometerReading& reading);

  bool started_{false};
  LagometerReading periodStart_;
  engine::Duration sincePeriod_{};
  std::uint64_t frames_{0};
  std::optional<double> smallestDelayTicks_;
  std::optional<double> largestDelayTicks_;
};

}  // namespace rtype::client
