#pragma once

#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "KeyChip.hpp"
#include "PlayfieldMapping.hpp"
#include "RouletteBarActions.hpp"
#include "RouletteBarStates.hpp"

namespace sf {
class Event;
}  // namespace sf

namespace rtype::client {

/**
 * @brief The three bars of the band of the table: CLEAR, its key C, REBET, R,
 * and BACK, MENU_CANCEL_KEY. Each acts once per hold of its key, on its
 * release.
 */
class RouletteBars {
 public:
  /** @param sounds Plays the clicks; it must outlive the bars. */
  RouletteBars(const RouletteBarActions& actions, ISoundPlayer& sounds);

  /** @returns Whether one of the bars took @p event. */
  bool handleEvent(const sf::Event& event,
                   const PlayfieldMapping& playfieldPointAt);

  /** @brief Enables the bars @p states opens, disables the others. */
  void show(const RouletteBarStates& states);

  void paint(IInterfaceCanvas& canvas) const;

 private:
  KeyChip clearChip_;
  KeyChip rebetChip_;
  KeyChip backChip_;
};

}  // namespace rtype::client
