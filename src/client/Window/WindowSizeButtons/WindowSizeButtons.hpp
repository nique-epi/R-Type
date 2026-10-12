#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <vector>
#include "WindowSizeSelection.hpp"

namespace rtype::client {

/**
 * @brief Row of buttons, one per size of WINDOW_SIZES, drawn at the top-left
 * corner of the playfield and labelled with the size they give.
 *
 * The labels refer to the font given at construction, which must outlive this
 * object.
 *
 * Provisional: the buttons stand in for the options menu until that one
 * exists.
 */
class WindowSizeButtons {
 public:
  /** @brief Builds the buttons, their labels written with the given font. */
  explicit WindowSizeButtons(const sf::Font& labelFont);

  /**
   * @brief Colors each button after the state of its size: selected,
   * available, or too large for the desktop.
   */
  void showState(const WindowSizeSelection& selection);

  /**
   * @param point A point of the playfield, in playfield units.
   * @returns The index, in WINDOW_SIZES, of the size whose button contains the
   * point; an empty optional when the point is on no button.
   */
  [[nodiscard]] std::optional<std::size_t> indexAt(sf::Vector2f point) const;

  void draw(sf::RenderTarget& target) const;

 private:
  struct Colors {
    sf::Color button;
    sf::Color label;
  };

  [[nodiscard]] static Colors colorsOf(std::size_t index,
                                       const WindowSizeSelection& selection);

  std::vector<sf::RectangleShape> buttons_;
  std::vector<sf::Text> labels_;
};

}  // namespace rtype::client
