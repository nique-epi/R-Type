#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <vector>
#include "EntitySpriteConstants.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Matches the kills of Bydos with the gains of score the world states
 * show, so a figure tells the gain where the Bydo died. The points come from
 * the server only: the client never computes a gain.
 *
 * A gain is a score that grew since the previous score of the same pilot; the
 * first score of a pilot is no gain. Kills and gains wait GAIN_MATCH_WINDOW
 * for each other, in either order, and are forgotten after. Once both wait,
 * they make one figure: the sum of the gains, at the place of the last kill,
 * in the color of the pilot when one pilot gained for one kill, in no color
 * otherwise, and large when one of the kills was a big catch. A kill whose
 * score never grows makes no figure: a gain is never invented.
 */
class GainAttribution {
 public:
  /** @brief The figure of a gain. */
  struct Figure {
    game::Position place{};
    std::uint32_t points{};
    /** @brief 0 to PLAYER_COLOR_COUNT - 1; nothing when several pilots
     * gained, or one gained for several kills. */
    std::optional<int> pilotColor;
    bool big = false;
  };

  void noteKill(game::Position place, bool big);

  /** @brief Notes the score of the pilot of a color; a color outside 0 to
   * PLAYER_COLOR_COUNT - 1 is ignored. */
  void noteScore(int pilotColor, std::uint32_t score);

  /** @brief Ages every kill and gain waiting, and forgets those that waited
   * GAIN_MATCH_WINDOW. */
  void advance(engine::Duration elapsed);

  /** @returns The figure the kills and the gains waiting make, which forgets
   * them; nothing while either is missing. */
  [[nodiscard]] std::optional<Figure> takeFigure();

 private:
  struct Kill {
    game::Position place{};
    bool big = false;
    engine::Duration age{};
  };

  struct Gain {
    int pilotColor{};
    std::uint32_t points{};
    engine::Duration age{};
  };

  [[nodiscard]] std::uint32_t totalPoints() const;

  /** @returns The color of the pilot who gained, when one pilot gained for
   * one kill. */
  [[nodiscard]] std::optional<int> soleGainer() const;

  std::array<std::optional<std::uint32_t>, PLAYER_COLOR_COUNT> scores_;
  std::vector<Kill> kills_;
  std::vector<Gain> gains_;
};

}  // namespace rtype::client
