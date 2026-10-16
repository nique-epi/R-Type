#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <string>
#include <vector>
#include "BarState.hpp"
#include "IInterfaceCanvas.hpp"
#include "RowControl.hpp"
#include "ValueBinding.hpp"

namespace rtype::client {

/**
 * @brief One choice of a few, written in a sunk box as wide as the longest,
 * with DECREASE_ARROW and INCREASE_ARROW on its sides while its row is
 * selected, the arrow at an end of the list hidden.
 *
 * DECREASE_KEY and INCREASE_KEY step to the neighbouring choice, without
 * wrapping; MENU_CONFIRM_KEY steps to the next one, wrapping around; a click
 * on an arrow steps, a click on the box does as MENU_CONFIRM_KEY.
 */
class ChoiceSelector : public RowControl {
 public:
  /** @param choices Texts of the choices, at least one. @param value Order of
   * the current choice. */
  ChoiceSelector(std::vector<std::string> choices,
                 ValueBinding<std::size_t> value);

  [[nodiscard]] int width() const override;
  void paint(IInterfaceCanvas& canvas, sf::IntRect area,
             BarState row) const override;
  bool keyPressed(const sf::Event::KeyPressed& key) override;
  [[nodiscard]] bool isActivatable() const override;
  void activate() override;
  bool pointerPressed(sf::Vector2i pixel, sf::IntRect area) override;

 private:
  [[nodiscard]] int boxWidth() const;
  [[nodiscard]] sf::IntRect boxArea(sf::IntRect area) const;

  std::vector<std::string> choices_;
  ValueBinding<std::size_t> value_;
};

}  // namespace rtype::client
