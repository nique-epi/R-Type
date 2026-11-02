#pragma once

#include <SFML/Graphics/Color.hpp>
#include <string>

namespace rtype::client {

/** @brief What a row of an ItemList shows in a column. */
struct ItemCell {
  std::string text;
  sf::Color color;
  /** @brief A word on ink right after the text of a cell of the regular font,
   * such as LOCAL_PLAYER_TAG; empty for none. */
  // NOLINTNEXTLINE(readability-redundant-member-init)
  std::string tag{};
};

}  // namespace rtype::client
