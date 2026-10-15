#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <functional>
#include <string>
#include "ChamferedBox.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/**
 * @brief A bar of a menu: a label, and the action it runs when activated.
 *
 * It knows its area and paints itself in each State; which button is selected
 * or pressed, and when one is activated, is decided by the Menu that holds it.
 * A disabled button is never selected nor activated. A button that is one
 * choice among others shows a check box before its label.
 */
class Button {
 public:
  /** @brief How a bar looks: at rest, selected, pressed, or disabled. */
  enum class State : std::uint8_t {
    Idle,
    Selected,
    Pressed,
    Disabled,
  };

  /**
   * @brief The check box of a button: none for an ordinary button; empty, or
   * filled for the current one, for a button that is one choice among others.
   */
  enum class CheckMark : std::uint8_t {
    None,
    Unchecked,
    Checked,
  };

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
   */
  Button(Text text, sf::IntRect area, std::function<void()> action);

  [[nodiscard]] bool isEnabled() const;
  void setEnabled(bool enabled);
  void setMark(CheckMark mark);

  /** @returns Whether a pixel of the interface picture lies on the bar. */
  [[nodiscard]] bool contains(sf::Vector2i pixel) const;

  [[nodiscard]] const std::string& description() const;

  /** @brief Runs the action. */
  void activate() const;

  void paint(IInterfaceCanvas& canvas, State state) const;

 private:
  /** @brief Colors of a bar and of its label in one state, and how far the
   * label drops. */
  struct Look {
    ChamferedBoxColors bar;
    sf::Color text;
    int textDrop{};
  };

  [[nodiscard]] static Look lookOf(State state);

  /** @brief Paints the check box at the start of a line of label whose top is
   * given, its bottom on the baseline. */
  void paintCheckBox(IInterfaceCanvas& canvas, sf::Vector2i lineTop,
                     sf::Color color) const;

  Text text_;
  sf::IntRect area_;
  std::function<void()> action_;
  bool enabled_ = true;
  CheckMark mark_ = CheckMark::None;
};

}  // namespace rtype::client
