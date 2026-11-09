#include "LagometerText.hpp"
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include "LagometerConstants.hpp"
#include "LagometerFigureText.hpp"
#include "LagometerFigures.hpp"
#include "RoundTripFigures.hpp"

namespace rtype::client {

namespace {

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

}  // namespace

std::array<std::string, LAGOMETER_TEXT_LINE_COUNT> lagometerTextLines(
    const LagometerFigures& figures) {
  return {pingLine(figures), lossLine(figures), trafficLine(figures),
          bufferLine(figures)};
}

}  // namespace rtype::client
