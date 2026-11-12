#include "LagometerJournalText.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include <sstream>
#include <string>
#include "LagometerFigureText.hpp"
#include "LagometerJournalConstants.hpp"
#include "LagometerJournalFigures.hpp"
#include "LagometerRates.hpp"

namespace rtype::client {

namespace {

/** @returns The part of the journal line on the round trip and the
 * datagrams of the server. */
std::string journalLinkText(const LagometerJournalFigures& figures) {
  std::optional<std::uint64_t> roundTrip;
  std::optional<std::uint64_t> jitter;
  if (figures.roundTrip.has_value()) {
    roundTrip = wholeMilliseconds(figures.roundTrip->roundTrip);
    jitter = wholeMilliseconds(figures.roundTrip->jitter);
  }
  const std::uint64_t expected =
      figures.arrivals.accepted + figures.arrivals.missed;
  std::ostringstream text;
  text << figureText(roundTrip) << JOURNAL_JITTER_WORD << figureText(jitter)
       << JOURNAL_MISSED_WORD << figures.arrivals.missed << JOURNAL_OF_WORD
       << expected << JOURNAL_DATAGRAMS_WORD;
  return text.str();
}

/** @returns The part of the journal line on the frames and the display. */
std::string journalDisplayText(const LagometerJournalFigures& figures) {
  std::ostringstream text;
  text << figures.frames << JOURNAL_FRAMES_WORD
       << perSecond(figures.frames, figures.period) << JOURNAL_FRAMES_END_WORD
       << figures.pastTheNewest << JOURNAL_PAST_THE_NEWEST_WORD
       << perSecond(figures.pastTheNewest, figures.period)
       << JOURNAL_DISPLAY_WORD
       << std::chrono::round<std::chrono::milliseconds>(figures.smallestDelay)
              .count()
       << JOURNAL_TO_WORD
       << std::chrono::round<std::chrono::milliseconds>(figures.largestDelay)
              .count()
       << JOURNAL_BEHIND_WORD;
  return text.str();
}

/** @returns The part of the journal line on the moves of the clock of the
 * other entities and on the pace of the frames. */
std::string journalPaceText(const LagometerJournalFigures& figures) {
  const std::chrono::microseconds moved =
      displayDelayOf(figures.recalibrations.ticks)
          .value_or(std::chrono::microseconds::zero());
  std::ostringstream text;
  text << JOURNAL_CLOCK_WORD << figures.recalibrations.count
       << JOURNAL_CLOCK_TIMES_WORD << wholeMilliseconds(moved)
       << JOURNAL_CLOCK_TOTAL_WORD
       << wholeMilliseconds(std::chrono::round<std::chrono::microseconds>(
              figures.longestFrame))
       << JOURNAL_SLOW_WORD << figures.slowFrames << JOURNAL_SLOW_OVER_WORD
       << wholeMilliseconds(LAGOMETER_SLOW_FRAME) << JOURNAL_SLOW_END_WORD;
  return text.str();
}

}  // namespace

std::string lagometerJournalLine(const LagometerJournalFigures& figures) {
  std::ostringstream line;
  line << JOURNAL_PERIOD_WORD
       << std::chrono::duration<double>(figures.period).count()
       << JOURNAL_PING_WORD << journalLinkText(figures)
       << journalDisplayText(figures) << journalPaceText(figures);
  return line.str();
}

}  // namespace rtype::client
