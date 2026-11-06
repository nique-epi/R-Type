#pragma once

#include <SFML/System/Vector2.hpp>
#include <string_view>
#include "LabelSide.hpp"

namespace rtype::client {

/** @brief The name of a star, on the side placeLabels() gave it; the chosen
 * star writes it in cyan, a server of another version in blue. */
struct StarName {
  sf::Vector2i point;
  std::string_view name;
  LabelSide side{LabelSide::None};
  bool chosen{};
  bool incompatible{};
};

}  // namespace rtype::client
