#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include "BarState.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/**
 * @brief The value a row of a menu shows and changes, on the right of its
 * label: a slider, a switch, a list...
 *
 * The Menu holding the row hands the control the input of its row: every
 * key but MENU_CONFIRM_KEY and MENU_CANCEL_KEY, first; MENU_CONFIRM_KEY,
 * through activate(), when isActivatable(); the left button pressed over the
 * control; and, while holdsInput(), every key, pointer and focus event. The
 * area
 * given to the methods is the control's own, width() wide, right-aligned in
 * its row, in pixels of the interface picture. Every method but width() and
 * paint() does nothing by default.
 */
class RowControl {
 public:
  RowControl() = default;
  virtual ~RowControl() = default;
  RowControl(const RowControl&) = delete;
  RowControl& operator=(const RowControl&) = delete;
  RowControl(RowControl&&) = delete;
  RowControl& operator=(RowControl&&) = delete;

  /** @returns The width of the control now, in pixels. */
  [[nodiscard]] virtual int width() const = 0;

  /** @brief Paints the control in its area, in the look of its row. */
  virtual void paint(IInterfaceCanvas& canvas, sf::IntRect area,
                     BarState row) const = 0;

  /** @brief Paints what the control lays over the rest of the menu, such as
   * an open list, once everything else is painted. */
  virtual void paintOverlay(IInterfaceCanvas& canvas, sf::IntRect area) const;

  /** @returns Whether MENU_CONFIRM_KEY, or a click on the label of the row,
   * does something. */
  [[nodiscard]] virtual bool isActivatable() const;

  /** @brief What MENU_CONFIRM_KEY does, once the row has looked pressed. */
  virtual void activate();

  /** @returns Whether every key and pointer event goes to the control for
   * now, as while a list is open or a thumb is dragged. */
  [[nodiscard]] virtual bool holdsInput() const;

  /** @returns Whether the control used the key: a step of DECREASE_KEY or
   * INCREASE_KEY, FIRST_VALUE_KEY..., or any key while it holds the input. An
   * unused key keeps its meaning in the menu. */
  virtual bool keyPressed(const sf::Event::KeyPressed& key);

  /** @brief A key released while the control holds the input. */
  virtual void keyReleased(const sf::Event::KeyReleased& key);

  /** @returns Whether the control used a left press at a pixel of its area;
   * an unused press presses the row, as a click on its label does. */
  virtual bool pointerPressed(sf::Vector2i pixel, sf::IntRect area);

  /** @brief The pointer moved, or the left button released, while the control
   * holds the input. */
  virtual void pointerMoved(sf::Vector2i pixel, sf::IntRect area);
  virtual void pointerReleased(sf::Vector2i pixel, sf::IntRect area);

  /** @brief Drops what the control holds, as when the window loses the
   * focus. */
  virtual void cancelInput();
};

}  // namespace rtype::client
