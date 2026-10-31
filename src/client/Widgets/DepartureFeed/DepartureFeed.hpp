#pragma once

#include <cstdint>
#include <span>
#include <vector>
#include "DepartureLine.hpp"
#include "PilotStatus.hpp"
#include "PlayerPalette.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

class IInterfaceCanvas;

/**
 * @brief The feed of departures of a game, in the top-left corner of the
 * playfield (GameNoticeConstants.hpp): a line for each pilot the status band
 * shows gone, left or with its signal lost, the newest at the top. A pilot
 * gets one line per game, however many copies of its PlayerLeft arrive; past
 * DEPARTURE_FEED_LINES lines, the oldest goes; a line goes after
 * DEPARTURE_LINE_TIME. A line appears and goes at once: nothing moves.
 */
class DepartureFeed {
 public:
  /** @brief Adds a line at the top for each pilot of @p pilots that left or
   * lost its signal and had no line yet this game. */
  void follow(std::span<const PilotStatus> pilots);

  /** @brief Ages every line by @p elapsed, then drops each one shown
   * DEPARTURE_LINE_TIME. */
  void advance(engine::Duration elapsed);

  /** @returns The lines shown, the newest first. */
  [[nodiscard]] std::span<const DepartureLine> lines() const;

  /** @returns A count of the lines added and dropped: equal to the one a
   * reader painted, nothing to repaint. */
  [[nodiscard]] std::uint32_t revision() const;

  /** @brief Paints every line, each pilot in the shades @p palette gives its
   * color. */
  void paint(IInterfaceCanvas& canvas, const PlayerPalette& palette) const;

 private:
  /** @returns Whether @p pilot left or lost its signal, and had no line this
   * game. */
  [[nodiscard]] bool isNewDeparture(const PilotStatus& pilot) const;

  /** @brief Puts the line of @p pilot at the top, the oldest dropped past
   * DEPARTURE_FEED_LINES. */
  void announce(const PilotStatus& pilot);

  std::vector<std::uint8_t> announced_;
  std::vector<DepartureLine> lines_;
  std::uint32_t revision_{};
};

}  // namespace rtype::client
