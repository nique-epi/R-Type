#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <functional>
#include <optional>
#include <string>
#include "BarState.hpp"
#include "IInterfaceCanvas.hpp"
#include "RowControl.hpp"
#include "ValueBinding.hpp"

namespace rtype::client {

/**
 * @brief The key of an action, drawn as a keycap, that the player changes by
 * pressing the new one.
 *
 * MENU_CONFIRM_KEY, or a click on the row, makes the slot wait for a key: it
 * then holds every event and writes KEY_LISTENING_TEXT in a lit box. The next
 * key pressed that the slot accepts becomes its key; MENU_CANCEL_KEY, a click
 * or the loss of the focus stops the wait and keeps the key; a key it does not
 * accept leaves the wait on, and lastKeyRefused() says so until the next key.
 * Keys are read by their place on the keyboard. Once a key
 * is taken, the slot holds the events until that key is released, so the
 * repeats of a held key never start a new wait. A key another action shares
 * is drawn with a gold edge, KEY_CONFLICT_FLAG before it.
 */
class KeySlot : public RowControl {
 public:
  /** @brief What a slot reads and asks: its key; the name drawn for a key;
   * whether a key may be taken; whether another action has the same key. */
  struct Wiring {
    ValueBinding<sf::Keyboard::Scancode> key;
    std::function<std::string(sf::Keyboard::Scancode)> name;
    std::function<bool(sf::Keyboard::Scancode)> accepts;
    std::function<bool()> conflicts;
  };

  explicit KeySlot(Wiring wiring);

  [[nodiscard]] int width() const override;
  void paint(IInterfaceCanvas& canvas, sf::IntRect area,
             BarState row) const override;
  [[nodiscard]] bool isActivatable() const override;
  void activate() override;
  [[nodiscard]] bool holdsInput() const override;
  bool keyPressed(const sf::Event::KeyPressed& key) override;
  void keyReleased(const sf::Event::KeyReleased& key) override;
  bool pointerPressed(sf::Vector2i pixel, sf::IntRect area) override;
  void cancelInput() override;

  /** @returns Whether the slot waits for a key. */
  [[nodiscard]] bool isListening() const;

  /** @returns Whether the last key pressed during the wait was refused, so the
   * player can be told why nothing happened. */
  [[nodiscard]] bool lastKeyRefused() const;

 private:
  Wiring wiring_;
  bool listening_ = false;
  bool lastKeyRefused_ = false;
  /** @brief The key just taken, until it is released. */
  std::optional<sf::Keyboard::Scancode> takenKeyHeld_;
};

}  // namespace rtype::client
