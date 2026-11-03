#pragma once

#include <SFML/Window/Keyboard.hpp>
#include "ActionSet.hpp"
#include "KeyBindings.hpp"

namespace rtype::client {

/**
 * @brief The actions the player holds, fed by keyboard and focus events, read
 * through the keys the player chose.
 *
 * While the window has no focus, key events are ignored. Losing focus also
 * releases everything: the release of a key held at that moment goes to
 * another window, so it would otherwise stay held for good. A key already
 * held when the focus comes back counts only once it is pressed again. A key
 * changed while it is held keeps its action held until it is pressed and
 * released again.
 */
class PlayerCommands {
 public:
  /**
   * @param focused Whether the window has the focus at construction.
   * @param bindings Keys of the controls, read at each event, which must
   * outlive the commands.
   */
  PlayerCommands(bool focused, const KeyBindings& bindings);

  void keyPressed(sf::Keyboard::Scancode key);
  void keyReleased(sf::Keyboard::Scancode key);
  void focusLost();
  void focusGained();

  /** @brief Releases every action held, as when another screen takes the
   * keyboard: a key rebound meanwhile would otherwise never release its
   * action. */
  void releaseAll();

  [[nodiscard]] ActionSet held() const;

 private:
  const KeyBindings* bindings_;
  ActionSet held_;
  bool focused_;
};

}  // namespace rtype::client
