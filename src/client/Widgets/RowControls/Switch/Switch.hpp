#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include "BarState.hpp"
#include "IInterfaceCanvas.hpp"
#include "RowControl.hpp"
#include "ValueBinding.hpp"

namespace rtype::client {

/**
 * @brief A yes or no: SWITCH_OFF_TEXT and SWITCH_ON_TEXT side by side in a
 * sunk box, the current one filled. DECREASE_KEY sets it off, INCREASE_KEY on,
 * MENU_CONFIRM_KEY turns it over; a click on a word sets that word.
 */
class Switch : public RowControl {
 public:
  explicit Switch(ValueBinding<bool> value);

  [[nodiscard]] int width() const override;
  void paint(IInterfaceCanvas& canvas, sf::IntRect area,
             BarState row) const override;
  bool keyPressed(const sf::Event::KeyPressed& key) override;
  [[nodiscard]] bool isActivatable() const override;
  void activate() override;
  bool pointerPressed(sf::Vector2i pixel, sf::IntRect area) override;

 private:
  /** @returns The cell of a word: off on the left, on on the right. */
  [[nodiscard]] static sf::IntRect cellOf(bool onSide, sf::IntRect area);

  ValueBinding<bool> value_;
};

}  // namespace rtype::client
