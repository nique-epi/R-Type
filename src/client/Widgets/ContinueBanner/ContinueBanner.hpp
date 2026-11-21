#pragma once

#include <cstdint>
#include <optional>
#include "TimeConstants.hpp"

namespace rtype::client {

class IInterfaceCanvas;

/**
 * @brief The banner of a continue in a game that plays a level: shown
 * CONTINUE_BANNER_TIME once the server tells fewer continues left than it
 * told before, so once the team, without a life, restarted from a
 * checkpoint. The first count told shows nothing: it is where the game
 * stands, not a continue taken. A continue taken while it shows starts its
 * time over.
 */
class ContinueBanner {
 public:
  /** @brief Takes the continues a LevelStatus tells are left. */
  void follow(std::uint8_t continuesLeft);

  /** @brief Adds @p elapsed to the time the banner has shown. */
  void advance(engine::Duration elapsed);

  /** @returns The continues left the banner tells; nothing while it does not
   * show. */
  [[nodiscard]] std::optional<std::uint8_t> shownContinues() const;

  /** @brief Paints the banner centered in MISSION_BANNER_AREA while it
   * shows: CONTINUE, then the continues left under it, or NO CONTINUE LEFT in
   * the color of a warning, both outlined in ink, between two rules. */
  void paint(IInterfaceCanvas& canvas) const;

 private:
  /** The continues the server told last; nothing before its first status. */
  std::optional<std::uint8_t> told_;
  /** How long the banner has shown; nothing while it does not show. */
  std::optional<engine::Duration> shownFor_;
};

}  // namespace rtype::client
