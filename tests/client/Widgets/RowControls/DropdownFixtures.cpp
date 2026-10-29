#include "DropdownFixtures.hpp"
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include "DropdownTestConstants.hpp"

using rtype::client::testing::FIRST_LINE_MIDDLE;
using rtype::client::testing::LINE_HEIGHT;
using rtype::client::testing::OPEN_LIST;

sf::Vector2i lineMiddle(std::size_t line) {
  return {OPEN_LIST.position.x + (OPEN_LIST.size.x / 2),
          FIRST_LINE_MIDDLE + (static_cast<int>(line) * LINE_HEIGHT)};
}
