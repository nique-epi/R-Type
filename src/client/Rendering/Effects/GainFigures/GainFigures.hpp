#pragma once

#include <cstdint>
#include "EffectTools.hpp"
#include "GainAttribution.hpp"
#include "GainFigure.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Shows the figure of each gain of score where a Bydo died, once a
 * kill and a gain wait for each other (see GainAttribution): a line of text
 * in the Popups layer that rises, stays, blinks and goes, or stays shown
 * instead of blinking while flashes are reduced (see Popup). noteKill() and
 * noteScore() add components, so neither may be called during a forEach of
 * the registry.
 */
class GainFigures {
 public:
  explicit GainFigures(EffectTools tools);

  /** @brief Notes a Bydo destroyed there, a big catch or not, and shows the
   * figure it makes with a gain already waiting. */
  void noteKill(game::Position place, bool big);

  /** @brief Notes the score a world state gives the pilot of a color, and
   * shows the figure a grown score makes with a kill already waiting. */
  void noteScore(int pilotColor, std::uint32_t score);

  /** @brief Ages the kills and the gains waiting for each other. */
  void advance(engine::Duration elapsed);

 private:
  /** @brief Shows the figure of a gain once a kill and a gain wait for each
   * other. */
  void showGain();

  /** @brief Adds the entity that shows a figure. */
  void addGainFigure(const GainFigure& figure) const;

  EffectTools tools_;
  GainAttribution attribution_;
};

}  // namespace rtype::client
