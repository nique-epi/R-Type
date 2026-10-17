#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "BarState.hpp"
#include "Button.hpp"
#include "IInterfaceCanvas.hpp"
#include "Label.hpp"
#include "PlayfieldMapping.hpp"
#include "TabStrip.hpp"
#include "TextField.hpp"
#include "TextStyle.hpp"
#include "TimeConstants.hpp"
#include "WidgetConstants.hpp"
#include "WidgetStyles.hpp"

namespace rtype::client {

/**
 * @brief The widgets of one menu, already placed, and which of its buttons is
 * selected or pressed: the logic every menu shares.
 *
 * One button is selected at a time, the first enabled one at first. The next
 * and previous keys of the content (MENU_NEXT_KEYS and MENU_PREVIOUS_KEYS
 * unless it says otherwise) and MENU_TAB_KEY move the selection,
 * wrapping around and skipping disabled buttons; the pointer selects the
 * enabled button it moves over, unless the content selects on a click only, as
 * a form does. MENU_CONFIRM_KEY, or the left mouse button
 * over a button, presses it; on release (over the same button for the mouse)
 * it is activated once it has looked pressed for PRESS_DURATION. A release
 * counts only when the menu saw the press, so the release of a key pressed on
 * another screen does nothing here. MENU_CANCEL_KEY, released after its press,
 * selects the cancel button and activates it at once. While a button is
 * pressed, every other input waits; losing the focus drops a press not yet
 * released.
 *
 * A button that holds a TextField takes the keyboard while it is selected and
 * no button is pressed: the field gets the characters typed and the presses of
 * FIELD_ERASE_KEY, every other key keeps its meaning, MENU_CONFIRM_KEY
 * included. The left mouse button selects such a button without pressing it, so
 * a click on a field never runs its action.
 *
 * A row shows a RowControl on the right of its label. A key other than
 * MENU_CONFIRM_KEY and MENU_CANCEL_KEY goes first to the control of the
 * selected row; a key it uses does nothing else. A
 * left press over a control goes to it first; a press it does not use presses
 * the row when the row can be activated. While the control of the selected
 * row holds the input, every key, pointer and focus event goes to it and to
 * nothing else. The tabs of a tabbed panel open on a click, or on
 * PREVIOUS_TAB_KEY and NEXT_TAB_KEY, read by their place on the keyboard; a
 * tabbed panel holds no text field.
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
    /**
     * @brief Whether the pointer selects the bar it moves over, as on a page
     * or a dialog; a form selects on a click only, so the field the player
     * types into never changes under a passing pointer.
     */
    bool selectsOnHover = true;
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
    /** @brief Where the description of the selected button is written, and
     * how: outlined on the sky of a page, in small print inside a form. */
    sf::Vector2i descriptionAnchor;
    TextStyle descriptionStyle = PAGE_LINE_STYLE;
    /** @brief Where the status line is written: under the description of a
     * form, nowhere on a page or a dialog. */
    std::optional<sf::Vector2i> statusAnchor;
    /** @brief The area a screen paints its own picture in. */
    std::optional<sf::IntRect> illustration;
    /** @brief The keys that select the next and the previous button, besides
     * MENU_TAB_KEY. A tabbed panel leaves Left and Right out, for the values
     * of its rows: a key a control leaves unused and the menu does not list
     * does nothing. */
    std::span<const sf::Keyboard::Key> nextKeys = MENU_NEXT_KEYS;
    std::span<const sf::Keyboard::Key> previousKeys = MENU_PREVIOUS_KEYS;
    /** @brief The tabs over the panel of a tabbed panel. */
    std::optional<TabStrip> tabs;
    /** @brief Opens a tab by its order, when a click on the tabs,
     * PREVIOUS_TAB_KEY or NEXT_TAB_KEY asks for one. */
    std::function<void(std::size_t)> openTab;
    /** @brief How the status line is written: in gold under a form; small,
     * outlined, at the end of the rule of a tabbed panel. */
    TextStyle statusStyle = STATUS_STYLE;
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
   * @brief Paints the menu: the dimmed layer, the panel, the tabs, the rules,
   * the picture, the labels, the buttons, the description of the selected
   * button, the status line, then what the controls lay over the rest.
   */
  void paint(IInterfaceCanvas& canvas) const;

  /** @brief Sets the line written under the description of a form, such as
   * how a connection stands; empty for none. A page or a dialog writes none. */
  void setStatus(std::string text);

  /** @returns The text of the field of a button, by its order in the menu;
   * empty for a button without a field, or an order outside the menu. */
  [[nodiscard]] std::string_view fieldText(std::size_t button) const;

  /**
   * @brief Enables or disables a button, by its order in the menu; disabling
   * the selected button selects the next enabled one, and drops its press. An
   * order outside the menu changes nothing.
   */
  void setEnabled(std::size_t button, bool enabled);

  /** @brief Gives a button the line written while it is selected, by its
   * order in the menu; an order outside the menu changes nothing. */
  void setDescription(std::size_t button, std::string description);

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
  /** @brief Hands an event to the control of a row that holds the input. */
  static void routeToControl(Button& row, const sf::Event& event,
                             const PlayfieldMapping& playfieldPointAt);

  /** @returns Whether the control of the selected row used a key. */
  bool offerToSelectedControl(const sf::Event::KeyPressed& key);

  /** @brief Asks for a tab, by its order. */
  void openTab(std::size_t tab) const;

  /** @returns The order of the selected row while its control holds the
   * input, nothing otherwise, nor while a button is pressed. */
  [[nodiscard]] std::optional<std::size_t> rowHoldingInput() const;

  [[nodiscard]] BarState stateOf(std::size_t button) const;

  /** @returns The field of the selected button, or nullptr when it holds none
   * or no button is selected. */
  [[nodiscard]] TextField* selectedField();

  Content content_;
  std::optional<std::size_t> selected_;
  std::optional<Press> press_;
  bool cancelPressed_ = false;
  std::string status_;
};

}  // namespace rtype::client
