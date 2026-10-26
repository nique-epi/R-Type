#include "PixelCircle.hpp"
#include <SFML/System/Vector2.hpp>
#include <vector>

namespace rtype::client {

std::vector<sf::Vector2i> circleOf(int radius) {
  std::vector<sf::Vector2i> pixels;
  int column = radius;
  int row = 0;
  int error = 1 - radius;
  while (column >= row) {
    for (const sf::Vector2i octant :
         {sf::Vector2i(column, row), sf::Vector2i(row, column),
          sf::Vector2i(-row, column), sf::Vector2i(-column, row),
          sf::Vector2i(-column, -row), sf::Vector2i(-row, -column),
          sf::Vector2i(row, -column), sf::Vector2i(column, -row)}) {
      pixels.push_back(octant);
    }
    ++row;
    if (error < 0) {
      error += (2 * row) + 1;
    } else {
      --column;
      error += (2 * (row - column)) + 1;
    }
  }
  return pixels;
}

}  // namespace rtype::client
