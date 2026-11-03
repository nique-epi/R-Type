#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @brief A word in small print on ink, such as LOCAL_PLAYER_TAG beside a
 * nickname. */
struct InkTag {
  std::string word;
  /** @brief Where the ink starts. */
  sf::Vector2i topLeft;
  sf::Color wordColor;
};

/** @returns How wide the ink behind @p word is. */
[[nodiscard]] int inkTagWidth(const IInterfaceCanvas& canvas,
                              const std::string& word);

void paintInkTag(IInterfaceCanvas& canvas, const InkTag& tag);

}  // namespace rtype::client
