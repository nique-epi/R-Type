#pragma once

#include <SFML/Window/Keyboard.hpp>
#include "ActionSet.hpp"

namespace rtype::client {

/**
 * @brief The actions the player holds, fed by keyboard and focus events.
 *
 * While the window has no focus, key events are ignored. Losing focus also
 * releases everything: the release of a key held at that moment goes to
 * another window, so it would otherwise stay held for good. A key already
 * held when the focus comes back counts only once it is pressed again.
 */
class PlayerCommands {
 public:
  /** @param focused Whether the window has the focus at construction. */
  explicit PlayerCommands(bool focused);

  void keyPressed(sf::Keyboard::Key key);
  void keyReleased(sf::Keyboard::Key key);
  void focusLost();
  void focusGained();

  [[nodiscard]] ActionSet held() const;

 private:
  ActionSet held_;
  bool focused_;
};

}  // namespace rtype::client
