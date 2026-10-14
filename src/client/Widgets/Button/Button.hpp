#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <string>
#include "ButtonState.hpp"
#include "ButtonText.hpp"
#include "ChamferedBox.hpp"
#include "CheckMark.hpp"
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/**
 * @brief A bar of a menu: a label, and the action it runs when activated.
 *
 * It knows its area and paints itself in each ButtonState; which button is
 * selected or pressed, and when one is activated, is decided by the Menu that
 * holds it. A disabled button is never selected nor activated. A button that
 * is one choice among others shows a check box before its label.
 */
class Button {
 public:
  /**
   * @param text Label, and description written under the menu while the
   * button is selected.
   * @param area Where the bar lies, in pixels of the interface picture.
   * @param action Run each time the button is activated.
   */
  Button(ButtonText text, sf::IntRect area, std::function<void()> action);

  [[nodiscard]] bool isEnabled() const;
  void setEnabled(bool enabled);
  void setMark(CheckMark mark);

  /** @returns Whether a pixel of the interface picture lies on the bar. */
  [[nodiscard]] bool contains(sf::Vector2i pixel) const;

  [[nodiscard]] const std::string& description() const;

  /** @brief Runs the action. */
  void activate() const;

  void paint(IInterfaceCanvas& canvas, ButtonState state) const;

 private:
  /** @brief Colors of a bar and of its label in one state, and how far the
   * label drops. */
  struct Look {
    ChamferedBoxColors bar;
    sf::Color text;
    int textDrop{};
  };

  [[nodiscard]] static Look lookOf(ButtonState state);

  /** @brief Paints the check box at the start of a line of label whose top is
   * given, its bottom on the baseline. */
  void paintCheckBox(IInterfaceCanvas& canvas, sf::Vector2i lineTop,
                     sf::Color color) const;

  ButtonText text_;
  sf::IntRect area_;
  std::function<void()> action_;
  bool enabled_ = true;
  CheckMark mark_ = CheckMark::None;
};

}  // namespace rtype::client
