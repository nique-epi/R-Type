#include "RouletteBars.hpp"
#include <SFML/Window/Event.hpp>
#include "CasinoBarChip.hpp"
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "KeyChip.hpp"
#include "PlayfieldMapping.hpp"
#include "RouletteBarActions.hpp"
#include "RouletteBarStates.hpp"
#include "RouletteScreenConstants.hpp"

namespace rtype::client {

RouletteBars::RouletteBars(const RouletteBarActions& actions,
                           ISoundPlayer& sounds)
    : spinChip_(chipOf(SPIN_BAR, actions.spin, sounds)),
      spinAskedChip_(chipOf(SPIN_ASKED_BAR, actions.spin, sounds)),
      clearChip_(chipOf(CLEAR_BAR, actions.clear, sounds)),
      rebetChip_(chipOf(REBET_BAR, actions.rebet, sounds)),
      backChip_(chipOf(TABLE_BACK_BAR, actions.back, sounds)) {}

bool RouletteBars::handleEvent(const sf::Event& event,
                               const PlayfieldMapping& playfieldPointAt) {
  KeyChip& spin = spinAsked_ ? spinAskedChip_ : spinChip_;
  return spin.handleEvent(event, playfieldPointAt) ||
         clearChip_.handleEvent(event, playfieldPointAt) ||
         rebetChip_.handleEvent(event, playfieldPointAt) ||
         backChip_.handleEvent(event, playfieldPointAt);
}

void RouletteBars::show(const RouletteBarStates& states) {
  spinAsked_ = states.spinAsked;
  spinChip_.setEnabled(states.spinOpen);
  spinAskedChip_.setEnabled(states.spinOpen);
  clearChip_.setEnabled(states.clearOpen);
  rebetChip_.setEnabled(states.rebetOpen);
}

void RouletteBars::paint(IInterfaceCanvas& canvas) const {
  (spinAsked_ ? spinAskedChip_ : spinChip_).paint(canvas);
  clearChip_.paint(canvas);
  rebetChip_.paint(canvas);
  backChip_.paint(canvas);
}

}  // namespace rtype::client
