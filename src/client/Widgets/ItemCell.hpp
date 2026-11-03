#pragma once

#include <SFML/Graphics/Color.hpp>
#include <optional>
#include <string>

namespace rtype::client {

/** @brief What a row of an ItemList shows in a column. */
struct ItemCell {
  std::string text;
  sf::Color color;
  /** @brief A word on ink right after the text of a cell of the regular font,
   * such as LOCAL_PLAYER_TAG. */
  std::optional<std::string> tag = std::nullopt;
};

}  // namespace rtype::client
