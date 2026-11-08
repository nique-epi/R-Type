#include "LagometerText.hpp"
#include <array>
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include "LagometerConstants.hpp"
#include "LagometerFigures.hpp"
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

}  // namespace

std::array<std::string, LAGOMETER_TEXT_LINE_COUNT> lagometerTextLines(
    const LagometerFigures& figures) {
  return {pingLine(figures), lossLine(figures), trafficLine(figures)};
}

}  // namespace rtype::client
