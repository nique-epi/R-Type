#include "LagometerText.hpp"
#include <array>
#include <chrono>
#include <cstdint>
#include <optional>
#include <sstream>
#include <string>
#include "LagometerConstants.hpp"
#include "LagometerFigures.hpp"
#include "LagometerJournalFigures.hpp"
#include "LagometerRates.hpp"
#include "RoundTripFigures.hpp"

namespace rtype::client {

namespace {

/** @returns @p figure in decimal, or NO_FIGURE_TEXT without one. */
std::string figureText(std::optional<std::uint64_t> figure) {
  return figure.has_value() ? std::to_string(*figure)
                            : std::string{NO_FIGURE_TEXT};
}

/** @returns @p duration in whole milliseconds, rounded to the nearest. */
std::uint64_t wholeMilliseconds(std::chrono::microseconds duration) {
  return static_cast<std::uint64_t>(
      std::chrono::round<std::chrono::milliseconds>(duration).count());
}

/** @returns @p bytes in whole kilobytes, rounded to the nearest. */
std::optional<std::uint64_t> kilobytes(std::optional<std::uint64_t> bytes) {
  if (!bytes.has_value()) {
    return std::nullopt;
  }
  return (*bytes + (BYTES_PER_KILOBYTE / 2)) / BYTES_PER_KILOBYTE;
}

/** @returns The line of the round trip and the jitter. */
std::string pingLine(const LagometerFigures& figures) {
  std::optional<std::uint64_t> roundTrip;
  std::optional<std::uint64_t> jitter;
  if (figures.roundTrip.has_value()) {
    roundTrip = wholeMilliseconds(figures.roundTrip->roundTrip);
    jitter = wholeMilliseconds(figures.roundTrip->jitter);
  }
  return std::string{PING_WORD} + figureText(roundTrip) +
         std::string{MILLISECONDS_WORD} + std::string{JITTER_WORD} +
         figureText(jitter);
}

/** @returns The line of the losses and the frames per second. */
std::string lossLine(const LagometerFigures& figures) {
  return std::string{LOSS_WORD} + std::to_string(figures.lossPercent) +
         std::string{PERCENT_WORD} + std::string{FRAMES_WORD} +
         figureText(figures.framesPerSecond);
}

/** @returns The line of the traffic. */
std::string trafficLine(const LagometerFigures& figures) {
  return std::string{RECEIVED_WORD} +
         figureText(kilobytes(figures.bytesReceivedPerSecond)) +
         std::string{SENT_WORD} +
         figureText(kilobytes(figures.bytesSentPerSecond)) +
         std::string{KILOBYTES_WORD};
}

/** @returns The line of how far behind the server the other entities are
 * drawn, and of the frames per second drawn past the newest state. */
std::string bufferLine(const LagometerFigures& figures) {
  std::optional<std::uint64_t> delay;
  if (figures.displayDelay.has_value()) {
    delay = wholeMilliseconds(*figures.displayDelay);
  }
  return std::string{BUFFER_WORD} + figureText(delay) +
         std::string{MILLISECONDS_WORD} + std::string{LATE_WORD} +
         figureText(figures.pastTheNewestPerSecond);
}

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

std::array<std::string, LAGOMETER_TEXT_LINE_COUNT> lagometerTextLines(
    const LagometerFigures& figures) {
  return {pingLine(figures), lossLine(figures), trafficLine(figures),
          bufferLine(figures)};
}

}  // namespace rtype::client
