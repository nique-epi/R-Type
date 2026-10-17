#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <vector>
#include "BarState.hpp"
#include "IInterfaceCanvas.hpp"
#include "RowControl.hpp"
#include "ValueBinding.hpp"

namespace rtype::client {

/**
 * @brief One choice of a list that opens under its box.
 *
 * Closed, a sunk box holds the current choice and a pointer down.
 * MENU_CONFIRM_KEY or a click opens the list, the current choice highlighted;
 * open, the list holds every key and pointer event: LIST_PREVIOUS_KEY and
 * LIST_NEXT_KEY move the highlight, skipping the choices that cannot be taken
 * and never wrapping; FIRST_VALUE_KEY and LAST_VALUE_KEY move it to the first
 * and last choice that can be taken; MENU_CONFIRM_KEY takes the highlighted
 * choice and closes; MENU_CANCEL_KEY and MENU_TAB_KEY close and change
 * nothing; the pointer highlights the choice it is over, a click takes it, a
 * click elsewhere closes. A choice that cannot be taken is shown, dimmed and
 * dashed, never taken.
 */
class Dropdown : public RowControl {
 public:
  /**
   * @param choices Texts of the choices, in order.
   * @param available Whether a choice can be taken, asked whenever needed.
   * @param value Order of the current choice.
   */
  Dropdown(std::vector<std::string> choices,
           std::function<bool(std::size_t)> available,
           ValueBinding<std::size_t> value);

  [[nodiscard]] int width() const override;
  void paint(IInterfaceCanvas& canvas, sf::IntRect area,
             BarState row) const override;
  void paintOverlay(IInterfaceCanvas& canvas, sf::IntRect area) const override;
  [[nodiscard]] bool isActivatable() const override;
  void activate() override;
  [[nodiscard]] bool holdsInput() const override;
  bool keyPressed(const sf::Event::KeyPressed& key) override;
  bool pointerPressed(sf::Vector2i pixel, sf::IntRect area) override;
  void pointerMoved(sf::Vector2i pixel, sf::IntRect area) override;
  void cancelInput() override;

  [[nodiscard]] bool isOpen() const;

  /** @returns The highlighted choice while the list is open, nothing
   * otherwise. */
  [[nodiscard]] std::optional<std::size_t> highlighted() const;

  /** @returns The choice under the pointer while the list is open, taken or
   * not, nothing otherwise. */
  [[nodiscard]] std::optional<std::size_t> pointedChoice() const;

 private:
  [[nodiscard]] sf::IntRect listArea(sf::IntRect area) const;
  [[nodiscard]] sf::IntRect itemArea(std::size_t choice,
                                     sf::IntRect area) const;
  [[nodiscard]] std::optional<std::size_t> choiceAt(sf::Vector2i pixel,
                                                    sf::IntRect area) const;
  /** @returns The nearest choice that can be taken from one, one way, or
   * nothing past the end of the list. */
  [[nodiscard]] std::optional<std::size_t> nextAvailable(std::size_t from,
                                                         bool forward) const;
  void take(std::size_t choice);
  void close();
  void paintPointer(IInterfaceCanvas& canvas, sf::IntRect box,
                    sf::Color color) const;

  std::vector<std::string> choices_;
  std::function<bool(std::size_t)> available_;
  ValueBinding<std::size_t> value_;
  bool open_ = false;
  std::size_t highlight_ = 0;
  std::optional<std::size_t> pointed_;
};

}  // namespace rtype::client
