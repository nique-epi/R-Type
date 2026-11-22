#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <optional>
#include "IInterfaceCanvas.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "Language.hpp"
#include "PlayfieldMapping.hpp"
#include "TabRail.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The tabs over the server map: SECTORS, the map itself, open; HANGAR,
 * SHOP and CASINO, which push HANGAR_SCREEN, SHOP_SCREEN and CASINO_SCREEN over
 * the map, so their BACK comes back to it.
 *
 * A tab opens on a click, or on its key: HANGAR_KEY, SHOP_KEY, CASINO_KEY.
 * Tab and the arrows already move the choice of the map, and Q is QUICK
 * PLAY, so the keys are letters, drawn nowhere. A key held down opens its tab
 * once; what the tabs hold of the input is dropped when they push a screen,
 * since the events then go to that screen.
 */
class ServerMapTabs {
 public:
  ServerMapTabs(IScreenNavigator& navigator, ISoundPlayer& sounds,
                Language language);

  void handleEvent(const sf::Event& event,
                   const PlayfieldMapping& playfieldPointAt);
  void update(engine::Duration elapsed);
  void paint(IInterfaceCanvas& canvas) const;

 private:
  /** @brief Opens the key's tab, once per hold. */
  void keyPressed(sf::Keyboard::Key key);
  /** @brief Pushes the screen of a tab, or shakes it when it is locked; the
   * open tab does nothing. */
  void choose(std::size_t tab);

  IScreenNavigator* navigator_;
  ISoundPlayer* sounds_;
  TabRail rail_;
  std::optional<sf::Keyboard::Key> heldKey_;
};

}  // namespace rtype::client
