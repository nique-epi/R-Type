#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "Button.hpp"
#include "IInterfaceCanvas.hpp"
#include "Label.hpp"
#include "PlayfieldMapping.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The widgets of one menu, already placed, and which of its buttons is
 * selected or pressed: the logic every menu shares.
 *
 * One button is selected at a time, the first enabled one at first. The keys
 * of MENU_NEXT_KEYS, MENU_PREVIOUS_KEYS and MENU_TAB_KEY move the selection,
 * wrapping around and skipping disabled buttons; the pointer selects the
 * enabled button it moves over. MENU_CONFIRM_KEY, or the left mouse button
 * over a button, presses it; on release (over the same button for the mouse)
 * it is activated once it has looked pressed for PRESS_DURATION. A release
 * counts only when the menu saw the press, so the release of a key pressed on
 * another screen does nothing here. MENU_CANCEL_KEY, released after its press,
 * selects the cancel button and activates it at once. While a button is
 * pressed, every other input waits; losing the focus drops a press not yet
 * released.
 *
 * An action must not destroy the menu: screens ask for screen changes through
 * their IScreenNavigator, which applies them after the event or the update.
 */
class Menu {
 public:
  /** @brief An image of a menu, such as the logo: the texture it shows whole,
   * and where. */
  struct Picture {
    std::string assetId;
    sf::IntRect area;
  };

  /**
   * @brief Everything a menu shows, already placed in pixels of the interface
   * picture: built by MenuBuilder, never by a screen.
   */
  struct Content {
    /** @brief Whether the screens below are dimmed first. */
    bool dimmed = false;
    /** @brief The panel of a dialog. */
    std::optional<sf::IntRect> panel;
    /** @brief The logo of a page. */
    std::optional<Picture> picture;
    /** @brief The rule under the header of a page. */
    std::optional<sf::IntRect> rule;
    std::vector<Label> labels;
    /** @brief The buttons, in the order they are selected. */
    std::vector<Button> buttons;
    /** @brief The button MENU_CANCEL_KEY activates. */
    std::optional<std::size_t> cancelButton;
    /** @brief Where the description of the selected button is written. */
    sf::Vector2i descriptionAnchor;
    /** @brief The area a screen paints its own picture in. */
    std::optional<sf::IntRect> illustration;
  };

  /** @brief Selects the first enabled button, if any. */
  explicit Menu(Content content);

  /**
   * @param playfieldPointAt Gives the point of the playfield under a pixel of
   * the window; the menu reads it in pixels of the interface picture.
   */
  void handleEvent(const sf::Event& event,
                   const PlayfieldMapping& playfieldPointAt);

  /** @brief Advances a press by the time of a frame, and activates its button
   * once it is due. */
  void advance(engine::Duration elapsed);

  /**
   * @brief Paints the menu: the dimmed layer, the panel, the rules, the
   * picture, the labels, the buttons, then the description of the selected
   * button.
   */
  void paint(IInterfaceCanvas& canvas) const;

  /**
   * @brief Enables or disables a button, by its order in the menu; disabling
   * the selected button selects the next enabled one, and drops its press. An
   * order outside the menu changes nothing.
   */
  void setEnabled(std::size_t button, bool enabled);

  /** @brief Gives a button its check box, by its order in the menu; an order
   * outside the menu changes nothing. */
  void setMark(std::size_t button, Button::CheckMark mark);

  /** @returns The order of the selected button, or nothing when no button is
   * enabled. */
  [[nodiscard]] std::optional<std::size_t> selectedButton() const;

  /** @returns The area reserved by MenuBuilder::illustration(), where a
   * screen paints a picture of its own. */
  [[nodiscard]] std::optional<sf::IntRect> illustrationArea() const;

 private:
  enum class PressSource : std::uint8_t {
    Keyboard,
    Pointer,
  };

  enum class Direction : std::uint8_t {
    Next,
    Previous,
  };

  /** @brief A button held down: by what, for how long, and whether it was
   * released. */
  struct Press {
    std::size_t button;
    PressSource source;
    engine::Duration held{};
    bool released = false;
  };

  void keyPressed(const sf::Event::KeyPressed& key);
  void keyReleased(sf::Keyboard::Key key);
  void pointerMoved(sf::Vector2i pixel);
  void pointerPressed(sf::Vector2i pixel);
  void pointerReleased(sf::Vector2i pixel);
  void focusLost();
  void activateIfDue();
  void activateCancel();
  void selectNeighbour(Direction direction);
  [[nodiscard]] std::optional<std::size_t> enabledButtonAt(
      sf::Vector2i pixel) const;
  [[nodiscard]] Button::State stateOf(std::size_t button) const;

  Content content_;
  std::optional<std::size_t> selected_;
  std::optional<Press> press_;
  bool cancelPressed_ = false;
};

}  // namespace rtype::client
