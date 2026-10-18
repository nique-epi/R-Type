#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include "BarState.hpp"
#include "CutCornerBox.hpp"
#include "IInterfaceCanvas.hpp"
#include "RowControl.hpp"
#include "SunkBox.hpp"
#include "TextField.hpp"

namespace rtype::client {

/**
 * @brief A bar of a menu: a label, and the action it runs when activated.
 *
 * It knows its area and paints itself in each BarState; which button is
 * selected or pressed, and when one is activated, is decided by the Menu that
 * holds it. A disabled button is never selected nor activated.
 *
 * A button that holds a text field is drawn as that field: its label in small
 * print at the top of its area, under it a box sunk into the panel holding the
 * text, and a caret after the text while the button is selected or pressed. Its
 * area covers the label and the box, so a click on either selects it.
 *
 * A row of a tabbed panel shows its label on the left and its control on the
 * right, ROW_PADDING in from the sides; at rest it shows its label alone, and
 * looks like a bar only while selected or pressed. Its control decides whether
 * MENU_CONFIRM_KEY presses it, and what activating it does.
 */
class Button {
 public:
  /**
   * @brief What a button says: its label, and the line the menu writes under
   * its buttons while it is selected, empty for none.
   */
  struct Text {
    std::string label;
    // NOLINTNEXTLINE(readability-redundant-member-init)
    std::string description{};
  };

  /**
   * @param text Label, and description written under the menu while the
   * button is selected.
   * @param area Where the bar lies, in pixels of the interface picture.
   * @param action Run each time the button is activated.
   * @param field Text field the bar shows after its label, none for a bar that
   * shows its label only.
   */
  Button(Text text, sf::IntRect area, std::function<void()> action,
         std::optional<TextField> field);

  /**
   * @brief A row: its label, and its control, never null.
   * @param area Where the row lies, in pixels of the interface picture.
   */
  Button(Text text, sf::IntRect area, std::unique_ptr<RowControl> control);

  [[nodiscard]] bool isEnabled() const;
  void setEnabled(bool enabled);
  void setDescription(std::string description);

  /** @returns Whether a pixel of the interface picture lies on the bar. */
  [[nodiscard]] bool contains(sf::Vector2i pixel) const;

  [[nodiscard]] const std::string& description() const;

  /** @returns The text field of the bar, or nullptr for a bar without one. */
  [[nodiscard]] TextField* field();
  [[nodiscard]] const TextField* field() const;

  /** @returns Whether MENU_CONFIRM_KEY, or a click, presses it: always for a
   * bar or a field, as its control says for a row. */
  [[nodiscard]] bool isActivatable() const;

  /** @returns The control of a row, or nullptr for a bar or a field. */
  [[nodiscard]] RowControl* control();
  [[nodiscard]] const RowControl* control() const;

  /** @returns Where the control of a row lies: as wide as it is now,
   * ROW_PADDING from the right side, ROW_CONTROL_TOP down; empty for a bar
   * or a field. */
  [[nodiscard]] sf::IntRect controlArea() const;

  /** @brief Runs the action, or activates the control of a row. */
  void activate();

  void paint(IInterfaceCanvas& canvas, BarState state) const;

  /** @brief Paints what the control of a row lays over the menu. */
  void paintOverlay(IInterfaceCanvas& canvas) const;

 private:
  /** @brief Colors of a bar and of its label in one state, and how far the
   * label drops. */
  struct Look {
    CutCornerBox::Colors bar;
    sf::Color text;
    int textDrop{};
  };

  /** @brief Colors of a text field and of its label in one state, the sides
   * of its box, and whether the caret follows its text. */
  struct FieldLook {
    sf::Color label;
    SunkBox::Sides sides{};
    sf::Color text;
    bool caret = false;
  };

  [[nodiscard]] static Look lookOf(BarState state);
  [[nodiscard]] static FieldLook fieldLookOf(BarState state);

  /** @brief Paints the bar as its text field: the label at the top of the area,
   * the box under it, the text inside, then the caret if the look has one. */
  void paintField(IInterfaceCanvas& canvas, BarState state,
                  const TextField& field) const;

  /** @brief Paints the bar as a row: a bar while selected or pressed, the
   * label, then the control. */
  void paintRow(IInterfaceCanvas& canvas, BarState state) const;

  Text text_;
  sf::IntRect area_;
  std::function<void()> action_;
  std::optional<TextField> field_;
  std::unique_ptr<RowControl> control_;
  bool enabled_ = true;
};

}  // namespace rtype::client
