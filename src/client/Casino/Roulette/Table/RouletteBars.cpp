#include "RouletteBars.hpp"
#include <SFML/Window/Event.hpp>
#include "CasinoBarChip.hpp"
#include "CasinoChipWiring.hpp"
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "KeyChip.hpp"
#include "Language.hpp"
#include "PlayfieldMapping.hpp"
#include "RouletteBarActions.hpp"
#include "RouletteBarStates.hpp"
#include "RouletteScreenConstants.hpp"

namespace rtype::client {

RouletteBars::RouletteBars(const RouletteBarActions& actions,
                           ISoundPlayer& sounds, Language language)
    : clearChip_(chipOf(CLEAR_BAR, CasinoChipWiring{.action = actions.clear,
                                                    .sounds = sounds,
                                                    .language = language})),
      rebetChip_(chipOf(REBET_BAR, CasinoChipWiring{.action = actions.rebet,
                                                    .sounds = sounds,
                                                    .language = language})),
      backChip_(
          chipOf(TABLE_BACK_BAR, CasinoChipWiring{.action = actions.back,
                                                  .sounds = sounds,
                                                  .language = language})) {}

bool RouletteBars::handleEvent(const sf::Event& event,
                               const PlayfieldMapping& playfieldPointAt) {
  return clearChip_.handleEvent(event, playfieldPointAt) ||
         rebetChip_.handleEvent(event, playfieldPointAt) ||
         backChip_.handleEvent(event, playfieldPointAt);
}

void RouletteBars::show(const RouletteBarStates& states) {
  clearChip_.setEnabled(states.clearOpen);
  rebetChip_.setEnabled(states.rebetOpen);
}

void RouletteBars::paint(IInterfaceCanvas& canvas) const {
  clearChip_.paint(canvas);
  rebetChip_.paint(canvas);
  backChip_.paint(canvas);
}

}  // namespace rtype::client
