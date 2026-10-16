#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <functional>
#include <string>
#include "BarState.hpp"
#include "IInterfaceCanvas.hpp"
#include "RowControl.hpp"

namespace rtype::client {

/**
 * @brief A word and LINK_SUFFIX on the right of a row that opens something,
 * such as "RESET >" for the question asked before the defaults are put back.
 * MENU_CONFIRM_KEY, or a click on the row, runs its action once the row has
 * looked pressed.
 */
class LinkText : public RowControl {
 public:
  LinkText(std::string text, std::function<void()> action);

  [[nodiscard]] int width() const override;
  void paint(IInterfaceCanvas& canvas, sf::IntRect area,
             BarState row) const override;
  [[nodiscard]] bool isActivatable() const override;
  void activate() override;

 private:
  std::string text_;
  std::function<void()> action_;
};

}  // namespace rtype::client
