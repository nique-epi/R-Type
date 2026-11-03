#pragma once

#include <SFML/Window/Keyboard.hpp>
#include "BarState.hpp"
#include "Button.hpp"
#include "IInterfaceCanvas.hpp"
#include "ISoundPlayer.hpp"
#include "KeyChipParts.hpp"
#include "PlayfieldMapping.hpp"

namespace sf {
class Event;
}  // namespace sf

namespace rtype::client {

/**
 * @brief A button with its key, standing alone on a screen that holds no menu:
 * it plays the click and runs its action when its key, or the left button over
 * it, is released after its press.
 *
 * As in a Menu, a release counts only when the chip saw the press: the release
 * of a key pressed on another screen does nothing, and a key held down acts
 * once however often the system repeats its press. A disabled chip runs
 * nothing.
 */
class KeyChip {
 public:
  explicit KeyChip(KeyChipParts parts);

  /** @returns Whether the event was the chip's: its key, or a press or release
   * of the left button it saw. Moving the pointer and losing the focus are told
   * to it, and left to the others. Losing the focus drops everything it knows
   * of the input, so a screen that stops giving it events gives it that one
   * first.
   */
  bool handleEvent(const sf::Event& event,
                   const PlayfieldMapping& playfieldPointAt);

  /** @brief Enables or disables the chip, which drops a press under way. */
  void setEnabled(bool enabled);

  void paint(IInterfaceCanvas& canvas) const;

 private:
  bool pointerEvent(const sf::Event& event,
                    const PlayfieldMapping& playfieldPointAt);
  void activate();
  [[nodiscard]] BarState state() const;

  Button button_;
  sf::Keyboard::Key key_;
  ISoundPlayer* sounds_;
  BarState restState_;
  bool keyHeld_ = false;
  bool pointerHeld_ = false;
  bool hovered_ = false;
};

}  // namespace rtype::client
